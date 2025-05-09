#include "Judge.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    Judge::Judge(Game& game, const string& name)
        : Player(game, name, "Judge") {}

    void Judge::block_bribe(Player& target) {
        if (!is_active()) throw runtime_error("Judge is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        if (target.get_last_action() != "bribe") throw runtime_error("Target did not perform bribe.");

        // ביטול תור הבונוס
        target.use_bonus_turn();  // או: target.give_bonus_turns(-1);

        // מחיקת הפעולה
        target.clear_last_action();

        // סיום מצב שוחד
        game.set_awaiting_bribe_block(false);
        game.set_bribing_player(nullptr);

        // החזרת התור לשחקן
        game.set_turn_to(&target);  // חשוב שמימשת set_turn_to ב-Game
    }


    void Judge::on_sanctioned(Player& attacker) {
        if (!is_active() || !attacker.is_active()) return;

        attacker.deduct_coins(1);  // קנס נוסף
    }

}
