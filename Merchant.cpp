#include "Merchant.hpp"
#include <stdexcept>

using namespace std;

namespace coup {

    Merchant::Merchant(Game& game, const string& name)
        : Player(game, name, "Merchant") {}

    void Merchant::on_bribed_by(Player& source) {
        if (!is_active() || !source.is_active()) return;

        add_coins(1);  // הרווח שלו מהשוחד
    }
    // בתוך Merchant.cpp
    void Merchant::on_turn_start() {
        if (!is_active()) return;
        if (coins() >= 3) {
            add_coins(1);
        }
    }

}
