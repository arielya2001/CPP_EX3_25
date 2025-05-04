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

        // החסימה נעשתה, אבל לפי המטלה — שחקן שעשה את ה־coup עדיין איבד את הכסף
        // לכן אנחנו רק "מונעים" את ההדחה בפועל

        // מחזירים את המטרה לחיים אם הייתה מודחת (אפשר להשתמש ב-set_active בעתיד אם תוסיף)
        target.add_coins(0); // placeholder – אולי נוסיף setActive(true) בהמשך
    }

    void General::refund_arrest_coin() {
        add_coins(1); // הגנרל מקבל חזרה את המטבע שנלקח ממנו
    }

}
