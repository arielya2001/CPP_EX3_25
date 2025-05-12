#pragma once

#include "Player.hpp"

namespace coup {

    class Governor : public Player {
    public:
        Governor(Game& game, const std::string& name);

        void tax() override;
        void undo(Player& target) override;
        void block_tax(Player& target);
        void skip_tax_block();

    };

}
