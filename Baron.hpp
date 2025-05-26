/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include "Player.hpp"

namespace coup {

    class Baron : public Player {
    public:
        // Constructor: Initializes a Baron player with a game reference and name
        Baron(Game& game, const std::string& name);

        // Performs an investment action, deducting 3 coins and adding 6, then advancing the turn
        void invest();

        // Adds 1 coin as compensation when the Baron is targeted by a sanction
        void receive_sanction_penalty();  // ייקרא כאשר שחקן אחר עושה עליו sanction
    };

}