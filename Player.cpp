/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Player.hpp"                 // Include the Player class header
#include "Spy.hpp"                    // Include the Spy role class
#include "Baron.hpp"                  // Include the Baron role class
#include "General.hpp"                // Include the General role class
#include "Judge.hpp"                  // Include the Judge role class
#include "Merchant.hpp"              // Include the Merchant role class
#include "Game.hpp"                   // Include the Game class header
#include <stdexcept>                  // For throwing runtime exceptions
#include <iostream>                   // For input/output stream operations

using namespace std;                 // Use standard namespace for convenience

namespace coup {                     // Start of the coup namespace

    /// Constructs a new player with a given name and role
    Player::Player(Game& game, const string& name, const string& role)
        : game(game), player_name(name), role_name(role), coin_count(0), active(true),  // Initialize members
          was_arrested_last(false), gather_blocked(false), sanctioned(false), bonus_turns(0), last_arrested_target(nullptr)
    {
    }

    /// Basic gather action: earns 1 coin if allowed
    void Player::gather() {
        if (game.num_players() < 2) {                            // Ensure game has at least 2 players
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot play.");               // Check if player is active
        if (game.turn() != name()) throw runtime_error("Not your turn.");              // Check if it's player's turn
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins."); // Coup must be performed if coins ≥ 10
        if (sanctioned) throw runtime_error("You are sanctioned and cannot gather.");  // Sanctioned players can't gather

        last_arrested_target = nullptr;     // Reset last arrested target
        add_coins(1);                       // Add 1 coin to player
        last_action = "gather";             // Record last action
        std::cout << "[Debug] Player " << name() << " performed gather.\n"; // Debug log
        game.next_turn();                   // Advance turn
    }

    /// Performs a basic tax (2 coins), with possible block by Governor
    void Player::tax() {
        if (game.num_players() < 2) {                            // Ensure enough players
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot act.");               // Must be active
        if (game.turn() != name()) throw runtime_error("Not your turn.");              // Must be current player's turn
        if (coin_count >= 10) throw runtime_error("Must perform coup with 10 coins."); // Force coup if too rich
        if (sanctioned) throw runtime_error("You are under sanction and cannot tax."); // Sanctioned can't tax

        add_coins(2);                          // Earn 2 coins
        last_action = "tax";                   // Mark last action

        game.set_last_tax_index_if_needed(game.get_player_index(this)); // Update last-taxed player index if necessary
        game.init_tax_blockers(this);          // Initialize potential tax blockers (Governors)
        game.set_awaiting_tax_block(true);     // Set game to waiting for tax block
        game.set_tax_target(this);             // Set current player as tax target

        Player* nextGov = game.pop_next_tax_blocker(); // Get next potential tax blocker
        if (nextGov) {
            game.set_tax_source(nextGov);      // Record source of tax block
            game.set_turn_to(nextGov);         // Switch turn to the blocking Governor
        } else {
            game.set_awaiting_tax_block(false); // No blockers, reset state
            game.set_tax_target(nullptr);       // Clear tax target
            game.set_tax_source(nullptr);       // Clear source of block
            last_arrested_target = nullptr;     // Clear last arrest
            game.next_turn();                   // Proceed to next turn
        }
    }

    /// Performs a bribe action: pays 4 coins, may be blocked by Judges
    void Player::bribe() {
        if (game.num_players() < 2) {                              // Game must have at least 2 players
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!active) throw runtime_error("Inactive player cannot play.");               // Must be active
        if (game.turn() != name()) throw runtime_error("Not your turn.");              // Must be their turn
        if (coin_count < 4) throw runtime_error("Not enough coins for bribe.");        // Must have at least 4 coins

        deduct_coins(4);                      // Deduct 4 coins for bribe
        bonus_turns += 1;                     // Grant a bonus turn
        last_action = "bribe";                // Mark last action

        game.set_awaiting_bribe_block(true);  // Set state to awaiting block
        game.set_bribing_player(this);        // Register current briber
        game.init_bribe_blockers(this);       // Identify judges who can block

        Player* nextJudge = game.pop_next_bribe_blocker(); // Check if there’s a judge to block
        if (nextJudge) {
            game.set_turn_to(nextJudge);      // If yes, transfer turn to the judge
            return;                           // Wait for judge’s decision
        }
        last_arrested_target = nullptr;       // Reset arrested target if no block

        game.set_awaiting_bribe_block(false); // Reset game state
        game.set_bribing_player(nullptr);     // Clear bribing player
        game.set_turn_to(this);               // Return turn to current player for bonus
    }

/// Arrests another player, with logic for Spy, Merchant, General
void Player::arrest(Player& target) {
    if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players."); // Game not started
    if (!active) throw runtime_error("Inactive player cannot play."); // This player is out
    if (game.turn() != name()) throw runtime_error("Not your turn."); // Not the current turn
    if (!target.is_active()) throw runtime_error("Target player is not active."); // Target is already out

    std::cout << "[Debug] arrest: " << name()
              << " wants to arrest " << target.name()
              << " [last_arrested_target="
              << (last_arrested_target ? last_arrested_target->name() : "nullptr")
              << "], bonus_turns=" << bonus_turns << std::endl; // Debug info

    if (last_arrested_target == &target) {
        throw runtime_error("Cannot arrest the same player twice in a row."); // Prevents consecutive arrests of same player
    }

    for (Player* p : game.get_all_players()) { // Loop through all players
        Spy* spy = dynamic_cast<Spy*>(p); // Try casting to Spy
        if (spy && spy->is_active() && spy->is_arrest_blocked(this)) {
            throw runtime_error("Arrest blocked by Spy."); // Spy blocks the arrest
        }
    }

    if (Merchant* merchant = dynamic_cast<Merchant*>(&target)) {
        merchant->deduct_coins(2); // Merchant pays 2 coins
        std::cout << merchant->name() << " paid 2 coins due to arrest (Merchant penalty).\n";
    } else {
        target.deduct_coins(1); // Others pay 1 coin
        this->add_coins(1); // Arresting player gains 1 coin

        if (General* general = dynamic_cast<General*>(&target)) {
            general->refund_arrest_coin(); // General gets refund
            this->deduct_coins(1); // Arresting player loses 1 coin
            std::cout << general->name() << " refunded coin due to arrest (General ability).\n";
        }
    }

    last_arrested_target = &target; // Save for next arrest check
    last_action = "arrest"; // Set action

    std::cout << "[Debug] arrest complete: " << name()
              << " arrested " << target.name()
              << ", bonus_turns=" << bonus_turns << std::endl; // Debug info

    game.next_turn(); // Proceed to next turn
}

/// Performs a sanction against another player (3 coins)
void Player::sanction(Player& target) {
    if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players."); // Game not started
    if (!active) throw runtime_error("Inactive player cannot play."); // Player is inactive
    if (game.turn() != name()) throw runtime_error("Not your turn."); // Not the current turn
    if (!target.is_active()) throw runtime_error("Target player is not active."); // Target is already out

    int required_coins = 3; // Base cost
    if (dynamic_cast<Judge*>(&target)) {
        required_coins += 1; // Extra cost to sanction a Judge
    }

    if (coin_count < required_coins) {
        throw runtime_error("Not enough coins for sanction."); // Not enough coins
    }

    deduct_coins(3); // Deduct 3 coins
    target.set_sanctioned(true); // Mark target as sanctioned
    target.set_gather_blocked(true); // Prevent them from gathering

    if (Judge* judge = dynamic_cast<Judge*>(&target)) {
        judge->on_sanctioned(*this); // Judge penalizes attacker
    }
    if (Baron* baron = dynamic_cast<Baron*>(&target)) {
        baron->receive_sanction_penalty(); // Baron receives a coin
    }

    last_arrested_target = nullptr; // Clear arrest state
    last_action = "sanction"; // Set last action
    game.next_turn(); // Move to next turn
}

/// Performs a coup action, or initiates block opportunity
void Player::coup(Player& target) {
    if (game.num_players() < 2) throw runtime_error("Game has not started – need at least 2 players."); // Game not started
    if (!active) throw runtime_error("Inactive player cannot play."); // Inactive player
    if (game.turn() != name()) throw runtime_error("Not your turn."); // Not current turn
    if (!target.is_active()) throw runtime_error("Target already out."); // Already out
    if (coin_count < 7) throw runtime_error("Not enough coins to perform coup."); // Not enough coins

    deduct_coins(7); // Pay 7 coins
    last_action = "coup"; // Mark action

    // === Preparation for General block ===
    game.set_coup_attacker(this); // Set attacker
    game.set_coup_target(&target); // Set target
    game.init_coup_blockers(this); // Initialize blockers
    game.set_awaiting_coup_block(true); // Enter block phase
    std::cout << "[DEBUG] Coup initiated by " << name() << ", awaiting block: " << (game.is_awaiting_coup_block() ? "true" : "false") << "\n";

    Player* firstGen = game.pop_next_coup_blocker(); // Get first General
    if (firstGen) {
        std::cout << "[DEBUG] Passing turn to first General: " << firstGen->name() << "\n";
        game.set_turn_to(firstGen); // Let General block
        return;
    }

    game.set_awaiting_coup_block(false); // No block
    game.set_coup_attacker(nullptr); // Clear attacker
    game.set_coup_target(nullptr); // Clear target
    last_arrested_target = nullptr; // Clear arrest state
    target.set_couped(true); // Mark target as couped
    target.deactivate(); // Remove from game
    std::cout << name() << " performed coup on " << target.name() << std::endl;
    game.next_turn(); // Next turn
}


   /// Called at the beginning of each turn
void Player::on_turn_start() {
    was_arrested_last = false;    // Reset arrest flag (used to prevent consecutive arrests)
    gather_blocked = false;       // Allow gathering unless newly sanctioned
}

/// Getters and utility methods

/// Returns the name of the player
string Player::name() const { return player_name; }

/// Returns the role of the player
string Player::role() const { return role_name; }

/// Returns how many coins the player currently has
int Player::coins() const { return coin_count; }

/// Returns whether the player is currently active in the game
bool Player::is_active() const { return active; }

/// Adds the given amount of coins to the player
void Player::add_coins(int amount) { coin_count += amount; }

/// Deducts coins; throws error if player does not have enough
void Player::deduct_coins(int amount) {
    if (coin_count < amount) throw runtime_error("Not enough coins.");
    coin_count -= amount;
}

/// Sets the player as inactive (e.g., after being couped)
void Player::deactivate() { active = false; }

/// Returns whether the player was arrested in their last turn
bool Player::was_arrested_recently() const { return was_arrested_last; }

/// Sets the flag indicating if the player was recently arrested
void Player::set_arrested_recently(bool val) { was_arrested_last = val; }

/// Returns true if the player has bonus turns available
bool Player::has_bonus_turn() const { return bonus_turns > 0; }

/// Uses one bonus turn if available
void Player::use_bonus_turn() {
    if (bonus_turns > 0) {
        --bonus_turns;
    }
}

/// Adds the given number of bonus turns to the player
void Player::give_bonus_turns(int turns) { bonus_turns += turns; }

/// Sets the active status of the player
void Player::set_active(bool val) { active = val; }

/// Clears the record of the last action taken
void Player::clear_last_action() { last_action = ""; }

/// Updates the role name of the player (used in dynamic role changes)
void Player::set_role_name(const std::string& role) { this->role_name = role; }

}
