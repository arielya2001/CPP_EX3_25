#pragma once

#include "Player.hpp"
#include <set>

namespace coup {

    class Spy : public Player {
    private:
        std::set<Player*> blocked_arrests;

    public:
        Spy(Game& game, const std::string& name);

        // יכולת לרגל - מחזיר מספר מטבעות של שחקן אחר
        int spy_on(Player& target) const;

        // חסימת arrest - "מחסן" שחקן
        void block_arrest(Player& target);

        // פונקציה לבדיקה אם שחקן חסום (נשתמש מחוץ למחלקה הזו)
        bool is_arrest_blocked(Player* p) const;
    };

}
