#pragma once

#include <string>
#include "Game.h"

namespace coup {

    class Player {
    protected:
        Game& game;
        std::string player_name;
        std::string role_name;
        int coin_count;
        bool active;
        bool was_arrested_last; // למניעת arrest כפול

    public:
        Player(Game& game, const std::string& name, const std::string& role);
        virtual ~Player() = default;

        // פעולות בסיסיות
        virtual void gather();
        virtual void tax();
        virtual void bribe();
        virtual void arrest(Player& target);
        virtual void sanction(Player& target);
        virtual void coup(Player& target);

        // פעולות גישה
        std::string name() const;
        std::string role() const;
        int coins() const;
        bool is_active() const;

        // ניהול מצב
        void add_coins(int amount);
        void deduct_coins(int amount);
        void deactivate(); // אם הודח

        // יכולת לדעת אם מישהו עשה עליו arrest לאחרונה
        bool was_arrested_recently() const;
        void set_arrested_recently(bool val);
    };

}
