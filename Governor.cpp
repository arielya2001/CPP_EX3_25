#include "Governor.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    Governor::Governor(Game& game, const std::string& name)
        : Player(game, name, "Governor") {}

    void Governor::tax() {
        if (!is_active()) throw runtime_error("Inactive player cannot act.");
        if (game.turn() != name()) throw runtime_error("Not your turn.");
        if (coins() >= 10) throw runtime_error("Must perform coup with 10 coins.");

        add_coins(3);
        game.next_turn();
    }

    void Governor::undo(Player& target) {
        if (!is_active()) throw runtime_error("Governor is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        if (target.get_last_action() != "tax") {
            throw runtime_error("Cannot undo: last action was not tax.");
        }


        // בינתיים נניח ש־Spy הוא היחיד שעושה tax רגיל (2 מטבעות)
        if (target.role() != "Spy") {
            throw runtime_error("Can only undo tax for Spy.");
        }

        if (target.coins() < 2) {
            throw runtime_error("Target doesn't have enough coins to undo.");
        }

        target.deduct_coins(2);
    }

}
