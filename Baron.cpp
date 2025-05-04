#include "Baron.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    Baron::Baron(Game& game, const string& name)
        : Player(game, name, "Baron") {}

    void Baron::invest() {
        if (!is_active()) throw runtime_error("Baron is not active.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coins() < 3) throw runtime_error("Not enough coins to invest.");
        if (coins() >= 10) throw runtime_error("Must coup with 10 coins.");

        deduct_coins(3);
        add_coins(6);
        game.next_turn();
    }

    void Baron::receive_sanction_penalty() {
        // מקבל מטבע "פיצוי" כשמישהו עושה עליו sanction
        add_coins(1);
    }

}
