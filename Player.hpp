/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include <string>
#include "Game.hpp"

namespace coup {

    class Player {
    protected:
        Game& game;                     ///< Reference to the game instance
        std::string player_name;       ///< Player's name
        std::string role_name;         ///< Role type (e.g., "Spy", "Governor")
        int coin_count;                ///< Number of coins the player holds
        bool active;                   ///< Is the player still in the game
        bool was_arrested_last;        ///< Was this player arrested in last round
        bool gather_blocked;           ///< Can this player gather coins
        bool sanctioned = false;       ///< Is the player sanctioned
        std::string last_action;       ///< Last action taken by this player
        bool couped_flag = false;      ///< Was the player couped
        int bonus_turns;               ///< Number of bonus turns
        Player* last_arrested_target;  ///< Target of last arrest

    public:
        /// Constructs a player with name and role
        Player(Game& game, const std::string& name, const std::string& role);
        virtual ~Player() = default;

        /// Basic actions
        virtual void gather();
        virtual void tax();
        virtual void bribe();
        virtual void arrest(Player& target);
        virtual void sanction(Player& target);
        virtual void coup(Player& target);

        /// Accessors
        std::string name() const;
        std::string role() const;
        int coins() const;
        bool is_active() const;

        /// Coin and status handling
        void add_coins(int amount);
        void deduct_coins(int amount);
        void deactivate();           ///< Deactivates the player (e.g., due to coup)
        void set_active(bool val);   ///< Reactivates the player if needed

        /// Arrest history flag
        bool was_arrested_recently() const;
        void set_arrested_recently(bool val);

        /// Sanction and gather-block flags
        void set_gather_blocked(bool val) { gather_blocked = val; }
        bool is_gather_blocked() const { return gather_blocked; }

        void set_sanctioned(bool val) { sanctioned = val; }
        bool is_sanctioned() const { return sanctioned; }

        /// Coup status
        bool was_couped() const { return couped_flag; }
        void set_couped(bool val) { couped_flag = val; }

        /// Bonus turns management
        bool has_bonus_turn() const;
        void use_bonus_turn();
        void give_bonus_turns(int turns);
        int get_bonus_turns() const { return bonus_turns; }

        /// Action tracking
        const std::string& get_last_action() const { return last_action; }
        void clear_last_action();

        /// Called automatically when the player's turn starts
        virtual void on_turn_start();

        /// Returns reference to the game object
        const Game& getGame() const {
            return game;
        }

        /// Manually sets role name
        void set_role_name(const std::string& role);
    };

}
