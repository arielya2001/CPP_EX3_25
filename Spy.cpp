#include "Spy.hpp"
#include "Game.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    Spy::Spy(Game& game, const std::string& name)
        : Player(game, name, "Spy") {}

    int Spy::spy_on(Player& target) const {
        if (!is_active()) throw runtime_error("Spy is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");
        return target.coins();
    }

    void Spy::block_arrest(Player& target) {
        if (!is_active() || !target.is_active()) return;

        // חסום את השחקן עד שיסיים את התור הבא שלו
        int target_index = game.get_player_index(&target);
        int unblock_index = (target_index + 1) % game.num_players();
        blocked_arrests[&target] = unblock_index;
    }

    bool Spy::is_arrest_blocked(Player* p) const {
        auto it = blocked_arrests.find(p);
        if (it == blocked_arrests.end()) return false;

        int unblock_index = it->second;
        return game.get_turn_index() != unblock_index; // נחסם עד *אחרי* ה־index
    }

    void Spy::clear_expired_blocks() {
        for (auto it = blocked_arrests.begin(); it != blocked_arrests.end(); ) {
            if (game.get_turn_index() == it->second) {
                it = blocked_arrests.erase(it);  // שחקן הגיע לתור שאחריו הוא חופשי
            } else {
                ++it;
            }
        }
    }

}
