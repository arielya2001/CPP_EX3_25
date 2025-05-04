#pragma once

#include "Player.hpp"

namespace coup {

    class General : public Player {
    public:
        General(Game& game, const std::string& name);

        // פעולה לביטול הפיכה
        void block_coup(Player& target);

        // תופעל אוטומטית כשנעשה עליו arrest
        void refund_arrest_coin();
    };

}
