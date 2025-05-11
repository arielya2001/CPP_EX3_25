
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
        : game(game), player_name(name), role_name(role), coin_count(0), active(true),
          was_arrested_last(false), gather_blocked(false), sanctioned(false), bonus_turns(0) {
    }

    void Player::gather() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (sanctioned) throw runtime_error("You are sanctioned and cannot gather.");

        add_coins(1);
        last_action = "gather";
        game.next_turn();
    }

    void Player::tax() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");
        if (sanctioned) throw runtime_error("You are sanctioned and cannot tax.");

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

        bonus_turns += 1;  // ניתן תור בונוס, יבוטל אם השופט יחסום
        last_action = "bribe";

        game.set_awaiting_bribe_block(true);
        game.set_bribing_player(this);

        // חפש שופט חי – אם קיים, העבר אליו את התור
        for (Player* p : game.get_all_players()) {
            if (p->is_active() && p->role() == "Judge") {
                game.set_turn_to(p);
                return;
            }
        }

        // אם אין שופט – לא ניתן לבטל, ממשיכים ישירות לבונוס של המשחד
        game.set_awaiting_bribe_block(false);
        game.set_bribing_player(nullptr);
        game.set_turn_to(this);  // תור חוזר למשחד
    }



    void Player::arrest(Player& target) {
        if (!active) {
            throw runtime_error("Inactive player cannot play.");
        }
        if (game.turn() != name()) {
            throw runtime_error("Not your turn.");
        }
        if (!target.is_active()) {
            throw runtime_error("Target player is not active.");
        }
        if (last_arrested_target == &target) {
            throw runtime_error("Cannot arrest the same player twice in a row.");
        }

        // בדיקה אם יש Spy שחוסם את ה-arrest
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
        // עדכון מצב
        last_arrested_target = &target;  // שומר מי נעצר על ידי
        last_action = "arrest";
        game.next_turn();
    }






    void Player::sanction(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");

        // בדיקת עלות כוללת מראש (כולל אם זה שופט)
        int required_coins = 3;
        if (dynamic_cast<Judge*>(&target)) {
            required_coins += 1;  // עלות נוספת בגלל Judge
        }

        if (coin_count < required_coins) {
            throw runtime_error("Not enough coins for sanction.");
        }

        // נגבים קודם כל 3 מטבעות
        deduct_coins(3);

        // הגדרת הסנקציה
        target.set_sanctioned(true);
        target.set_gather_blocked(true);

        // השלמה: שופט מחייב את התוקף לעוד מטבע
        if (Judge* judge = dynamic_cast<Judge*>(&target)) {
            judge->on_sanctioned(*this);
        }

        // ברון מקבל קנס
        if (Baron* baron = dynamic_cast<Baron*>(&target)) {
            baron->receive_sanction_penalty();
        }

        last_action = "sanction";
        game.next_turn();
    }



    void Player::coup(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target already out.");
        if (coin_count < 7) throw runtime_error("Not enough coins to perform coup.");

        deduct_coins(7);
        last_action = "coup";

        // בדוק אם יש גנרל חי
        for (Player* p : game.get_all_players()) {
            if (p->is_active() && p->role() == "General" && p->coins() >= 5) {
                game.set_awaiting_coup_block(true);
                game.set_coup_attacker(this);
                game.set_coup_target(&target);
                game.set_turn_to(p);
                return;
            }
        }

        // אם אין גנרל חי – מבצעים הפיכה רגילה
        target.set_couped(true);
        target.deactivate();
        std::cout << name() << " performed coup on " << target.name() << std::endl;
        game.next_turn();
    }


    void Player::on_turn_start() {
        was_arrested_last = false;
        gather_blocked = false;
        last_arrested_target = nullptr;
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
    void Player::set_role_name(const std::string& role) {
        this->role_name = role;
    }







}
