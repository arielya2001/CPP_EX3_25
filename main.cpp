#include <iostream>
#include "Game.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

using namespace std;
using namespace coup;

void print_status(const Game& game) {
    cout << "\n--- Status ---" << endl;
    for (Player* p : game.get_all_players()) {
        cout << p->name() << " (" << p->role() << ") - Coins: " << p->coins()
             << " - " << (p->is_active() ? "Alive" : "Out") << endl;
    }
}

void game1() {
    cout << "\n==== Game 1 ====" << endl;
    Game game;

    Governor g(game, "Gili");
    Spy s(game, "Shai");
    Baron b(game, "Tamar");

    game.add_player(&g);
    game.add_player(&s);
    game.add_player(&b);

    g.gather(); print_status(game);
    s.gather(); print_status(game);
    b.gather(); print_status(game);

    g.tax(); print_status(game);
    s.tax(); print_status(game);
    g.block_tax(s); print_status(game);

    b.gather(); print_status(game);
    g.tax(); print_status(game);
    s.gather(); print_status(game);

    b.tax(); print_status(game);

    g.bribe(s); print_status(game);
    g.arrest(s); print_status(game);
    g.tax(); print_status(game);

    s.gather(); print_status(game);
    b.sanction(g); print_status(game);

    try {
        g.tax();
    } catch (const exception& e) {
        cout << "[Error] " << g.name() << ": " << e.what() << endl;
    }


    g.coup(b); print_status(game);
    s.tax(); print_status(game);
    g.block_tax(s); print_status(game);

    g.tax(); print_status(game);
    s.arrest(g); print_status(game);

    g.arrest(s); print_status(game);
    s.gather(); print_status(game);

    g.tax(); print_status(game);
    s.gather(); print_status(game);

    g.gather(); print_status(game);
    s.gather(); print_status(game);



    g.coup(s); print_status(game);

    cout << "\n🏆 Winner: " << game.winner() << endl;
}

void game2() {
    cout << "\n==== Game 2 ====" << endl;
    Game game;

    Baron bar(game, "Tamar");
    Merchant merch(game, "Noam");
    Governor gov(game, "Gili");
    Spy spidy(game, "spido");

    game.add_player(&bar);
    game.add_player(&merch);
    game.add_player(&gov);
    game.add_player(&spidy);

    auto debug_turn = [&](const std::string& label) {
        std::cout << "[Debug][" << label << "] game.turn(): " << game.turn()
                  << " | turn_index = " << game.get_turn_index()
                  << " | Expected: " << gov.name()
                  << " | (ptr match: "
                  << ((void*)game.get_all_players()[game.get_turn_index()] == (void*)&gov)
                  << ")" << std::endl;
    };

    bar.gather(); print_status(game);
    merch.gather(); print_status(game);
    gov.gather(); print_status(game);
    debug_turn("After gov.gather");

    spidy.gather(); print_status(game);
    debug_turn("After spidy.gather");

    std::cout << "[Debug] merch name(): " << merch.name() << std::endl;
    std::cout << "[Debug] game.turn(): " << game.turn() << std::endl;
    bar.tax(); print_status(game);
    debug_turn("After bar.tax");
    std::cout << "[Debug] merch name(): " << merch.name() << std::endl;
    std::cout << "[Debug] game.turn(): " << game.turn() << std::endl;

    gov.skip_tax_block(); print_status(game);
    std::cout << "[Debug] merch name(): " << merch.name() << std::endl;
    std::cout << "[Debug] game.turn(): " << game.turn() << std::endl;
    debug_turn("After gov.skip_tax_block 1");

    std::cout << "[Debug] merch name(): " << merch.name() << std::endl;
    std::cout << "[Debug] game.turn(): " << game.turn() << std::endl;
    merch.tax(); print_status(game);
    debug_turn("After merch.tax");

    gov.skip_tax_block(); print_status(game);
    gov.tax(); print_status(game);
    debug_turn("After gov.tax");

    spidy.tax(); print_status(game);
    debug_turn("After spidy.tax");

    gov.skip_tax_block(); print_status(game);
    debug_turn("After gov.skip_tax_block 2");

    bar.invest(); print_status(game);
    merch.tax(); print_status(game);
    gov.skip_tax_block(); print_status(game);
    gov.tax(); print_status(game);
    spidy.tax(); print_status(game);
    gov.skip_tax_block(); print_status(game);

    bar.tax(); print_status(game);
    gov.skip_tax_block(); print_status(game);
    merch.tax(); print_status(game);
    gov.skip_tax_block(); print_status(game);

    gov.undo(spidy); print_status(game);
    spidy.spy_on(bar); print_status(game);
    spidy.block_arrest(bar); print_status(game);
    spidy.tax(); print_status(game);
    gov.skip_tax_block(); print_status(game);

    try {
        bar.arrest(merch); print_status(game);
    } catch (const exception& e) {
        cout << ": " << e.what() << endl;
    }

    bar.coup(spidy); print_status(game);
    merch.coup(gov); print_status(game);

    bar.tax(); print_status(game);
    merch.tax(); print_status(game);
    bar.tax(); print_status(game);
    merch.tax(); print_status(game);
    bar.tax(); print_status(game);
    merch.tax(); print_status(game);

    bar.coup(merch); print_status(game);

    cout << "\n🏆 Winner: " << game.winner() << endl;
}



int main() {
    game1();
    game2();
    return 0;
}
