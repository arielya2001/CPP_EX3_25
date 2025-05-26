/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Player.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    /// Constructs a new player with a given name and role
    Player::Player(Game& game, const string& name, const string& role)
        : game(game), player_name(name), role_name(role), coin_count(0), active(true),
          was_arrested_last(false), gather_blocked(false), sanctioned(false), bonus_turns(0), last_arrested_target(nullptr)
    {
    }

    /// Basic gather action: earns 1 coin if allowed
    void Player::gather() {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (sanctioned) throw runtime_error("You are sanctioned and cannot gather.");

        last_arrested_target = nullptr;
        add_coins(1);
        last_action = "gather";
        std::cout << "[Debug] Player " << name() << " performed gather.\n";
        game.next_turn();
    }

    /// Performs a basic tax (2 coins), with possible block by Governor
    void Player::tax() {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot act.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (sanctioned) throw runtime_error("You are under sanction and cannot tax.");

        add_coins(2);
        last_action = "tax";

        for (Player* p : game.get_all_players()) {
            if (p->is_active() && p->role() == "Governor" && p != this) {
                game.set_last_tax_index_if_needed(game.get_player_index(this));  // ✅ only if new
                game.init_tax_blockers(this);
                game.set_awaiting_tax_block(true);
                game.set_tax_target(this);
                game.set_tax_source(p);
                game.set_turn_to(p);
                return;
            }
        }
        last_arrested_target = nullptr;
        game.next_turn();
    }

    /// Performs a bribe action: pays 4 coins, may be blocked by Judges
    void Player::bribe() {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count < 4) throw runtime_error("Not enough coins for bribe.");

        deduct_coins(4);
        bonus_turns += 1;
        last_action = "bribe";

        game.set_awaiting_bribe_block(true);
        game.set_bribing_player(this);
        game.init_bribe_blockers(this);

        Player* nextJudge = game.pop_next_bribe_blocker();
        if (nextJudge) {
            game.set_turn_to(nextJudge);
            return;
        }
        last_arrested_target = nullptr;

        game.set_awaiting_bribe_block(false);
        game.set_bribing_player(nullptr);
        game.set_turn_to(this);
    }

    /// Arrests another player, with logic for Spy, Merchant, General
    void Player::arrest(Player& target) {
        if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players.");
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");

        std::cout << "[Debug] arrest: " << name()
                  << " wants to arrest " << target.name()
                  << " [last_arrested_target="
                  << (last_arrested_target ? last_arrested_target->name() : "nullptr")
                  << "], bonus_turns=" << bonus_turns << std::endl;

        if (last_arrested_target == &target) {
            throw runtime_error("Cannot arrest the same player twice in a row.");
        }

        for (Player* p : game.get_all_players()) {
            Spy* spy = dynamic_cast<Spy*>(p);
            if (spy && spy->is_active() && spy->is_arrest_blocked(this)) {
                throw runtime_error("Arrest blocked by Spy.");
            }
        }

        if (Merchant* merchant = dynamic_cast<Merchant*>(&target)) {
            merchant->deduct_coins(2);
            std::cout << merchant->name() << " paid 2 coins due to arrest (Merchant penalty).\n";
        } else {
            target.deduct_coins(1);
            this->add_coins(1);

            if (General* general = dynamic_cast<General*>(&target)) {
                general->refund_arrest_coin();
                this->deduct_coins(1);
                std::cout << general->name() << " refunded coin due to arrest (General ability).\n";
            }
        }

        last_arrested_target = &target;
        last_action = "arrest";

        std::cout << "[Debug] arrest complete: " << name()
                  << " arrested " << target.name()
                  << ", bonus_turns=" << bonus_turns << std::endl;

        game.next_turn();
    }

    /// Performs a sanction against another player (3 coins)
    void Player::sanction(Player& target) {
        if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players.");
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");

        int required_coins = 3;
        if (dynamic_cast<Judge*>(&target)) {
            required_coins += 1;
        }

        if (coin_count < required_coins) {
            throw runtime_error("Not enough coins for sanction.");
        }

        deduct_coins(3);
        target.set_sanctioned(true);
        target.set_gather_blocked(true);

        if (Judge* judge = dynamic_cast<Judge*>(&target)) {
            judge->on_sanctioned(*this);
        }
        if (Baron* baron = dynamic_cast<Baron*>(&target)) {
            baron->receive_sanction_penalty();
        }

        last_arrested_target = nullptr;
        last_action = "sanction";
        game.next_turn();
    }

    /// Performs a coup action, or initiates block opportunity
    void Player::coup(Player& target) {
        if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players.");
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target already out.");
        if (coin_count < 7) throw runtime_error("Not enough coins to perform coup.");

        deduct_coins(7);
        last_action = "coup";

        for (Player* p : game.get_all_players()) {
            if (p->is_active() && p->role() == "General" && p->coins() >= 5) {
                game.set_awaiting_coup_block(true);
                game.set_coup_attacker(this);
                game.set_coup_target(&target);
                game.set_turn_to(p);
                return;
            }
        }

        last_arrested_target = nullptr;
        target.set_couped(true);
        target.deactivate();
        std::cout << name() << " performed coup on " << target.name() << std::endl;
        game.next_turn();
    }

    /// Called at the beginning of each turn
    void Player::on_turn_start() {
        was_arrested_last = false;
        gather_blocked = false;
    }

    /// Getters and utility methods
    string Player::name() const { return player_name; }
    string Player::role() const { return role_name; }
    int Player::coins() const { return coin_count; }
    bool Player::is_active() const { return active; }
    void Player::add_coins(int amount) { coin_count += amount; }

    void Player::deduct_coins(int amount) {
        if (coin_count < amount) throw runtime_error("Not enough coins.");
        coin_count -= amount;
    }

    void Player::deactivate() { active = false; }

    bool Player::was_arrested_recently() const { return was_arrested_last; }
    void Player::set_arrested_recently(bool val) { was_arrested_last = val; }

    bool Player::has_bonus_turn() const { return bonus_turns > 0; }

    void Player::use_bonus_turn() {
        if (bonus_turns > 0) {
            --bonus_turns;
        }
    }

    void Player::give_bonus_turns(int turns) { bonus_turns += turns; }
    void Player::set_active(bool val) { active = val; }
    void Player::clear_last_action() { last_action = ""; }
    void Player::set_role_name(const std::string& role) { this->role_name = role; }

}
