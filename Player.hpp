/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include <string>
#include "Game.hpp"

namespace coup {

    class Player {
    protected:
        Game& game;                     ///< Reference to the game instance this player belongs to
        std::string player_name;       ///< The name of the player
        std::string role_name;         ///< The role of the player (e.g., "Spy", "Governor")
        int coin_count;                ///< Current number of coins the player has
        bool active;                   ///< Whether the player is still in the game
        bool was_arrested_last;        ///< Flag indicating if the player was arrested last round
        bool gather_blocked;           ///< Flag indicating if the player is blocked from gathering
        bool sanctioned = false;       ///< Flag indicating if the player is currently sanctioned
        std::string last_action;       ///< Stores the last action the player took (e.g., "tax", "bribe")
        bool couped_flag = false;      ///< Flag indicating if the player was eliminated via coup
        int bonus_turns;               ///< Number of extra turns the player has (usually from bribe)
        Player* last_arrested_target;  ///< Pointer to the last player this player tried to arrest

    public:
        /// Constructs a new player with the given game, name, and role
        Player(Game& game, const std::string& name, const std::string& role);

        /// Default virtual destructor
        virtual ~Player() = default;

        Player(const Player&) = delete;            ///< Copy constructor deleted (no copying allowed)
        Player& operator=(const Player&) = delete; ///< Copy assignment operator deleted (no assignment)

        // === Basic actions ===

        /// Gathers 1 coin (if not blocked or sanctioned)
        virtual void gather();

        /// Takes 2 coins via tax (may be blocked by Governor)
        virtual void tax();

        /// Pays 4 coins to receive bonus turn (may be blocked by Judge)
        virtual void bribe();

        /// Arrests a target player (possible counteractions apply)
        virtual void arrest(Player& target);

        /// Applies sanction to another player (prevents gathering and tax)
        virtual void sanction(Player& target);

        /// Eliminates another player by paying 7 coins (may be blocked by Generals)
        virtual void coup(Player& target);

        // === Accessors ===

        /// Returns the player's name
        std::string name() const;

        /// Returns the player's role
        std::string role() const;

        /// Returns the number of coins the player currently holds
        int coins() const;

        /// Returns whether the player is still in the game
        bool is_active() const;

        // === Coin and status handling ===

        /// Adds coins to the player
        void add_coins(int amount);

        /// Deducts coins from the player (throws if not enough)
        void deduct_coins(int amount);

        /// Marks player as inactive (e.g., after being couped)
        void deactivate();

        /// Sets the active state of the player (used rarely)
        void set_active(bool val);

        // === Arrest history flag ===

        /// Returns whether the player was arrested last turn
        bool was_arrested_recently() const;

        /// Sets whether the player was arrested last turn
        void set_arrested_recently(bool val);

        // === Sanction and gather-block flags ===

        /// Blocks or unblocks the player's ability to gather
        void set_gather_blocked(bool val) { gather_blocked = val; }

        /// Returns true if the player's gathering ability is currently blocked
        bool is_gather_blocked() const { return gather_blocked; }

        /// Sets the player's sanctioned status
        void set_sanctioned(bool val) { sanctioned = val; }

        /// Returns whether the player is currently sanctioned
        bool is_sanctioned() const { return sanctioned; }

        // === Coup status ===

        /// Returns whether the player was couped
        bool was_couped() const { return couped_flag; }

        /// Sets the coup flag for the player
        void set_couped(bool val) { couped_flag = val; }

        // === Bonus turns management ===

        /// Returns true if the player has any bonus turns
        bool has_bonus_turn() const;

        /// Consumes one bonus turn if available
        void use_bonus_turn();

        /// Grants the player a number of bonus turns
        void give_bonus_turns(int turns);

        /// Returns number of bonus turns the player currently has
        int get_bonus_turns() const { return bonus_turns; }

        // === Action tracking ===

        /// Returns the last action performed by the player
        const std::string& get_last_action() const { return last_action; }

        /// Clears the last action record
        void clear_last_action();

        // === Turn hooks ===

        /// Called automatically at the start of the player's turn
        virtual void on_turn_start();

        // === Game association ===

        /// Returns a const reference to the game object (useful for rules or interaction)
        const Game& getGame() const {
            return game;
        }

        // === Role management ===

        /// Changes the player's role (used in dynamic role assignments or tests)
        void set_role_name(const std::string& role);
    };

}
