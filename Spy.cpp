#include "Spy.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    /// Constructor: creates a Spy with name and assigns role "Spy"
    Spy::Spy(Game& game, const std::string& name)
        : Player(game, name, "Spy") {}

    /// Reveals the number of coins the target player holds
    int Spy::spy_on(Player& target) const {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) throw runtime_error("Spy is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");

        return target.coins();
    }

    /// Blocks an arrest attempt on the target player by remembering the turn
    void Spy::block_arrest(Player& target) {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) {
            throw runtime_error("Spy is not active.");
        }
        if (!target.is_active()) {
            throw runtime_error("Target is not active.");
        }

        // Save the turn number when the block was applied
        int blocked_at_turn = game.get_total_turns();
        blocked_arrests[&target] = blocked_at_turn;
    }

    /// Checks if the spy is still blocking arrest attempts on the given player
    bool Spy::is_arrest_blocked(Player* p) const {
        auto it = blocked_arrests.find(p);
        if (it == blocked_arrests.end()) return false;

        int blocked_at = it->second;
        int turns_since_block = game.get_total_turns() - blocked_at;

        // Block lasts for fewer turns than total number of players
        return turns_since_block < game.num_players();
    }

    /// Clears expired arrest blocks based on game progress
    void Spy::clear_expired_blocks() {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }

        int current_turn = game.get_total_turns();
        for (auto it = blocked_arrests.begin(); it != blocked_arrests.end(); ) {
            int blocked_at = it->second;
            if (current_turn - blocked_at >= game.num_players()) {
                it = blocked_arrests.erase(it);
            } else {
                ++it;
            }
        }
    }

}
