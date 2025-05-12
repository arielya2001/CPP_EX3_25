#include "Governor.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

Governor::Governor(Game& game, const std::string& name)
    : Player(game, name, "Governor") {}

    void Governor::tax() {
    if (!is_active()) throw runtime_error("Inactive player cannot act.");
    if (game.turn() != name()) throw runtime_error("Not your turn.");
    if (coins() >= 10) throw runtime_error("Must perform coup with 10 coins.");
    if (is_sanctioned()) throw runtime_error("You are under sanction and cannot tax.");

    add_coins(3);
    last_action = "tax";

    for (Player* p : game.get_all_players()) {
        if (p->is_active() && p->role() == "Governor" && p != this) {
            // ⬅️ תיקון חשוב – חייבים לשמור את אינדקס השחקן שתקף
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


void Governor::undo(Player& target) {
    if (!is_active()) throw runtime_error("Governor is not active.");
    if (!target.is_active()) throw runtime_error("Target is not active.");
    if (target.get_last_action() != "tax") {
        throw runtime_error("Cannot undo: last action was not tax.");
    }

    if (target.coins() < 2) {
        throw runtime_error("Target doesn't have enough coins to undo.");
    }

    target.deduct_coins(2);
    target.clear_last_action();
    game.next_turn();
}

    void Governor::block_tax(Player& target) {
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
    game.set_turn_index(index);  // ← נשמור את המיקום החדש

}




    void Governor::skip_tax_block() {
    Player* next = game.pop_next_tax_blocker();
    if (next) {
        game.set_turn_to(next);
    } else {
        game.set_awaiting_tax_block(false);
        Player* taxed = game.get_tax_target();
        game.set_tax_target(nullptr);
        game.set_tax_source(nullptr);
        int index = game.get_last_tax_index();
        const auto& players = game.get_all_players();
        do { index = (index + 1) % players.size(); } while (!players[index]->is_active());
        game.set_turn_to(players[index]);
        game.set_turn_index(index);  // ← נשמור את המיקום החדש
    }
}



}
