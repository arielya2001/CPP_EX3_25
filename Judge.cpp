/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Judge.hpp"                          // Include Judge class definition
#include <stdexcept>                          // For runtime_error exception

using namespace std;

namespace coup {

    /// Constructs a Judge player with the given name
    Judge::Judge(Game& game, const string& name)
        : Player(game, name, "Judge") {}      // Call base constructor with role "Judge"

    /// Blocks a bribe action, cancels bonus turn, clears bribe state, and continues game
    void Judge::block_bribe(Player& target) {
        if (game.num_players() < 2) {                             // Game must have at least 2 players
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) throw runtime_error("Judge is not active.");           // Judge must be active
        if (!target.is_active()) throw runtime_error("Target is not active.");   // Target must be active
        if (target.get_last_action() != "bribe") throw runtime_error("Target did not perform bribe."); // Validate action

        target.use_bonus_turn();                               // Cancel bonus turn from bribe
        target.clear_last_action();                            // Clear the "bribe" action

        game.set_awaiting_bribe_block(false);                  // Reset bribe block state
        game.set_bribing_player(nullptr);                      // Clear the briber reference

        int index = game.get_player_index(&target);            // Get target's index in player list
        const auto& players = game.get_all_players();          // Access full player list
        do {
            index = (index + 1) % players.size();              // Find next active player
        } while (!players[index]->is_active());

        game.set_turn_to(players[index]);                      // Set turn to next active player
    }

    /// Applies an extra penalty to an attacker who sanctioned the judge
    void Judge::on_sanctioned(Player& attacker) {
        if (!is_active() || !attacker.is_active()) return;     // Both players must be active

        attacker.deduct_coins(1);                              // Apply extra 1-coin penalty
    }

}
