#pragma once

#include "Player.hpp"

namespace coup {

    class Baron : public Player {
    public:
        Baron(Game& game, const std::string& name);

        void invest();
        void receive_sanction_penalty();  // ייקרא כאשר שחקן אחר עושה עליו sanction
    };

}
