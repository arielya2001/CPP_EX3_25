/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Spy.hpp"                  // Include the Spy class definition
#include "Game.hpp"                // Include the Game class for game state references
#include <stdexcept>               // For throwing runtime errors
#include <iostream>                // For debug output using std::cout

using namespace std;               // Allow use of standard namespace elements directly

namespace coup {                  // Define everything in the 'coup' namespace

    /// Constructor: creates a Spy with name and assigns role "Spy"
    Spy::Spy(Game& game, const std::string& name)
        : Player(game, name, "Spy") {}  // Initialize the Spy using the Player constructor with role "Spy"

    /// Reveals the number of coins the target player holds
    int Spy::spy_on(Player& target) const {
        if (game.num_players() < 2) {   // Ensure that the game has started with enough players
            throw runtime_error("Game has not started – need at least 2 players."); // Throw error if not
        }
        if (!is_active()) throw runtime_error("Spy is not active."); // Prevent action if Spy is out
        if (!target.is_active()) throw runtime_error("Target is not active."); // Prevent spying on eliminated player

        return target.coins(); // Return the coin count of the target player
    }

    /// Blocks an arrest attempt on the target player by remembering the turn
    void Spy::block_arrest(Player& target) {
        if (game.num_players() < 2) {   // Check if game is active
            throw runtime_error("Game has not started – need at least 2 players."); // Error if not
        }
        if (!is_active()) {             // Check if the Spy is still in the game
            throw runtime_error("Spy is not active."); // Error if not
        }
        if (!target.is_active()) {      // Target must also be active
            throw runtime_error("Target is not active."); // Error if not
        }

        // Save the turn number when the block was applied
        int blocked_at_turn = game.get_total_turns();       // Get current total turn count
        blocked_arrests[&target] = blocked_at_turn;         // Store that this target is now protected from arrest
    }

    /// Checks if the spy is still blocking arrest attempts on the given player
    bool Spy::is_arrest_blocked(Player* p) const {
        auto it = blocked_arrests.find(p);                  // Look for player p in the blocked list
        if (it == blocked_arrests.end()) return false;      // If not found, no block is active

        int blocked_at = it->second;                        // Get the turn number when the block was applied
        int turns_since_block = game.get_total_turns() - blocked_at; // Calculate how many turns have passed

        // Block lasts for fewer turns than total number of players
        return turns_since_block < game.num_players();      // Return true if still within valid blocking duration
    }

    /// Clears expired arrest blocks based on game progress
    void Spy::clear_expired_blocks() {
        if (game.num_players() < 2) {                       // Game must be active to perform cleanup
            throw runtime_error("Game has not started – need at least 2 players."); // Error if not
        }

        int current_turn = game.get_total_turns();          // Get the current number of turns in the game
        for (auto it = blocked_arrests.begin(); it != blocked_arrests.end(); ) { // Iterate through all blocks
            int blocked_at = it->second;                    // Turn when block was applied
            if (current_turn - blocked_at >= game.num_players()) { // If block has expired
                it = blocked_arrests.erase(it);             // Remove it from the map and advance iterator
            } else {
                ++it;                                       // Otherwise, just advance to the next
            }
        }
    }

}
