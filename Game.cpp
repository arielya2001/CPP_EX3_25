#include "Game.hpp"
#include "Player.hpp"  // נצטרך את זה בהמשך כשנעבוד עם שמות וכו'

#include <stdexcept>
#include <iostream>
using namespace std;

namespace coup {

    Game::Game() : turn_index(0), started(false) {}

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

        // מדלגים על שחקנים לא פעילים
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
        do {
            turn_index = (turn_index + 1) % players_list.size();
        } while (!players_list[turn_index]->is_active());
    }
    const std::vector<Player*>& Game::get_all_players() const {
        return players_list;
    }


}
