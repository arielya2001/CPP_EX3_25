/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include "Player.hpp"
#include <unordered_set>

namespace coup {

    /**
     * @brief The General class represents a player with the ability to protect others from a coup.
     */
    class General : public Player {
    private:

    public:
        /**
         * @brief Constructs a General and registers them in the game.
         * @param game The game this General is part of.
         * @param name The name of the player.
         */
        General(Game& game, const std::string& name);

        /**
         * @brief A set of players currently protected by this General.
         */
        std::unordered_set<Player*> protected_players;

        /**
         * @brief Checks whether a specific player is currently protected by this General.
         * @param target The player to check.
         * @return True if the player is protected, false otherwise.
         */
        bool is_protecting(Player* target) const;

        /**
         * @brief Automatically triggered when the General is arrested — refunds 1 coin.
         */
        void refund_arrest_coin();
    };

}
