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
        bool was_arrested_last;
        bool gather_blocked;
        bool sanctioned= false;  // ✅ בוליאני פשוט ויעיל
        std::string last_action;
        bool couped_flag = false;
        int bonus_turns;
        Player* last_arrested_target;

    public:
        Player(Game& game, const std::string& name, const std::string& role);
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

        // arrest
        bool was_arrested_recently() const;
        void set_arrested_recently(bool val);

        // סנקציות ואיסורים
        void set_gather_blocked(bool val) { gather_blocked = val; }
        bool is_gather_blocked() const { return gather_blocked; }

        void set_sanctioned(bool val) { sanctioned = val; }
        bool is_sanctioned() const { return sanctioned; }

        // coup
        bool was_couped() const { return couped_flag; }
        void set_couped(bool val) { couped_flag = val; }

        // bonus turns
        bool has_bonus_turn() const;
        void use_bonus_turn();
        void give_bonus_turns(int turns);
        int get_bonus_turns() const { return bonus_turns; }

        const std::string& get_last_action() const { return last_action; }
        void clear_last_action();
        virtual void on_turn_start();  // קריאה בתחילת תור

        const Game& getGame() const {
            return game;
        }
        void set_role_name(const std::string& role);

    };

}
