#include "Merchant.hpp"
#include <iostream>

namespace coup {

    Merchant::Merchant(Game& game, const std::string& name)
        : Player(game, name, "Merchant") {}

    void Merchant::on_turn_start() {
        Player::on_turn_start(); // חשוב לשמור על הבסיס
        if (coins() >= 3) {
            add_coins(1);
            std::cout << name() << " (Merchant) received 1 bonus coin for having 3+ coins.\n";
        }
    }

    void Merchant::on_bribed_by(Player& p) {
        add_coins(1);
        std::cout << name() << " received 1 coin from bribe by " << p.name() << std::endl;
    }

}
