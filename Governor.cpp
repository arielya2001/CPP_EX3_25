/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "Governor.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

Governor::Governor(Game& game, const std::string& name)
    : Player(game, name, "Governor") {}

/// Performs the 'tax' action: adds 3 coins and may trigger a tax block phase.
void Governor::tax() {
    if (game.num_players() < 2) {
        throw runtime_error("Game has not started – need at least 2 players.");
    }
    if (!is_active()) throw runtime_error("Inactive player cannot act.");
    if (game.turn() != name()) throw runtime_error("Not your turn.");
    if (coins() >= 10) throw runtime_error("Must perform coup with 10 coins.");
    if (is_sanctioned()) throw runtime_error("You are under sanction and cannot tax.");

    add_coins(3);
    last_action = "tax";

    for (Player* p : game.get_all_players()) {
        if (p->is_active() && p->role() == "Governor" && p != this) {
            game.set_last_tax_index(game.get_player_index(this));
            game.set_awaiting_tax_block(true);
            game.set_tax_target(this);
            game.set_tax_source(p);
            game.set_turn_to(p);
            return;
        }
    }

    game.next_turn();
}

/// Blocks a tax action, refunds coins from the target, and resumes turn order.
void Governor::block_tax(Player& target) {
    if (game.num_players() < 2) {
        throw runtime_error("Game has not started – need at least 2 players.");
    }
    if (!is_active() || !target.is_active()) return;
    if (target.get_last_action() != "tax") throw runtime_error("Target didn't tax.");

    int refund = (dynamic_cast<Governor*>(&target)) ? 3 : 2;
    if (target.coins() < refund) throw runtime_error("Not enough coins to undo tax.");

    target.deduct_coins(refund);
    target.clear_last_action();

    game.set_awaiting_tax_block(false);
    game.set_tax_target(nullptr);
    game.set_tax_source(nullptr);

    int index = game.get_last_tax_index();
    const auto& players = game.get_all_players();
    do {
        index = (index + 1) % players.size();
    } while (!players[index]->is_active());

    game.set_turn_to(players[index]);
    game.set_turn_index(index);
}

/// Skips current Governor's tax block and moves to next or resumes game.
    void Governor::skip_tax_block() {
    if (!game.is_awaiting_tax_block()) {
        throw runtime_error("Game has not started – need at least 2 players.");
    }

    std::cout << "[DEBUG] Governor " << name() << " is skipping tax block.\n";
    Player* next = game.pop_next_tax_blocker();
    if (next) {
        std::cout << "[DEBUG] Passing tax block to next Governor: " << next->name() << "\n";
        game.set_tax_source(next);
        game.set_turn_to(next);
    } else {
        std::cout << "[DEBUG] No more Governors to block tax, resolving.\n";
        Player* tax_target = game.get_tax_target();
        game.set_awaiting_tax_block(false);
        game.set_tax_target(nullptr);
        game.set_tax_source(nullptr);

        // התקדם לשחקן הבא אחרי ה-target
        if (tax_target && tax_target->is_active()) {
            int target_index = game.get_player_index(tax_target);
            const auto& players = game.get_all_players();
            int next_index = target_index;
            do {
                next_index = (next_index + 1) % players.size();
            } while (!players[next_index]->is_active());
            std::cout << "[DEBUG] Advancing turn to next player after tax target: " << players[next_index]->name() << "\n";
            game.set_turn_to(players[next_index]);
        } else {
            game.next_turn();
        }
    }
}

}
