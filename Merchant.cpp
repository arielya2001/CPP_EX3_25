#include "Merchant.hpp"
#include <iostream>

namespace coup {

    /// Constructor: initializes a Merchant player with a given name and role.
    Merchant::Merchant(Game& game, const std::string& name)
        : Player(game, name, "Merchant") {}

    /// Called automatically at the start of the player's turn.
    /// Grants 1 bonus coin if the player has 3 or more coins.
    void Merchant::on_turn_start() {
        Player::on_turn_start(); // חשוב לשמור על הבסיס
        if (coins() >= 3) {
            add_coins(1);
            std::cout << name() << " (Merchant) received 1 bonus coin for having 3+ coins.\n";
        }
    }

    /// Called when another player bribes the Merchant.
    /// Grants the Merchant 1 coin and logs the action.
    void Merchant::on_bribed_by(Player& p) {
        add_coins(1);
        std::cout << name() << " received 1 coin from bribe by " << p.name() << std::endl;
    }

}
