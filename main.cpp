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
    b.tax(); print_status(game);

    g.tax(); print_status(game);
    s.tax(); print_status(game);
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
    g.tax(); print_status(game);
    s.coup(g); print_status(game);

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

    bar.gather(); print_status(game);
    merch.gather(); print_status(game);
    gov.gather(); print_status(game);
    spidy.gather(); print_status(game);


    bar.tax(); print_status(game);
    merch.tax(); print_status(game);
    gov.tax(); print_status(game);
    spidy.tax(); print_status(game);

    bar.invest(); print_status(game);
    merch.tax(); print_status(game);
    gov.tax(); print_status(game);
    spidy.tax(); print_status(game);

    bar.tax(); print_status(game); //8
    merch.tax(); print_status(game);
    gov.undo(spidy); print_status(game);
    spidy.spy_on(bar); print_status(game);
    spidy.block_arrest(bar); print_status(game);
    spidy.tax(); print_status(game);
    try {
        bar.arrest(merch); print_status(game);
    } catch (const exception& e) {
        cout << ":" << e.what() << endl;
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
