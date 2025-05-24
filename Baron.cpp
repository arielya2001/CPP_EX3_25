#include "Baron.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    // Constructor: Initializes a Baron player with a game reference and name
    Baron::Baron(Game& game, const string& name)
        : Player(game, name, "Baron") {}

    // Performs an investment action, deducting 3 coins and adding 6, then advancing the turn
    void Baron::invest() {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) throw runtime_error("Baron is not active.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coins() < 3) throw runtime_error("Not enough coins to invest.");
        if (coins() >= 10) throw runtime_error("Must coup with 10 coins.");


        deduct_coins(3);
        add_coins(6);
        game.next_turn();
    }

    // Adds 1 coin as compensation when the Baron is targeted by a sanction
    void Baron::receive_sanction_penalty() {
        // מקבל מטבע "פיצוי" כשמישהו עושה עליו sanction
        add_coins(1);
    }

}