/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <iostream>
using namespace std;

namespace coup {

    /// Constructor for the Game class. Initializes game state variables.
    Game::Game() : turn_index(0), started(false), awaiting_bribe_block(false), bribing_player(nullptr), awaiting_coup_block(false), coup_attacker(nullptr), coup_target(nullptr), total_turns(0) {}

    /// Adds a player to the game. Throws an error if there are already 6 players.
    void Game::add_player(Player* player) {
        if (players_list.size() >= 6) {
            throw runtime_error("Cannot add more than 6 players.");
        }
        players_list.push_back(player);
    }

    /// Returns the name of the current player whose turn it is.
    string Game::turn() const {
        if (players_list.empty()) {
            throw runtime_error("No players in the game.");
        }
        if (players_list.size() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }

        size_t idx = turn_index % players_list.size();
        size_t tries = 0;
        while (tries < players_list.size() && !players_list[idx]->is_active()) {
            idx = (idx + 1) % players_list.size();
            tries++;
        }
        return players_list[idx]->name();
    }

    /// Returns a vector with the names of all active players.
    vector<string> Game::players() const {
        vector<string> active_players;
        for (Player* p : players_list) {
            if (p->is_active()) {
                active_players.push_back(p->name());
            }
        }
        return active_players;
    }

    /// Returns the name of the last player alive. Throws if the game is still ongoing.
    string Game::winner() const {
        int alive = 0;
        string last;
        for (Player* p : players_list) {
            if (p->is_active()) {
                alive++;
                last = p->name();
            }
        }
        if (alive == 1) {
            return last;
        }
        throw runtime_error("Game is still ongoing.");
    }

    /// Advances the game to the next player's turn, handling bonus turns and block states.
    void Game::next_turn() {
        Player* current = players_list[turn_index];
        std::cout << "[Debug] next_turn: current=" << current->name()
              << ", bonus_turns=" << current->has_bonus_turn() << std::endl;

        if (current->has_bonus_turn()) {
            current->use_bonus_turn();
            total_turns++;
            std::cout << "[Debug] Bonus turn used. Total turns: " << total_turns << std::endl;
            current->on_turn_start();
            return;
        }

        current->set_sanctioned(false);

        total_turns++;
        std::cout << "[Debug] Regular turn. Total turns: " << total_turns << std::endl;

        if (awaiting_bribe_block) {
            awaiting_bribe_block = false;
            if (bribing_player && bribing_player->is_active()) {
                turn_index = get_player_index(bribing_player);
                bribing_player = nullptr;
                players_list[turn_index]->on_turn_start();
                return;
            }
        }

        if (awaiting_coup_block) {
            awaiting_coup_block = false;
            if (coup_attacker && coup_attacker->is_active()) {
                turn_index = get_player_index(coup_attacker);
                players_list[turn_index]->on_turn_start();
                return;
            }
        }

        do {
            turn_index = (turn_index + 1) % players_list.size();
        } while (!players_list[turn_index]->is_active());

        std::cout << "[Debug] Next turn: " << players_list[turn_index]->name() << std::endl;
        players_list[turn_index]->on_turn_start();
    }

    /// Returns a reference to the list of all players (active and inactive).
    const std::vector<Player*>& Game::get_all_players() const {
        return players_list;
    }

    /// Returns the index of the given player in the list.
    int Game::get_player_index(Player* p) const {
        for (size_t i = 0; i < players_list.size(); ++i) {
            if (players_list[i] == p) return i;
        }
        return -1;
    }

    /// Returns the current turn index.
    int Game::get_turn_index() const {
        return turn_index;
    }

    /// Returns the total number of players in the game.
    int Game::num_players() const {
        return static_cast<int>(players_list.size());
    }

    /// Sets the current turn to the specified player. Throws if the player is not in the game.
    void Game::set_turn_to(Player* player) {
        std::cout << "[DEBUG] Game::set_turn_to called for: " << player->name() << std::endl;
        for (size_t i = 0; i < players_list.size(); ++i) {
            if (players_list[i] == player) {
                turn_index = i;
                players_list[i]->on_turn_start();
                return;
            }
        }
        throw std::runtime_error("Player not found in game.");
    }

    /// Clears the state of a pending coup (e.g., after it's resolved or blocked).
    void Game::clear_coup_target() {
        if (awaiting_coup_block && coup_attacker) {
            size_t attacker_index = get_player_index(coup_attacker);
            if (attacker_index == turn_index) {
                turn_index = (turn_index + 1) % players_list.size();
            }
        }

        coup_target = nullptr;
        coup_attacker = nullptr;
        awaiting_coup_block = false;
    }

    /// Initializes a queue of Governors who can block a tax initiated by the given player.
    void Game::init_tax_blockers(Player* initiator) {
        tax_blockers_queue.clear();
        for (Player* p : players_list) {
            if (p->is_active() && p->role() == "Governor" && p != initiator) {
                tax_blockers_queue.push_back(p);
            }
        }
    }

    /// Initializes a queue of Judges who can block a bribe initiated by the given player.
    void Game::init_bribe_blockers(Player* briber) {
        bribe_blockers_queue.clear();
        std::cout << "[DEBUG] Initializing bribe blockers:\n";
        for (Player* p : players_list) {
            if (p->is_active() && p->role() == "Judge" && p != briber) {
                std::cout << "- " << p->name() << " added to bribe queue.\n";
                bribe_blockers_queue.push_back(p);
            }
        }
    }

    /// Returns the next Governor in the tax blocking queue, or nullptr if the queue is empty.
    Player* Game::pop_next_tax_blocker() {
        if (tax_blockers_queue.empty()) return nullptr;
        Player* next = tax_blockers_queue.front();
        tax_blockers_queue.pop_front();
        return next;
    }

    /// Returns the next Judge in the bribe blocking queue, or nullptr if the queue is empty.
    Player* Game::pop_next_bribe_blocker() {
        if (bribe_blockers_queue.empty()) return nullptr;
        Player* next = bribe_blockers_queue.front();
        bribe_blockers_queue.pop_front();
        return next;
    }

    /// Advances the bribe blocking queue to the next Judge or returns the turn to the briber.
    void Game::advance_bribe_block_queue() {
        Player* nextJudge = pop_next_bribe_blocker();

        if (nextJudge) {
            set_turn_to(nextJudge);
        } else {
            set_awaiting_bribe_block(false);
            Player* briber = get_bribing_player();
            set_bribing_player(nullptr);
            if (briber && briber->is_active()) {
                set_turn_to(briber);
            }
        }
    }

    /// Called if no General blocks a coup. Finalizes the coup and eliminates the target.
    void Game::skip_coup_block() {
        if (!awaiting_coup_block || !coup_attacker || !coup_target) {
            throw runtime_error("No coup block is pending.");
        }

        std::cout << "General skipped coup block.\n";

        coup_target->set_couped(true);
        coup_target->deactivate();
        std::cout << coup_attacker->name() << " performed coup on " << coup_target->name() << std::endl;

        set_awaiting_coup_block(false);
        set_coup_attacker(nullptr);
        set_coup_target(nullptr);

        int attackerIndex = get_player_index(coup_attacker);
        do {
            attackerIndex = (attackerIndex + 1) % num_players();
        } while (!get_all_players()[attackerIndex]->is_active());

        set_turn_to(get_all_players()[attackerIndex]);
    }

    /// Allows a General to block a pending coup if valid. Returns turn to next player.
    void Game::block_coup(Player* general) {
        if (!awaiting_coup_block || !coup_attacker || !coup_target) {
            throw std::runtime_error("No coup to block.");
        }
        if (!general || general->role() != "General" || !general->is_active()) {
            throw std::runtime_error("Only an active General can block a coup.");
        }

        std::cout << general->name() << " blocked the coup from " << coup_attacker->name() << std::endl;

        set_awaiting_coup_block(false);
        set_coup_attacker(nullptr);
        set_coup_target(nullptr);

        int attackerIndex = get_player_index(coup_attacker);
        const auto& players = get_all_players();

        size_t nextIndex = (attackerIndex + 1) % players.size();
        while (!players[nextIndex]->is_active()) {
            nextIndex = (nextIndex + 1) % players.size();
        }

        set_turn_to(players[nextIndex]);
    }

} // namespace coup
