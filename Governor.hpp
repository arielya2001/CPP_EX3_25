/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include "Player.hpp"

namespace coup {

    class Governor : public Player {
    public:
        /// Constructor for Governor role
        Governor(Game& game, const std::string& name);

        /// Takes 3 coins and may trigger tax block phase
        void tax() override;

        /// Blocks a tax action and refunds coins
        void block_tax(Player& target);

        /// Skips the tax block and resumes the game
        void skip_tax_block();
    };

}
