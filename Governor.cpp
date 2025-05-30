/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Governor.hpp"                     // Include header for the Governor class
#include <stdexcept>                        // For runtime_error exception
#include <iostream>                         // For debugging output

using namespace std;

namespace coup {

Governor::Governor(Game& game, const std::string& name)
    : Player(game, name, "Governor") {}     // Constructor: initialize as a "Governor" role

/// Performs the 'tax' action: adds 3 coins and may trigger a tax block phase.
void Governor::tax() {
    if (game.num_players() < 2) {                                               // Check if game has started
        throw runtime_error("Game has not started – need at least 2 players.");
    }
    if (!is_active()) throw runtime_error("Inactive player cannot act.");       // Can't act if not active
    if (game.turn() != name()) throw runtime_error("Not your turn.");           // Only act on your turn
    if (coins() >= 10) throw runtime_error("Must perform coup with 10 coins."); // Coup required with 10+
    if (is_sanctioned()) throw runtime_error("You are under sanction and cannot tax."); // Check sanction

    add_coins(3);                                                                // Add 3 coins for tax
    last_action = "tax";                                                        // Save last action

    for (Player* p : game.get_all_players()) {                                  // Check for other Governors
        if (p->is_active() && p->role() == "Governor" && p != this) {
            game.set_last_tax_index(game.get_player_index(this));              // Save index of current Governor
            game.set_awaiting_tax_block(true);                                 // Set tax block state
            game.set_tax_target(this);                                         // Set tax target
            game.set_tax_source(p);                                            // Set blocker source
            game.set_turn_to(p);                                               // Give turn to the blocker
            return;
        }
    }

    game.next_turn();                                                          // If no one can block, move on
}

/// Blocks a tax action, refunds coins from the target, and resumes turn order.
void Governor::block_tax(Player& target) {
    if (game.num_players() < 2) {                                               // Game must have started
        throw runtime_error("Game has not started – need at least 2 players.");
    }
    if (!is_active() || !target.is_active()) return;                            // Both players must be active
    if (target.get_last_action() != "tax") throw runtime_error("Target didn't tax."); // Only block tax

    int refund = (dynamic_cast<Governor*>(&target)) ? 3 : 2;                    // Refund 3 for Governor, else 2
    if (target.coins() < refund) throw runtime_error("Not enough coins to undo tax."); // Must have enough coins

    target.deduct_coins(refund);                                                // Remove coins
    target.clear_last_action();                                                 // Clear the "tax" mark

    game.set_awaiting_tax_block(false);                                         // Reset block state
    game.set_tax_target(nullptr);
    game.set_tax_source(nullptr);

    int index = game.get_last_tax_index();                                      // Start from original Governor
    const auto& players = game.get_all_players();                               // Get all players
    do {
        index = (index + 1) % players.size();                                   // Find next active player
    } while (!players[index]->is_active());

    game.set_turn_to(players[index]);                                           // Set next turn
    game.set_turn_index(index);                                                // Update index accordingly
}

/// Skips current Governor's tax block and moves to next or resumes game.
void Governor::skip_tax_block() {
    if (!game.is_awaiting_tax_block()) {
        throw runtime_error("Game has not started – need at least 2 players."); // Defensive check
    }

    std::cout << "[DEBUG] Governor " << name() << " is skipping tax block.\n";  // Debug message
    Player* next = game.pop_next_tax_blocker();                                 // Get next Governor
    if (next) {
        std::cout << "[DEBUG] Passing tax block to next Governor: " << next->name() << "\n";
        game.set_tax_source(next);                                              // Set new source
        game.set_turn_to(next);                                                 // Pass turn
    } else {
        std::cout << "[DEBUG] No more Governors to block tax, resolving.\n";
        Player* tax_target = game.get_tax_target();                             // Get the original taxer
        game.set_awaiting_tax_block(false);                                     // Reset tax block state
        game.set_tax_target(nullptr);
        game.set_tax_source(nullptr);

        // Advance to the next active player after the tax_target
        if (tax_target && tax_target->is_active()) {
            int target_index = game.get_player_index(tax_target);              // Start after target
            const auto& players = game.get_all_players();
            int next_index = target_index;
            do {
                next_index = (next_index + 1) % players.size();                // Skip inactive players
            } while (!players[next_index]->is_active());
            std::cout << "[DEBUG] Advancing turn to next player after tax target: " << players[next_index]->name() << "\n";
            game.set_turn_to(players[next_index]);                             // Set turn to next
        } else {
            game.next_turn();                                                  // If target is gone, skip turn
        }
    }
}

}
