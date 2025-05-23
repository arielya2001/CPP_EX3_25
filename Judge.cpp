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
        target.use_bonus_turn();

        // מחיקת הפעולה
        target.clear_last_action();

        // סיום מצב שוחד
        game.set_awaiting_bribe_block(false);
        game.set_bribing_player(nullptr);

        // מצא את התור הבא אחרי המשחד
        int index = game.get_player_index(&target);
        const auto& players = game.get_all_players();
        do {
            index = (index + 1) % players.size();
        } while (!players[index]->is_active());

        game.set_turn_to(players[index]);
    }



    void Judge::on_sanctioned(Player& attacker) {
        if (!is_active() || !attacker.is_active()) return;

        attacker.deduct_coins(1);  // קנס נוסף
    }

}