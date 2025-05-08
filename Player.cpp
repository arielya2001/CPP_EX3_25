#include "Player.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    Player::Player(Game& game, const string& name, const string& role)
        : game(game), player_name(name), role_name(role), coin_count(0), active(true), was_arrested_last(false) {
        game.add_player(this);
    }

    void Player::gather() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (gather_blocked) {
            gather_blocked = false; // נחסם רק לפעם אחת
            throw runtime_error("You are blocked from gathering due to sanction.");
        }
        if (is_under_sanction()) throw runtime_error("You are under sanction and cannot gather.");
        add_coins(1);
        last_action = "gather";
        game.next_turn();
    }

    void Player::tax() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (is_under_sanction()) throw runtime_error("You are under sanction and cannot gather.");

        add_coins(2);
        last_action = "tax";
        game.next_turn();
    }

    void Player::bribe(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count < 4) throw runtime_error("Not enough coins for bribe.");

        deduct_coins(4);

        // אם היעד הוא Merchant – הוא מקבל מטבע
        Merchant* merchant = dynamic_cast<Merchant*>(&target);
        if (merchant != nullptr && merchant->is_active()) {
            merchant->on_bribed_by(*this);
        }

        // ✅ השחקן מקבל שני תורות בונוס
        bonus_turns += 1;
        last_action = "bribe";

    }


    void Player::arrest(Player& target) {
        if (!active) {
            throw runtime_error("Inactive player cannot play.");}
        if (game.turn() != name()) {
            throw runtime_error("Not your turn.");}
        if (!target.is_active()) {
            throw runtime_error("Target player is not active.");}
        if (target.was_arrested_recently()) {
            throw runtime_error("Cannot arrest the same player twice in a row.");}
        // בדיקה אם יש Spy שחוסם את ה-arrest
        for (Player* p : game.get_all_players()) {
            Spy* spy = dynamic_cast<Spy*>(p);
            if (spy && spy->is_active() && spy->is_arrest_blocked(&target)) {
                throw runtime_error("Arrest blocked by Spy.");}}
        // אם target הוא General – החזר לו את המטבע
        General* general = dynamic_cast<General*>(&target);
        if (general != nullptr) {
            general->refund_arrest_coin();
        } else {
            target.deduct_coins(1);
        }
        // עדכון מצב
        target.set_arrested_recently(true);
        add_coins(1);
        last_action = "arrest";
        game.next_turn();
    }



    void Player::sanction(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count < 3) throw runtime_error("Not enough coins for sanction.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");

        deduct_coins(3);
        target.set_sanctioned(true);
        Judge* judge = dynamic_cast<Judge*>(&target);
        if (judge != nullptr) {
            judge->on_sanctioned(*this);  // 'this' הוא התוקף
        }
        // ✅ אם השחקן המותקף הוא Baron – נפצה אותו
        Baron* baron = dynamic_cast<Baron*>(&target);
        if (baron != nullptr) {
            baron->receive_sanction_penalty();
        }
        target.set_gather_blocked(true);
        last_action = "sanction";
        game.next_turn();
    }

    void Player::coup(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target already out.");
        if (coin_count < 7) throw runtime_error("Not enough coins to perform coup.");

        deduct_coins(7);
        target.deactivate();
        last_action = "coup";
        game.next_turn();
    }

    string Player::name() const {
        return player_name;
    }

    string Player::role() const {
        return role_name;
    }

    int Player::coins() const {
        return coin_count;
    }

    bool Player::is_active() const {
        return active;
    }

    void Player::add_coins(int amount) {
        coin_count += amount;
    }

    void Player::deduct_coins(int amount) {
        if (coin_count < amount) throw runtime_error("Not enough coins.");
        coin_count -= amount;
    }

    void Player::deactivate() {
        active = false;
    }

    bool Player::was_arrested_recently() const {
        return was_arrested_last;
    }

    void Player::set_arrested_recently(bool val) {
        was_arrested_last = val;
    }
    void Player::undo(Player& target) {
        (void)target;  // מדכא warning על unused parameter
        throw std::runtime_error("This role cannot perform undo.");
    }
    bool Player::has_bonus_turn() const {
        return bonus_turns > 0;
    }

    void Player::use_bonus_turn() {
        if (bonus_turns > 0) {
            --bonus_turns;
        }
    }

    void Player::give_bonus_turns(int turns) {
        bonus_turns += turns;
    }
    void Player::set_active(bool val) {
        active = val;
    }
    void Player::clear_last_action() {
        last_action = "";
    }






}
