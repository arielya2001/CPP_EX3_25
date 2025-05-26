/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once
#include <vector>
#include <string>
#include <deque>  // למעלה בקובץ

namespace coup {
    class Player;  // Forward declaration – Game uses Player before it's defined

    class Game {
    private:
        // List of all players in the game
        std::vector<Player*> players_list;

        // Index of the current player’s turn
        size_t turn_index = 0;

        // Total number of turns that have occurred
        int total_turns = 0;

        // Indicates whether the game has started (at least two players)
        bool started = false;

        // Flags and pointers related to bribe block logic
        bool awaiting_bribe_block = false;
        Player* bribing_player = nullptr;

        // Flags and pointers related to coup block logic
        Player* coup_attacker = nullptr;
        Player* coup_target = nullptr;
        bool awaiting_coup_block = false;

        // Flags and pointers related to tax block logic
        bool awaiting_tax_block = false;
        Player* tax_target = nullptr;
        Player* tax_source = nullptr;

        // Index of the last tax action (used for undoing or blocking)
        int last_tax_index = -1;

        // Queues for managing multiple blockers
        std::deque<Player*> tax_blockers_queue;
        std::deque<Player*> bribe_blockers_queue;

    public:
        /// Constructor that initializes the game state.
        Game();

        /// Adds a new player to the game. Throws if the maximum number is exceeded.
        void add_player(Player* player);

        /// Returns the name of the current player whose turn it is.
        std::string turn() const;

        /// Returns a list of names of active players.
        std::vector<std::string> players() const;

        /// Returns the name of the winner. Throws if the game is not yet over.
        std::string winner() const;

        /// Advances the game to the next player's turn.
        void next_turn();

        /// Returns a reference to the full list of players (active and inactive).
        const std::vector<Player*>& get_all_players() const;

        /// Returns the index of a player in the players list.
        int get_player_index(Player* p) const;

        /// Returns the index of the current turn.
        int get_turn_index() const;

        /// Returns the number of players in the game.
        int num_players() const;

        /// Directly sets the turn index (for manual control or testing).
        void set_turn_index(size_t index) {
            turn_index = index;
        }

        /// Sets whether the game is awaiting a bribe block.
        void set_awaiting_bribe_block(bool val) { awaiting_bribe_block = val; }

        /// Checks if a bribe block is pending.
        bool is_awaiting_bribe_block() const { return awaiting_bribe_block; }

        /// Sets the player who initiated the bribe.
        void set_bribing_player(Player* p) { bribing_player = p; }

        /// Gets the player who initiated the bribe.
        Player* get_bribing_player() const { return bribing_player; }

        /// Sets the turn to a specific player. Throws if the player is not found.
        void set_turn_to(Player* player);

        /// Gets the total number of turns taken.
        int get_total_turns() const { return total_turns; }

        /// Sets whether the game is awaiting a coup block.
        void set_awaiting_coup_block(bool val) { awaiting_coup_block = val; }

        /// Checks if a coup block is pending.
        bool is_awaiting_coup_block() const { return awaiting_coup_block; }

        /// Sets the player who attempted a coup.
        void set_coup_attacker(Player* p) { coup_attacker = p; }

        /// Returns the player who attempted a coup.
        Player* get_coup_attacker() const { return coup_attacker; }

        /// Sets the target of the coup.
        void set_coup_target(Player* p) { coup_target = p; }

        /// Returns the target of the coup.
        Player* get_coup_target() const { return coup_target; }

        /// Clears coup-related data (after block or execution).
        void clear_coup_target();

        /// Returns whether a tax block is pending.
        bool is_awaiting_tax_block() const { return awaiting_tax_block; }

        /// Sets whether a tax block is pending.
        void set_awaiting_tax_block(bool val) { awaiting_tax_block = val; }

        /// Gets the player who received coins from a tax.
        Player* get_tax_target() const { return tax_target; }

        /// Sets the tax target.
        void set_tax_target(Player* p) { tax_target = p; }

        /// Gets the player who performed the tax.
        Player* get_tax_source() const { return tax_source; }

        /// Sets the tax initiator.
        void set_tax_source(Player* p) { tax_source = p; }

        /// Sets the index of the last tax action.
        void set_last_tax_index(int idx) { last_tax_index = idx; }

        /// Gets the index of the last tax action.
        int get_last_tax_index() const { return last_tax_index; }

        /// Sets the tax index only if a tax block is not already pending.
        void set_last_tax_index_if_needed(int index) {
            if (!awaiting_tax_block) {
                last_tax_index = index;
            }
        }

        /// Initializes the queue of players who may block a tax action.
        void init_tax_blockers(Player* initiator);

        /// Initializes the queue of players who may block a bribe action.
        void init_bribe_blockers(Player* briber);

        /// Advances to the next judge in the bribe block queue or returns to the briber.
        void advance_bribe_block_queue();

        /// Called if no general blocks a coup. Executes the coup.
        void skip_coup_block();

        /// Called if a general blocks a coup. Cancels the coup and moves to next player.
        void block_coup(Player* general);

        /// Returns the next player eligible to block tax. Returns nullptr if none.
        Player* pop_next_tax_blocker();

        /// Returns the next player eligible to block bribe. Returns nullptr if none.
        Player* pop_next_bribe_blocker();
    };
}
