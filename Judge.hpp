/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include "Player.hpp"

namespace coup {

    class Judge : public Player {
    public:
        Judge(Game& game, const std::string& name);

        // חוסם שוחד
        void block_bribe(Player& target);

        // מקבל פיצוי נוסף במקרה של סנקציה
        void on_sanctioned(Player& attacker);
    };
}
