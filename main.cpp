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

/// Prints the current status of all players in the game
void print_status(const Game& game) {
    cout << "\n--- Status ---" << endl;
    for (Player* p : game.get_all_players()) {
        cout << p->name() << " (" << p->role() << ") - Coins: " << p->coins()
             << " - " << (p->is_active() ? "Alive" : "Out") << endl;
    }
}

/// Runs a full sample game between Governor, Spy, and Baron
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

    g.gather(); print_status(game);
    s.gather(); print_status(game);
    b.invest(); print_status(game);

    g.bribe(); print_status(game);
    g.arrest(s); print_status(game);
    g.tax(); print_status(game);

    s.gather(); print_status(game);
    b.sanction(g); print_status(game);

    try {
        g.tax();
    } catch (const exception& e) {
        cout << "[Error] " << g.name() << ": " << e.what() << endl;
    }
    g.sanction(b); print_status(game);
    s.gather(); print_status(game);
    b.arrest(g); print_status(game);

    g.tax(); print_status(game);
    s.gather(); print_status(game);
    b.gather(); print_status(game);

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
    s.tax(); print_status(game);
    g.skip_tax_block(); print_status(game);
    g.gather(); print_status(game);

    s.gather(); print_status(game);

    g.coup(s); print_status(game);

    cout << "\n🏆 Winner: " << game.winner() << endl;
}

/// Runs a full sample game between General, Judge, Merchant and Spy
void game2() {
    cout << "\n==== Game 2 ====" << endl;
    Game game;

    General gen(game, "Dana");
    Judge jud(game, "Lior");
    Merchant mer(game, "Noam");
    Spy spy(game, "Shai");

    game.add_player(&gen);
    game.add_player(&jud);
    game.add_player(&mer);
    game.add_player(&spy);

    // שלב 1: איסוף משאבים
    gen.tax(); print_status(game);
    jud.gather(); print_status(game);
    mer.tax(); print_status(game);
    spy.gather(); print_status(game);

    // שלב 2: פעולות אינטראקטיביות
    gen.tax(); print_status(game);
    jud.tax(); print_status(game);
    mer.gather(); print_status(game);
    cout << spy.spy_on(mer); print_status(game);
    spy.gather(); print_status(game);

    // שלב 3: פעולות עם חסימות
    gen.arrest(mer); print_status(game);
    jud.tax(); print_status(game);
    mer.tax(); print_status(game);
    spy.tax(); print_status(game);

    gen.bribe(); print_status(game);
    jud.block_bribe(gen); print_status(game);
    jud.tax(); print_status(game);
    mer.tax(); print_status(game);
    spy.tax(); print_status(game);

    // שלב 4: פעולות מתקדמות
    gen.tax(); print_status(game);
    jud.tax(); print_status(game);
    mer.sanction(jud); print_status(game);
    spy.gather(); print_status(game);

    gen.gather(); print_status(game);
    try {
        jud.tax();
    } catch (const exception& e) {
        cout << "[Error] " << jud.name() << ": " << e.what() << endl;
    }
    jud.arrest(spy); print_status(game);
    mer.gather(); print_status(game);
    spy.gather(); print_status(game);

    // שלב 5: הגנה והפיכות
    gen.tax(); print_status(game);
    try {
        jud.gather();
    } catch (const exception& e) {
        cout << "[Error] " << jud.name() << ": " << e.what() << endl;
    }
    jud.coup(mer); print_status(game);
    game.skip_coup_block(); print_status(game);
    spy.block_arrest(gen); print_status(game);
    try {
        gen.arrest(spy);
    } catch (const exception& e) {
        cout << "[Error] " << gen.name() << ": " << e.what() << endl;
    }
    gen.tax(); print_status(game);
    jud.tax(); print_status(game);
    spy.gather(); print_status(game);

    gen.gather(); print_status(game);
    jud.gather(); print_status(game);
    spy.gather(); print_status(game);

    gen.gather(); print_status(game);
    jud.arrest(gen); print_status(game);
    spy.gather(); print_status(game);

    // שלב 6: סיום עם הפיכות
    gen.coup(spy); print_status(game);
    jud.tax(); print_status(game);
    gen.tax(); print_status(game);
    jud.gather(); print_status(game);
    gen.gather(); print_status(game);
    jud.gather(); print_status(game);
    gen.sanction(jud); print_status(game);
    jud.coup(gen); print_status(game);
    cout << "\n🏆 Winner: " << game.winner() << endl;
}

/// Entry point – runs both sample games
int main() {
    game1();
    game2();
    return 0;
}
