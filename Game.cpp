#include "Game.hpp"
#include "Player.hpp"
#include "Spy.hpp"
#include <stdexcept>
#include <iostream>
using namespace std;

namespace coup {

    Game::Game() : turn_index(0), started(false), awaiting_bribe_block(false), bribing_player(nullptr), awaiting_coup_block(false), coup_attacker(nullptr), coup_target(nullptr), total_turns(0) {}

    void Game::add_player(Player* player) {
        if (players_list.size() >= 6) {
            throw runtime_error("Cannot add more than 6 players.");
        }
        players_list.push_back(player);
    }

    string Game::turn() const {
        if (players_list.empty()) {
            throw runtime_error("No players in the game.");
        }

        size_t idx = turn_index % players_list.size();
        size_t tries = 0;
        while (tries < players_list.size() && !players_list[idx]->is_active()) {
            idx = (idx + 1) % players_list.size();
            tries++;
        }
        return players_list[idx]->name();
    }

    vector<string> Game::players() const {
        vector<string> active_players;
        for (Player* p : players_list) {
            if (p->is_active()) {
                active_players.push_back(p->name());
            }
        }
        return active_players;
    }

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

    void Game::next_turn() {
        Player* current = players_list[turn_index];

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


    const std::vector<Player*>& Game::get_all_players() const {
        return players_list;
    }

    int Game::get_player_index(Player* p) const {
        for (size_t i = 0; i < players_list.size(); ++i) {
            if (players_list[i] == p) return i;
        }
        return -1;
    }

    int Game::get_turn_index() const {
        return turn_index;
    }

    int Game::num_players() const {
        return static_cast<int>(players_list.size());
    }

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

    void Game::init_tax_blockers(Player* initiator) {
        tax_blockers_queue.clear();
        for (Player* p : players_list) {
            if (p->is_active() && p->role() == "Governor" && p != initiator) {
                tax_blockers_queue.push_back(p);
            }
        }
    }
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



    Player* Game::pop_next_tax_blocker() {
        if (tax_blockers_queue.empty()) return nullptr;
        Player* next = tax_blockers_queue.front();
        tax_blockers_queue.pop_front();
        return next;
    }
    Player* Game::pop_next_bribe_blocker() {
        if (bribe_blockers_queue.empty()) return nullptr;
        Player* next = bribe_blockers_queue.front();
        bribe_blockers_queue.pop_front();
        return next;
    }

    void Game::advance_bribe_block_queue() {
        // מוציא את השופט הבא מהתור (אם יש)
        Player* nextJudge = pop_next_bribe_blocker();

        if (nextJudge) {
            // אם יש עוד שופט - מעביר אליו את התור
            set_turn_to(nextJudge);
        } else {
            // אם אין יותר שופטים - מבטל את מצב השוחד
            set_awaiting_bribe_block(false);

            // שומר את המשחד
            Player* briber = get_bribing_player();
            set_bribing_player(nullptr);

            // מחזיר את התור למשחד אם הוא עדיין חי
            if (briber && briber->is_active()) {
                set_turn_to(briber);
            }
        }
    }





} // namespace coup
