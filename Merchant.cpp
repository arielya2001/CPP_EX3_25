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

}
