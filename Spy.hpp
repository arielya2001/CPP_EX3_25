#pragma once

#include "Player.hpp"
#include <set>
#include <unordered_map>

namespace coup {

    class Spy : public Player {
    private:
        std::unordered_map<Player*, int> blocked_arrests;

    public:
        /// Constructs a Spy player with a given name
        Spy(Game& game, const std::string& name);

        /// Reveals how many coins a target player has
        int spy_on(Player& target) const;

        /// Blocks arrest attempts against the target player
        void block_arrest(Player& target);

        /// Checks if arrest attempts against the player are currently blocked
        bool is_arrest_blocked(Player* p) const;

        /// Removes expired arrest blocks based on turn count
        void clear_expired_blocks();
    };

}
