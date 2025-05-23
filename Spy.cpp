#include "Spy.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    Spy::Spy(Game& game, const std::string& name)
        : Player(game, name, "Spy") {}

    int Spy::spy_on(Player& target) const {
        if (game.num_players() < 2) {
            throw runtime_error("Game has not started – need at least 2 players.");
        }
        if (!is_active()) throw runtime_error("Spy is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");

        return target.coins();
    }

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

        // שמירת מספר הסיבוב בעת החסימה
        int blocked_at_turn = game.get_total_turns();
        blocked_arrests[&target] = blocked_at_turn;
    }

    bool Spy::is_arrest_blocked(Player* p) const {
        auto it = blocked_arrests.find(p);
        if (it == blocked_arrests.end()) return false;

        int blocked_at = it->second;
        int turns_since_block = game.get_total_turns() - blocked_at;

        // חסימה נמשכת עד שחלף סיבוב מלא של כל השחקנים
        return turns_since_block < game.num_players();
    }

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
