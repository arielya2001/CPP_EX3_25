/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include <iostream>                                 // For standard I/O operations
#include "Game.hpp"                                 // Game logic and management
#include "Governor.hpp"                             // Governor role definition
#include "Spy.hpp"                                  // Spy role definition
#include "Baron.hpp"                                // Baron role definition
#include "General.hpp"                              // General role definition
#include "Judge.hpp"                                // Judge role definition
#include "Merchant.hpp"                             // Merchant role definition

using namespace std;
using namespace coup;

/// Prints the current status of all players in the game
void print_status(const Game& game) {
    cout << "\n--- Status ---" << endl;             // Header for game status
    for (Player* p : game.get_all_players()) {      // Loop through all players
        cout << p->name() << " (" << p->role() << ") - Coins: " << p->coins()
             << " - " << (p->is_active() ? "Alive" : "Out") << endl;  // Print each player's status
    }
}

/// Runs a full sample game between Governor, Spy, and Baron
void game1() {
    cout << "\n==== Game 1 ====" << endl;           // Header for game1
    Game game;                                      // Create a new game

    Governor g(game, "Gili");                       // Add Governor named Gili
    Spy s(game, "Shai");                            // Add Spy named Shai
    Baron b(game, "Tamar");                         // Add Baron named Tamar

    game.add_player(&g);                            // Register Gili in the game
    game.add_player(&s);                            // Register Shai in the game
    game.add_player(&b);                            // Register Tamar in the game

    g.gather(); print_status(game);                 // Gili gathers 1 coin
    s.gather(); print_status(game);                 // Shai gathers 1 coin
    b.gather(); print_status(game);                 // Tamar gathers 1 coin

    g.tax(); print_status(game);                    // Gili taxes (3 coins)
    s.tax(); print_status(game);                    // Shai taxes (2 coins, since not a Governor)
    g.block_tax(s); print_status(game);             // Gili blocks Shai's tax

    b.gather(); print_status(game);                 // Tamar gathers 1 coin
    g.tax(); print_status(game);                    // Gili taxes again
    s.gather(); print_status(game);                 // Shai gathers

    b.tax(); print_status(game);                    // Tamar taxes (3 coins)

    g.gather(); print_status(game);                 // Gili gathers
    s.gather(); print_status(game);                 // Shai gathers
    b.invest(); print_status(game);                 // Tamar invests (pays 3, gains 6)

    g.bribe(); print_status(game);                  // Gili bribes (bonus turn expected)
    g.arrest(s); print_status(game);                // Gili arrests Shai
    g.tax(); print_status(game);                    // Gili taxes

    s.gather(); print_status(game);                 // Shai gathers
    b.sanction(g); print_status(game);              // Tamar sanctions Gili

    try {
        g.tax();                                    // Gili attempts to tax while sanctioned
    } catch (const exception& e) {
        cout << "[Error] " << g.name() << ": " << e.what() << endl;  // Handle tax failure
    }
    g.sanction(b); print_status(game);              // Gili sanctions Tamar
    s.gather(); print_status(game);                 // Shai gathers
    b.arrest(g); print_status(game);                // Tamar arrests Gili

    g.tax(); print_status(game);                    // Gili taxes
    s.gather(); print_status(game);                 // Shai gathers
    b.gather(); print_status(game);                 // Tamar gathers

    g.coup(b); print_status(game);                  // Gili performs a coup on Tamar
    s.tax(); print_status(game);                    // Shai taxes
    g.block_tax(s); print_status(game);             // Gili blocks Shai’s tax

    g.tax(); print_status(game);                    // Gili taxes
    s.arrest(g); print_status(game);                // Shai arrests Gili

    g.arrest(s); print_status(game);                // Gili arrests Shai
    s.gather(); print_status(game);                 // Shai gathers

    g.tax(); print_status(game);                    // Gili taxes
    s.gather(); print_status(game);                 // Shai gathers

    g.gather(); print_status(game);                 // Gili gathers
    s.tax(); print_status(game);                    // Shai taxes
    g.skip_tax_block(); print_status(game);         // Gili skips tax block phase
    g.gather(); print_status(game);                 // Gili gathers again

    s.gather(); print_status(game);                 // Shai gathers

    g.coup(s); print_status(game);                  // Gili performs a coup on Shai

    cout << "\n🏆 Winner: " << game.winner() << endl; // Announce the winner
}

/// Runs a full sample game between General, Judge, Merchant and Spy
void game2() {
    cout << "\n==== Game 2 ====" << endl;           // Header for game2
    Game game;                                      // New game instance

    General gen(game, "Dana");                      // Add General Dana
    Judge jud(game, "Lior");                        // Add Judge Lior
    Merchant mer(game, "Noam");                     // Add Merchant Noam
    Spy spy(game, "Shai");                          // Add Spy Shai

    game.add_player(&gen);                          // Register Dana
    game.add_player(&jud);                          // Register Lior
    game.add_player(&mer);                          // Register Noam
    game.add_player(&spy);                          // Register Shai

    // Phase 1: Resource gathering
    gen.tax(); print_status(game);                  // Dana taxes
    jud.gather(); print_status(game);               // Lior gathers
    mer.tax(); print_status(game);                  // Noam taxes
    spy.gather(); print_status(game);               // Shai gathers

    // Phase 2: Interactive actions
    gen.tax(); print_status(game);                  // Dana taxes
    jud.tax(); print_status(game);                  // Lior taxes
    mer.gather(); print_status(game);               // Noam gathers
    cout << spy.spy_on(mer); print_status(game);    // Shai spies on Noam
    spy.gather(); print_status(game);               // Shai gathers

    // Phase 3: Blocking actions
    gen.arrest(mer); print_status(game);            // Dana arrests Noam
    jud.tax(); print_status(game);                  // Lior taxes
    mer.tax(); print_status(game);                  // Noam taxes
    spy.tax(); print_status(game);                  // Shai taxes

    gen.bribe(); print_status(game);                // Dana bribes
    jud.block_bribe(gen); print_status(game);       // Lior blocks Dana's bribe
    jud.tax(); print_status(game);                  // Lior taxes
    mer.tax(); print_status(game);                  // Noam taxes
    spy.tax(); print_status(game);                  // Shai taxes

    // Phase 4: Advanced actions
    gen.tax(); print_status(game);                  // Dana taxes
    jud.tax(); print_status(game);                  // Lior taxes
    mer.sanction(jud); print_status(game);          // Noam sanctions Lior
    spy.gather(); print_status(game);               // Shai gathers

    gen.gather(); print_status(game);               // Dana gathers
    try {
        jud.tax();                                  // Lior attempts tax while sanctioned
    } catch (const exception& e) {
        cout << "[Error] " << jud.name() << ": " << e.what() << endl;  // Handle tax failure
    }
    jud.arrest(spy); print_status(game);            // Lior arrests Shai
    mer.gather(); print_status(game);               // Noam gathers
    spy.gather(); print_status(game);               // Shai gathers

    // Phase 5: Coup attempts and blocks
    gen.tax(); print_status(game);                  // Dana taxes
    try {
        jud.gather();                               // Lior attempts to gather (might be inactive)
    } catch (const exception& e) {
        cout << "[Error] " << jud.name() << ": " << e.what() << endl;  // Handle gather failure
    }
    jud.coup(mer); print_status(game);              // Lior coups Noam
    game.skip_coup_block(); print_status(game);     // Skip coup block
    spy.block_arrest(gen); print_status(game);      // Shai blocks Dana's arrest
    spy.gather(); print_status(game);               // Shai gathers
    try {
        gen.arrest(spy);                            // Dana tries to arrest Shai
    } catch (const exception& e) {
        cout << "[Error] " << gen.name() << ": " << e.what() << endl;  // Handle arrest failure
    }
    gen.tax(); print_status(game);                  // Dana taxes
    jud.tax(); print_status(game);                  // Lior taxes
    game.set_turn_to(&gen); print_status(game);     // Force turn to Dana

    gen.gather(); print_status(game);               // Dana gathers
    jud.gather(); print_status(game);               // Lior gathers
    spy.gather(); print_status(game);               // Shai gathers

    gen.gather(); print_status(game);               // Dana gathers
    jud.arrest(gen); print_status(game);            // Lior arrests Dana
    spy.gather(); print_status(game);               // Shai gathers

    // Phase 6: Final coups
    gen.coup(spy); print_status(game);              // Dana coups Shai
    jud.tax(); print_status(game);                  // Lior taxes
    gen.tax(); print_status(game);                  // Dana taxes
    jud.gather(); print_status(game);               // Lior gathers
    gen.gather(); print_status(game);               // Dana gathers
    jud.gather(); print_status(game);               // Lior gathers
    gen.sanction(jud); print_status(game);          // Dana sanctions Lior
    jud.coup(gen); print_status(game);              // Lior coups Dana
    cout << "\n🏆 Winner: " << game.winner() << endl; // Announce winner of game2
}

/// Entry point – runs both sample games
int main() {
    game1();                                        // Run first game
    game2();                                        // Run second game
    return 0;                                       // Program completed
}
