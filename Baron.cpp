/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Baron.hpp"                               // Include the Baron class definition
#include <stdexcept>                               // Include standard exception handling

using namespace std;                               // Use standard namespace for strings and exceptions

namespace coup {                                   // Define everything inside the coup namespace

    // Constructor: Initializes a Baron player with a game reference and name
    Baron::Baron(Game& game, const string& name)   // Constructor for the Baron role
        : Player(game, name, "Baron") {}           // Call base Player constructor with role name

    // Performs an investment action, deducting 3 coins and adding 6, then advancing the turn
    void Baron::invest() {
        if (game.num_players() < 2) {                      // Check if the game has enough players
            throw runtime_error("Game has not started – need at least 2 players."); // Throw if not
        }
        if (!is_active()) throw runtime_error("Baron is not active.");              // Check if player is still in the game
        if (game.turn() != name()) throw runtime_error("Not your turn.");           // Check if it's the player's turn
        if (coins() < 3) throw runtime_error("Not enough coins to invest.");        // Must have at least 3 coins to invest
        if (coins() >= 10) throw runtime_error("Must coup with 10 coins.");         // Must coup if holding 10 or more coins

        deduct_coins(3);      // Subtract 3 coins for the investment
        add_coins(6);         // Gain 6 coins as return on investment
        game.next_turn();     // Advance to the next player's turn
    }

    // Adds 1 coin as compensation when the Baron is targeted by a sanction
    void Baron::receive_sanction_penalty() {
        // מקבל מטבע "פיצוי" כשמישהו עושה עליו sanction
        add_coins(1);   // Add 1 coin as a compensation
    }

} // End of namespace coup
