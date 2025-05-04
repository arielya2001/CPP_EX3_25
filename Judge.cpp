#include "Judge.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    Judge::Judge(Game& game, const string& name)
        : Player(game, name, "Judge") {}

    void Judge::block_bribe(Player& target) {
        if (!is_active()) throw runtime_error("Judge is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        if (target.coins() < 4) throw runtime_error("Target did not bribe.");

        // מחזירים לו את 4 המטבעות ששילם על bribe
        target.add_coins(4);
    }

    void Judge::on_sanctioned(Player& attacker) {
        if (!is_active() || !attacker.is_active()) return;

        attacker.deduct_coins(1);  // קנס נוסף
    }

}
