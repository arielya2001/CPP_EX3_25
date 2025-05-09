#pragma once

#include <string>
#include "Game.hpp"

namespace coup {

    class Player {
    protected:
        Game& game;
        std::string player_name;
        std::string role_name;
        int coin_count;
        bool active;
        bool was_arrested_last; // למניעת arrest כפול
        int bonus_turns = 0;  // NEW
        std::string last_action;
        bool gather_blocked = false;
        bool is_sanctioned = false;
        bool couped_flag = false;

    public:
        Player(Game& game, const std::string& name, const std::string& role);
        const Game& getGame() const {
            return game;
        }

        virtual ~Player() = default;

        // פעולות בסיסיות
        virtual void gather();
        virtual void tax();
        virtual void bribe(Player& target);
        virtual void arrest(Player& target);
        virtual void sanction(Player& target);
        virtual void coup(Player& target);
        virtual void undo(Player& target);

        // פעולות גישה
        std::string name() const;
        std::string role() const;
        int coins() const;
        bool is_active() const;

        // ניהול מצב
        void add_coins(int amount);
        void deduct_coins(int amount);
        void deactivate(); // אם הודח
        void set_active(bool val);  // מאפשר להחזיר שחקן למשחק

        // יכולת לדעת אם מישהו עשה עליו arrest לאחרונה
        bool was_arrested_recently() const;
        void set_arrested_recently(bool val);
        void set_gather_blocked(bool val) { gather_blocked = val; }
        bool is_gather_blocked() const { return gather_blocked; }
        bool is_under_sanction() const { return is_sanctioned; }
        void set_sanctioned(bool val) { is_sanctioned = val; }
        bool was_couped() const { return couped_flag; }
        void set_couped(bool val) { couped_flag = val; }



        bool has_bonus_turn() const;
        void use_bonus_turn();
        void give_bonus_turns(int turns);
        int get_bonus_turns() const { return bonus_turns; }
        const std::string& get_last_action() const {
            return last_action;
        }
        virtual void on_turn_start();
        void clear_last_action();

    };

}
