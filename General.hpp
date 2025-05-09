#pragma once

#include "Player.hpp"
#include <unordered_set>

namespace coup {

    class General : public Player {
    private:

    public:
        General(Game& game, const std::string& name);
        std::unordered_set<Player*> protected_players;

        // פעולה יוזמת: הגנה על שחקן מ־coup
        void protect_from_coup(Player& target);

        // בדיקה אם שחקן מוגן
        bool is_protecting(Player* target) const;

        // תופעל אוטומטית כשנעשה עליו arrest
        void refund_arrest_coin();
    };

}
