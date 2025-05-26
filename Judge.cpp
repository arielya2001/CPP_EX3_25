/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Judge.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    /// Constructs a Judge player with the given name
    Judge::Judge(Game& game, const string& name)
        : Player(game, name, "Judge") {}

    /// Blocks a bribe action, cancels bonus turn, clears bribe state, and continues game
    void Judge::block_bribe(Player& target) {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
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

    /// Applies an extra penalty to an attacker who sanctioned the judge
    void Judge::on_sanctioned(Player& attacker) {
        if (!is_active() || !attacker.is_active()) return;

        attacker.deduct_coins(1);  // קנס נוסף
    }

}
