#include "General.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    General::General(Game& game, const string& name)
        : Player(game, name, "General") {}

    void General::protect_from_coup(Player& target) {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) throw runtime_error("General is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        if (coins() < 5) throw runtime_error("Not enough coins to protect.");


        deduct_coins(5);
        protected_players.insert(&target);
        std::cout << name() << " is now protecting " << target.name() << " from coup." << std::endl;
    }

    bool General::is_protecting(Player* target) const {
        return protected_players.count(target) > 0;
    }

    void General::refund_arrest_coin() {
        add_coins(1); // הגנרל מקבל חזרה את המטבע שנלקח ממנו
    }


}
