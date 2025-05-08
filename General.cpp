#include "General.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    General::General(Game& game, const string& name)
        : Player(game, name, "General") {}

    void General::block_coup(Player& target) {
        if (!is_active()) throw runtime_error("General is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        if (coins() < 5) throw runtime_error("Not enough coins to block coup.");

        deduct_coins(5);

        // מחזירים את המטרה לחיים (אם בוצעה עליו הפיכה)
        target.set_active(true);
        target.clear_last_action();
    }

    void General::refund_arrest_coin() {
        add_coins(1); // הגנרל מקבל חזרה את המטבע שנלקח ממנו
    }

}
