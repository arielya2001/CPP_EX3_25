/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include "Player.hpp"

namespace coup {

    class Merchant : public Player {
    public:
        /// Constructor: initializes a Merchant player with the given name.
        Merchant(Game& game, const std::string& name);

        /// Called when another player performs a bribe on the Merchant.
        void on_bribed_by(Player& source);

        /// Called automatically at the start of the Merchant's turn.
        /// Grants bonus coin if conditions are met.
        void on_turn_start() override;
    };

}
