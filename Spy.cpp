#include "Spy.hpp"
#include <stdexcept>
#include <iostream>

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
        if (!is_active()) throw runtime_error("Spy is not active.");
        if (!target.is_active()) throw runtime_error("Target is not active.");

        blocked_arrests.insert(&target);
    }

    bool Spy::is_arrest_blocked(Player* p) const {
        return blocked_arrests.count(p) > 0;
    }

}
