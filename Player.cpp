#include "Player.h"
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

        add_coins(1);
        game.next_turn();
    }

    void Player::tax() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins.");

        add_coins(2);
        game.next_turn();
    }

    void Player::bribe() {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count < 4) throw runtime_error("Not enough coins for bribe.");

        deduct_coins(4);
        // הפעולה הנוספת תתבצע אחר כך מבחינת המשתמש
        // כאן רק "מאשרים" את הזכות לפעולה נוספת
    }

    void Player::arrest(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");
        if (target.was_arrested_recently()) throw runtime_error("Cannot arrest the same player twice in a row.");

        target.set_arrested_recently(true);
        add_coins(1);
        target.deduct_coins(1);
        game.next_turn();
    }

    void Player::sanction(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coin_count < 3) throw runtime_error("Not enough coins for sanction.");
        if (!target.is_active()) throw runtime_error("Target player is not active.");

        deduct_coins(3);
        // הפעולה עצמה של "לחסום" תהיה דרך השחקן שהותקף (בתפקידים כמו Baron)
        game.next_turn();
    }

    void Player::coup(Player& target) {
        if (!active) throw runtime_error("Inactive player cannot play.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (!target.is_active()) throw runtime_error("Target already out.");
        if (coin_count < 7) throw runtime_error("Not enough coins to perform coup.");

        deduct_coins(7);
        target.deactivate();
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

}
