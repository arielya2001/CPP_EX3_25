#pragma once

#include "Player.hpp"
#include <set>
#include <unordered_map>

namespace coup {

    class Spy : public Player {
    private:
        std::unordered_map<Player*, int> blocked_arrests;

    public:
        Spy(Game& game, const std::string& name);

        // יכולת לרגל - מחזיר מספר מטבעות של שחקן אחר
        int spy_on(Player& target) const;

        // חסימת arrest - "מחסן" שחקן
        void block_arrest(Player& target);

        // פונקציה לבדיקה אם שחקן חסום (נשתמש מחוץ למחלקה הזו)
        bool is_arrest_blocked(Player* p) const;
        void clear_expired_blocks();

    };

}
