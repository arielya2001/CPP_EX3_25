#include "General.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    General::General(Game& game, const string& name)
        : Player(game, name, "General") {}

    bool General::is_protecting(Player* target) const {
        return protected_players.count(target) > 0;
    }

    void General::refund_arrest_coin() {
        add_coins(1); // הגנרל מקבל חזרה את המטבע שנלקח ממנו
    }


}
