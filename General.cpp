/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "General.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    /**
     * @brief Constructs a General player and registers them in the game.
     * @param game The Game object the player is part of.
     * @param name The name of the General player.
     */
    General::General(Game& game, const string& name)
        : Player(game, name, "General") {}

    /**
     * @brief Checks whether this General is currently protecting the given player.
     * @param target The player to check protection status for.
     * @return True if the General is protecting the target, false otherwise.
     */
    bool General::is_protecting(Player* target) const {
        return protected_players.count(target) > 0;
    }

    /**
     * @brief Refunds 1 coin to the General, typically after being arrested.
     */
    void General::refund_arrest_coin() {
        add_coins(1); // הגנרל מקבל חזרה את המטבע שנלקח ממנו
    }

}
