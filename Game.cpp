/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Game.hpp"            // Include the header file for the Game class
#include "Player.hpp"          // Include the Player class
#include <stdexcept>           // For standard runtime error exceptions
#include <iostream>            // For debug output using std::cout
using namespace std;           // Use the standard namespace

namespace coup
{
    // Begin namespace coup

    /// Constructor for the Game class. Initializes game state variables.
    Game::Game() :
        turn_index(0),                        // Start with first turn index
        started(false),                       // Game has not started yet
        awaiting_bribe_block(false),          // No bribe block pending
        bribing_player(nullptr),              // No bribing player initially
        awaiting_coup_block(false),           // No coup block pending
        coup_attacker(nullptr),               // No coup attacker
        coup_target(nullptr),                 // No coup target
        total_turns(0)                        // No turns taken yet
    {}

    /// Adds a player to the game. Throws an error if there are already 6 players.
    void Game::add_player(Player* player) {
        if (players_list.size() >= 6) {                               // Limit players to 6
            throw runtime_error("Cannot add more than 6 players.");  // Throw if exceeded
        }
        players_list.push_back(player);                              // Add player to the list
    }

    /// Returns the name of the current player whose turn it is.
    string Game::turn() const {
        if (players_list.empty()) {
            throw runtime_error("No players in the game.");                      // No players added yet
        }
        if (players_list.size() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");  // Need at least 2 players
        }

        size_t idx = turn_index % players_list.size();    // Normalize index
        size_t tries = 0;                                 // Counter to avoid infinite loop
        while (tries < players_list.size() && !players_list[idx]->is_active()) {
            idx = (idx + 1) % players_list.size();        // Skip inactive players
            tries++;
        }
        return players_list[idx]->name();                 // Return active player's name
    }

    /// Returns a vector with the names of all active players.
    vector<string> Game::players() const {
        vector<string> active_players;                        // Create list of names
        for (Player* p : players_list) {                      // Iterate all players
            if (p->is_active()) {                             // Only include active ones
                active_players.push_back(p->name());
            }
        }
        return active_players;                                // Return list
    }

    /// Returns the name of the last player alive. Throws if the game is still ongoing.
    string Game::winner() const {
        int alive = 0;                     // Count of active players
        string last;                      // Store last active player's name
        for (Player* p : players_list) {
            if (p->is_active()) {
                alive++;
                last = p->name();
            }
        }
        if (alive == 1) {                  // One player left – game over
            return last;
        }
        throw runtime_error("Game is still ongoing.");  // More than one active player
    }

    /// Advances the game to the next player's turn, handling bonus turns and block states.
    void Game::next_turn() {
        Player* current = players_list[turn_index];         // Get current player
        std::cout << "[Debug] next_turn: current=" << current->name()
                  << ", bonus_turns=" << current->has_bonus_turn() << std::endl;

        if (current->has_bonus_turn()) {                    // Check for bonus turn
            current->use_bonus_turn();                      // Use it
            total_turns++;                                 // Increment turn count
            std::cout << "[Debug] Bonus turn used. Total turns: " << total_turns << std::endl;
            current->on_turn_start();                       // Trigger start-of-turn effects
            return;
        }

        current->set_sanctioned(false);                     // Clear sanction flag

        total_turns++;                                     // Count a regular turn
        std::cout << "[Debug] Regular turn. Total turns: " << total_turns << std::endl;

        if (awaiting_bribe_block) {                         // Handle bribe block state
            awaiting_bribe_block = false;                   // Clear block flag
            if (bribing_player && bribing_player->is_active()) {
                turn_index = get_player_index(bribing_player);  // Return turn to briber
                bribing_player = nullptr;                       // Clear pointer
                players_list[turn_index]->on_turn_start();      // Start their turn
                return;
            }
        }

        if (awaiting_coup_block) {                          // Handle coup block state
            awaiting_coup_block = false;
            if (coup_attacker && coup_attacker->is_active()) {
                turn_index = get_player_index(coup_attacker);  // Return turn to attacker
                players_list[turn_index]->on_turn_start();     // Start their turn
                return;
            }
        }

        do {
            turn_index = (turn_index + 1) % players_list.size();  // Move to next player
        } while (!players_list[turn_index]->is_active());         // Skip inactive players

        std::cout << "[Debug] Next turn: " << players_list[turn_index]->name() << std::endl;
        players_list[turn_index]->on_turn_start();         // Start next player's turn
    }

    /// Returns a reference to the list of all players (active and inactive).
    const std::vector<Player*>& Game::get_all_players() const {
        return players_list;    // Return full player list
    }

    /// Returns the index of the given player in the list.
    int Game::get_player_index(Player* p) const {
        for (size_t i = 0; i < players_list.size(); ++i) {  // Search through list
            if (players_list[i] == p) return i;             // Match found
        }
        return -1;                                           // Not found
    }

    /// Returns the current turn index.
    int Game::get_turn_index() const {
        return turn_index;     // Return index of current turn
    }

    /// Returns the total number of players in the game.
    int Game::num_players() const {
        return static_cast<int>(players_list.size());   // Return player count
    }

    /// Sets the current turn to the specified player. Throws if the player is not in the game.
    void Game::set_turn_to(Player* player) {
        std::cout << "[DEBUG] Game::set_turn_to called for: " << player->name() << std::endl;
        for (size_t i = 0; i < players_list.size(); ++i) {  // Find the player
            if (players_list[i] == player) {
                turn_index = i;                             // Set turn to their index
                players_list[i]->on_turn_start();           // Start their turn
                return;
            }
        }
        throw std::runtime_error("Player not found in game.");  // Error if not found
    }

    /// Clears the state of a pending coup (e.g., after it's resolved or blocked).
    void Game::clear_coup_target() {
        if (awaiting_coup_block && coup_attacker) {                        // If a coup is pending and there is an attacker
            size_t attacker_index = get_player_index(coup_attacker);      // Get index of the attacker
            if (attacker_index == turn_index) {                            // If attacker was the current turn
                turn_index = (turn_index + 1) % players_list.size();       // Advance to the next player's turn
            }
        }

        coup_target = nullptr;            // Clear the coup target
        coup_attacker = nullptr;          // Clear the coup attacker
        awaiting_coup_block = false;      // Reset the blocking state
    }

    /// Initializes a queue of Governors who can block a tax initiated by the given player.
    void Game::init_tax_blockers(Player* initiator) {
        tax_blockers_queue.clear();                                      // Clear the current queue
        for (Player* p : players_list) {                                 // Iterate over all players
            if (p->is_active() && p->role() == "Governor" && p != initiator) { // Only other active Governors
                tax_blockers_queue.push_back(p);                         // Add to the queue
            }
        }
    }

    void Game::init_coup_blockers(Player* attacker) {
        coup_blockers_queue.clear();                                     // Clear the coup blockers queue
        std::cout << "[DEBUG] Initializing coup blockers queue for attacker: " << attacker->name() << "\n";
        for (Player* p : players_list) {                                 // Iterate over all players
            if (p->is_active() && p->role() == "General" && p->coins() >= 5 && p != attacker) {
                std::cout << "[DEBUG] Adding General " << p->name() << " to coup_blockers_queue.\n";
                coup_blockers_queue.push_back(p);                        // Add eligible Generals to the queue
            }
        }
        if (coup_blockers_queue.empty()) {
            std::cout << "[DEBUG] No Generals available to block coup.\n";
        }
    }

    Player* Game::pop_next_coup_blocker() {
        if (coup_blockers_queue.empty()) return nullptr;   // Return null if queue is empty
        Player* next = coup_blockers_queue.front();        // Get next in line
        coup_blockers_queue.pop_front();                   // Remove from queue
        return next;
    }

    /// Initializes a queue of Judges who can block a bribe initiated by the given player.
    void Game::init_bribe_blockers(Player* briber) {
        bribe_blockers_queue.clear();                      // Clear the current queue
        std::cout << "[DEBUG] Initializing bribe blockers:\n";
        for (Player* p : players_list) {                   // Iterate over all players
            if (p->is_active() && p->role() == "Judge" && p != briber) {
                std::cout << "- " << p->name() << " added to bribe queue.\n";
                bribe_blockers_queue.push_back(p);         // Add eligible Judges to the queue
            }
        }
    }

    /// Returns the next Governor in the tax blocking queue, or nullptr if the queue is empty.
    Player* Game::pop_next_tax_blocker() {
        if (tax_blockers_queue.empty()) return nullptr;    // If queue is empty, return null
        Player* next = tax_blockers_queue.front();         // Get next blocker
        tax_blockers_queue.pop_front();                    // Remove from queue
        return next;
    }

    /// Returns the next Judge in the bribe blocking queue, or nullptr if the queue is empty.
    Player* Game::pop_next_bribe_blocker() {
        if (bribe_blockers_queue.empty()) return nullptr;  // If queue is empty, return null
        Player* next = bribe_blockers_queue.front();       // Get next Judge
        bribe_blockers_queue.pop_front();                  // Remove from queue
        return next;
    }

    /// Advances the bribe blocking queue to the next Judge or returns the turn to the briber.
    void Game::advance_bribe_block_queue() {
        Player* nextJudge = pop_next_bribe_blocker();      // Get next Judge

        if (nextJudge) {
            set_turn_to(nextJudge);                        // Give turn to Judge if exists
        } else {
            set_awaiting_bribe_block(false);               // No more Judges, reset state
            Player* briber = get_bribing_player();         // Get the briber player
            set_bribing_player(nullptr);                   // Clear the briber
            if (briber && briber->is_active()) {
                set_turn_to(briber);                       // Return turn to briber
            }
        }
    }

    void Game::skip_coup_block() {
        if (!awaiting_coup_block || !coup_attacker || !coup_target) {
            throw runtime_error("No coup block is pending."); // Sanity check
        }

        Player* next = pop_next_coup_blocker();                // Get next General
        if (next) {
            std::cout << "[DEBUG] General skipped coup block, passing to next General: " << next->name() << "\n";
            set_turn_to(next);                                 // Pass turn to next blocker
        } else {
            std::cout << "[DEBUG] No more Generals to block, resolving coup.\n";
            coup_target->set_couped(true);                     // Mark target as coup'ed
            coup_target->deactivate();                         // Deactivate the target
            std::cout << coup_attacker->name() << " performed coup on " << coup_target->name() << std::endl;

            int attackerIndex = get_player_index(coup_attacker);   // Store index before clearing
            set_awaiting_coup_block(false);                    // Reset coup block state
            set_coup_attacker(nullptr);                        // Clear attacker
            set_coup_target(nullptr);                          // Clear target

            const auto& players = get_all_players();           // Get player list
            do {
                attackerIndex = (attackerIndex + 1) % players.size();  // Advance to next player
            } while (!players[attackerIndex]->is_active());    // Skip inactive players

            std::cout << "[DEBUG] Coup resolved, advancing turn to: " << players[attackerIndex]->name() << "\n";
            set_turn_to(players[attackerIndex]);               // Set next player's turn
        }
    }

    /// Allows a General to block a pending coup if valid. Returns turn to next player.
    void Game::block_coup(Player* general) {
        if (!awaiting_coup_block || !coup_attacker || !coup_target) {
            throw runtime_error("No coup block is pending.");  // Ensure coup block is active
        }
        if (!general->is_active()) {
            throw runtime_error("Only an active General can block a coup."); // Must be active
        }

        if (general->role() != "General" || general->coins() < 5) {
            throw runtime_error("Player cannot block coup.");  // Must be a General with ≥5 coins
        }

        general->deduct_coins(5);                              // Pay the block cost
        std::cout << general->name() << " blocked the coup from " << coup_attacker->name() << "\n";

        int attackerIndex = get_player_index(coup_attacker);   // Save attacker index
        if (attackerIndex == -1) {
            throw runtime_error("Attacker not found in player list.");
        }

        set_awaiting_coup_block(false);                        // Reset state
        set_coup_attacker(nullptr);                            // Clear attacker
        set_coup_target(nullptr);                              // Clear target

        const auto& players = get_all_players();               // Get list of players
        int nextIndex = attackerIndex;
        do {
            nextIndex = (nextIndex + 1) % players.size();      // Move to next
        } while (!players[nextIndex]->is_active() && nextIndex != attackerIndex); // Avoid infinite loop

        if (nextIndex == attackerIndex) {
            throw runtime_error("No active players available after attacker."); // No one left
        }

        std::cout << "[DEBUG] Coup blocked, advancing turn to: " << players[nextIndex]->name() << "\n";
        set_turn_to(players[nextIndex]);                       // Set next player's turn
    }

    Player* Game::get_current_player() const {
        if (players_list.empty()) throw runtime_error("No players in the game."); // Sanity check
        return players_list[turn_index];                                          // Return current player
    }
}
