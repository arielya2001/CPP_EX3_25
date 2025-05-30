/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN            // Enables doctest's main function
#include "doctest.h"                                  // Include doctest framework
#include "Game.hpp"                                   // Game class header
#include "Player.hpp"                                 // Base Player class
#include "Spy.hpp"                                    // Spy role implementation
#include "Baron.hpp"                                  // Baron role implementation
#include "General.hpp"                                // General role implementation
#include "Judge.hpp"                                  // Judge role implementation
#include "Merchant.hpp"                               // Merchant role implementation
#include "Governor.hpp"                               // Governor role implementation
#include <stdexcept>                                  // For throwing runtime errors

using namespace coup;                                 // Use coup namespace for convenience
using namespace std;                                  // Use standard namespace

TEST_CASE("בדיקת שגיאות עבור פעולות שחקן") {          // Test suite for player actions
    Game game;                                        // Create game instance

    Governor gov(game, "Gili");                       // Create Governor player
    Spy spy(game, "Shai");                            // Create Spy player
    Baron baron(game, "Tamar");                       // Create Baron player
    Merchant merch(game, "Noam");                     // Create Merchant player
    General general(game, "Dana");                    // Create General player
    Judge judge(game, "Lior");                        // Create Judge player

    game.add_player(&gov);                            // Add Governor to game
    game.add_player(&spy);                            // Add Spy to game
    game.add_player(&baron);                          // Add Baron to game
    game.add_player(&merch);                          // Add Merchant to game
    game.add_player(&general);                        // Add General to game
    game.add_player(&judge);                          // Add Judge to game

    SUBCASE("Player::gather - Successful execution") {
        game.set_turn_to(&gov);                       // Set turn to Governor
        gov.add_coins(5);                             // Ensure less than 10 coins
        int initial_coins = gov.coins();              // Store coin count before
        gov.gather();                                 // Perform gather (add 1 coin)
        CHECK(gov.coins() == initial_coins + 1);      // Check if coin was added
        CHECK(game.turn() == "Shai");                 // Turn should now be Spy
    }

    SUBCASE("Governor::tax - Successful execution") {
        game.set_turn_to(&gov);                       // Set turn to Governor
        gov.add_coins(5);                             // Add starting coins
        int initial_coins = gov.coins();              // Store coin count before
        gov.tax();                                    // Perform tax (+3 coins)
        CHECK(gov.coins() == initial_coins + 3);      // Verify coin addition
        CHECK_FALSE(game.is_awaiting_tax_block());    // No block stage expected
        CHECK(game.get_tax_target() == nullptr);      // No target for tax
        CHECK(game.turn() == "Shai");                 // Turn should go to Spy
    }

    SUBCASE("Player::bribe - Successful execution") {
        game.set_turn_to(&gov);                       // Set turn to Governor
        gov.add_coins(4);                             // Enough coins for bribe
        int initial_coins = gov.coins();              // Store coin count before
        gov.bribe();                                  // Perform bribe
        CHECK(gov.coins() == initial_coins - 4);      // Deduct bribe cost
        CHECK(gov.has_bonus_turn());                  // Bonus turn granted
        CHECK(game.is_awaiting_bribe_block());        // Judge should block
        CHECK(game.turn() == "Lior");                 // Turn moved to Judge
        game.advance_bribe_block_queue();             // Skip Judge's block
        CHECK(game.turn() == "Gili");                 // Return to Governor's bonus turn
    }

    SUBCASE("Player::arrest - Successful execution on General") {
        game.set_turn_to(&gov);                       // Governor's turn
        general.add_coins(2);                         // General has coins
        int gov_initial_coins = gov.coins();          // Coins before arrest
        int general_initial_coins = general.coins();  // Coins of General
        gov.arrest(general);                          // Perform arrest
        CHECK(gov.coins() == gov_initial_coins);      // Governor gets no coins
        CHECK(general.coins() == general_initial_coins); // General refunds
        CHECK(game.turn() == "Shai");                 // Turn passed to Spy
    }

    SUBCASE("Player::arrest - Successful execution on Merchant") {
        game.set_turn_to(&gov);                       // Set turn to Governor
        merch.add_coins(2);                           // Merchant has coins
        int gov_initial_coins = gov.coins();          // Before arrest
        int merch_initial_coins = merch.coins();      // Before arrest
        gov.arrest(merch);                            // Perform arrest
        CHECK(gov.coins() == gov_initial_coins);      // Governor unchanged
        CHECK(merch.coins() == merch_initial_coins - 2); // Merchant loses 2
        CHECK(game.turn() == "Shai");                 // Next is Spy
    }

    SUBCASE("Player::sanction - Successful execution on Baron") {
        game.set_turn_to(&gov);                       // Governor’s turn
        gov.add_coins(3);                             // Enough for sanction
        baron.add_coins(2);                           // Baron has coins
        int gov_initial_coins = gov.coins();          // Store state
        int baron_initial_coins = baron.coins();      // Store state
        gov.sanction(baron);                          // Apply sanction
        CHECK(gov.coins() == gov_initial_coins - 3);  // Deduct from Governor
        CHECK(baron.coins() == baron_initial_coins + 1); // Baron rewarded
        CHECK(baron.is_sanctioned());                 // Baron flagged
        CHECK(game.turn() == "Shai");                 // Turn passed
    }

    SUBCASE("Player::sanction - Successful execution on Judge") {
        game.set_turn_to(&gov);                       // Set turn
        gov.add_coins(5);                             // Enough for penalty
        judge.add_coins(2);                           // Judge has coins
        int gov_initial_coins = gov.coins();          // Store state
        int judge_initial_coins = judge.coins();      // Store state
        gov.sanction(judge);                          // Sanction judge
        CHECK(gov.coins() == gov_initial_coins - 4);  // Deduct 4 (3 + penalty)
        CHECK(judge.is_sanctioned());                 // Judge is sanctioned
        CHECK(game.turn() == "Shai");                 // Turn advanced
    }

    SUBCASE("Player::coup - Successful execution") {
        game.set_turn_to(&gov);                       // Governor’s turn
        gov.add_coins(7);                             // Enough for coup
        general.add_coins(5);                         // General has coins
        int initial_coins = gov.coins();              // Store state
        gov.coup(spy);                                // Coup the spy
        CHECK(gov.coins() == initial_coins - 7);      // Deduct 7 coins
        CHECK(game.is_awaiting_coup_block());         // Await block
        CHECK(game.get_coup_target() == &spy);        // Target = spy
        CHECK(game.turn() == "Dana");                 // General blocks
        game.skip_coup_block();                       // Skip block
        CHECK(!spy.is_active());                      // Spy is out
        CHECK_FALSE(game.is_awaiting_coup_block());   // Block state cleared
    }

    SUBCASE("Baron::invest - Successful execution") {
        game.set_turn_to(&baron);                     // Baron’s turn
        baron.add_coins(3);                           // Enough to invest
        int initial_coins = baron.coins();            // Store coin count
        baron.invest();                               // Perform invest
        CHECK(baron.coins() == initial_coins + 3);    // Gain net 3
        CHECK(game.turn() == "Noam");                 // Turn to Merchant
    }

    SUBCASE("Spy::spy_on - Successful execution") {
        game.set_turn_to(&spy);                       // Spy’s turn
        gov.add_coins(5);                             // Governor has coins
        int spy_initial_coins = spy.coins();          // Spy's coins
        int gov_initial_coins = gov.coins();          // Gov's coins
        int coins_seen = spy.spy_on(gov);             // Spy sees coins
        CHECK(coins_seen == gov_initial_coins);       // Must match
        CHECK(spy.coins() == spy_initial_coins);      // Spy's unchanged
        CHECK(gov.coins() == gov_initial_coins);      // Gov unchanged
        CHECK(game.turn() == "Shai");                 // Turn not changed
    }

    SUBCASE("Governor::block_tax - Successful execution") {
        game.set_turn_to(&spy);                       // Spy's turn
        spy.add_coins(5);                             // Enough for tax
        int initial_coins = spy.coins();              // Coins before
        spy.tax();                                    // Perform tax
        game.set_turn_to(&gov);                       // Governor blocks
        gov.block_tax(spy);                           // Block tax
        CHECK(spy.coins() == initial_coins);          // Tax undone
        CHECK_FALSE(game.is_awaiting_tax_block());    // Clear block
        CHECK(game.turn() == "Tamar");                // Turn to Baron
    }

    SUBCASE("Judge::block_bribe - Successful execution") {
        game.set_turn_to(&gov);                       // Governor’s turn
        gov.add_coins(4);                             // Enough for bribe
        gov.bribe();                                  // Perform bribe
        int judge_initial_coins = judge.coins();      // Judge’s coins
        game.set_turn_to(&judge);                     // Judge’s turn
        judge.block_bribe(gov);                       // Block bribe
        CHECK(judge.coins() == judge_initial_coins);  // No coins gained
        CHECK_FALSE(game.is_awaiting_bribe_block());  // Clear block
        CHECK_FALSE(gov.has_bonus_turn());            // Bonus revoked
        CHECK(game.turn() == "Shai");                 // Turn to Spy
    }

    SUBCASE("Spy::block_arrest - Successful execution") {
        game.set_turn_to(&spy);                       // Spy's turn
        gov.add_coins(2);                             // Governor can arrest
        spy.block_arrest(gov);                        // Block arrest
        CHECK(spy.is_arrest_blocked(&gov));           // Block is active
        game.set_turn_to(&gov);                       // Try to arrest
        CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error); // Must fail
        CHECK(game.turn() == "Gili");                 // Turn remains
    }

    SUBCASE("General::block_coup - Successful execution") {
        game.set_turn_to(&spy);                       // Spy's turn
        spy.add_coins(7);                             // Enough for coup
        general.add_coins(5);                         // Enough to block
        spy.coup(baron);                              // Coup Baron
        int general_initial_coins = general.coins();  // Before block
        game.block_coup(&general);                    // General blocks
        CHECK(general.coins() == general_initial_coins - 5); // Deduct 5
        CHECK(baron.is_active());                     // Baron survives
        CHECK_FALSE(game.is_awaiting_coup_block());   // Clear state
    }

    SUBCASE("Merchant::turn_start_bonus - Successful execution") {
        game.set_turn_to(&merch);                     // Merchant's turn
        merch.add_coins(3);                           // Enough for bonus
        int initial_coins = merch.coins();            // Before bonus
        game.set_turn_to(&merch);                     // Triggers bonus
        CHECK(merch.coins() == initial_coins + 1);    // Gains 1 coin
        CHECK(game.turn() == "Noam");                 // Turn stays
    }

SUBCASE("Player::gather - שגיאות") {
        gov.deactivate();                                 // Deactivate the player
        CHECK_THROWS_AS(gov.gather(), runtime_error);     // Expect runtime error when gathering
        CHECK_THROWS_WITH(gov.gather(), "Inactive player cannot play."); // Specific error message for inactive player
        gov.set_active(true);                              // Reactivate the player

        game.set_turn_to(&spy);                            // Set turn to a different player (Spy)
        CHECK_THROWS_AS(gov.gather(), runtime_error);      // Gathering not allowed if it's not your turn
        CHECK_THROWS_WITH(gov.gather(), "Not your turn."); // Specific error for wrong turn

        game.set_turn_to(&gov);                            // Set turn back to Governor
        gov.add_coins(10);                                 // Give Governor 10 coins
        CHECK_THROWS_AS(gov.gather(), runtime_error);      // Can't gather if you have 10+ coins
        CHECK_THROWS_WITH(gov.gather(), "Must perform coup with 10 coins."); // Error for not performing coup
        gov.deduct_coins(10);                              // Remove coins to allow normal actions

        gov.set_sanctioned(true);                          // Set Governor as sanctioned
        CHECK_THROWS_AS(gov.gather(), runtime_error);      // Gathering while sanctioned should throw
        CHECK_THROWS_WITH(gov.gather(), "You are sanctioned and cannot gather."); // Specific sanction error
        gov.set_sanctioned(false);                         // Remove sanction
    }

    SUBCASE("Player::tax - שגיאות") {
        gov.deactivate();                                 // Deactivate Governor
        CHECK_THROWS_AS(gov.tax(), runtime_error);        // Taxing while inactive should fail
        CHECK_THROWS_WITH(gov.tax(), "Inactive player cannot act."); // Specific error for inactive tax
        gov.set_active(true);                             // Reactivate Governor

        game.set_turn_to(&spy);                           // Set turn to another player
        CHECK_THROWS_AS(gov.tax(), runtime_error);        // Taxing when it's not your turn should fail
        CHECK_THROWS_WITH(gov.tax(), "Not your turn.");   // Specific error message

        game.set_turn_to(&gov);                           // Set turn to Governor
        gov.add_coins(10);                                // Give 10 coins (too many)
        CHECK_THROWS_AS(gov.tax(), runtime_error);        // Must coup if holding 10 coins
        CHECK_THROWS_WITH(gov.tax(), "Must perform coup with 10 coins."); // Specific error
        gov.deduct_coins(10);                             // Remove excess coins

        gov.set_sanctioned(true);                         // Apply sanction
        CHECK_THROWS_AS(gov.tax(), runtime_error);        // Sanctioned player can't tax
        CHECK_THROWS_WITH(gov.tax(), "You are under sanction and cannot tax."); // Sanction-specific error
        gov.set_sanctioned(false);                        // Remove sanction
    }

    SUBCASE("Player::bribe - שגיאות") {
        gov.deactivate();                                 // Deactivate Governor
        CHECK_THROWS_AS(gov.bribe(), runtime_error);      // Bribe should fail if inactive
        CHECK_THROWS_WITH(gov.bribe(), "Inactive player cannot play."); // Inactive bribe error
        gov.set_active(true);                             // Reactivate Governor

        game.set_turn_to(&spy);                           // Set turn to another player
        CHECK_THROWS_AS(gov.bribe(), runtime_error);      // Bribe should fail if not your turn
        CHECK_THROWS_WITH(gov.bribe(), "Not your turn."); // Specific error message

        game.set_turn_to(&gov);                           // Set turn to Governor
        gov.deduct_coins(gov.coins());                    // Remove all coins
        CHECK_THROWS_AS(gov.bribe(), runtime_error);      // Not enough coins for bribe
        CHECK_THROWS_WITH(gov.bribe(), "Not enough coins for bribe."); // Specific message
    }

    SUBCASE("Player::arrest - שגיאות") {
        gov.deactivate();                                 // Deactivate Governor
        CHECK_THROWS_WITH(gov.arrest(spy), "Inactive player cannot play."); // Inactive can't arrest
        gov.set_active(true);                             // Reactivate Governor

        game.set_turn_to(&spy);                           // Set turn to Spy
        CHECK(game.turn() == "Shai");                     // Verify turn belongs to Spy
        CHECK_THROWS_WITH(gov.arrest(spy), "Not your turn."); // Error when not your turn

        game.set_turn_to(&gov);                           // Set turn to Governor
        spy.deactivate();                                 // Deactivate Spy
        CHECK_THROWS_WITH(gov.arrest(spy), "Target player is not active."); // Target must be active
        spy.set_active(true);                             // Reactivate Spy

        game.set_turn_to(&gov);                           // Governor's turn
        gov.add_coins(2);                                 // Give Governor 2 coins
        spy.add_coins(2);                                 // Give Spy 2 coins
        gov.arrest(spy);                                  // First arrest is valid
        CHECK(game.turn() == "Shai");                     // Next turn is Spy

        game.set_turn_to(&gov);                           // Governor's turn again
        CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row."); // Can't double arrest

        gov.gather();                                     // Skip turn
        spy.gather();                                     // Spy gathers too

        game.set_turn_to(&gov);                           // Back to Governor
        gov.add_coins(4);                                 // Add coins for bribe
        gov.bribe();                                      // Initiate bribe
        CHECK(game.turn() == "Lior");                     // Judge's block turn
        game.advance_bribe_block_queue();                 // Skip blocking
        gov.arrest(spy);                                  // First arrest after bribe
        CHECK(game.turn() == "Gili");                     // Turn returns to Governor
        CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row."); // Still invalid

        game.set_turn_to(&gov);                           // Governor's turn
        gov.add_coins(2);                                 // Add coins
        baron.add_coins(2);                               // Add coins to Baron
        gov.arrest(baron);                                // Valid arrest on new target
        CHECK(game.turn() != "Gili");                     // Turn advances

        game.set_turn_to(&gov);                           // Governor's turn
        spy.block_arrest(gov);                            // Spy blocks arrest
        CHECK_THROWS_WITH(gov.arrest(spy), "Arrest blocked by Spy."); // Arrest blocked
    }

        SUBCASE("Player::sanction - שגיאות") {
        gov.deactivate();                                        // Deactivate Governor
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);       // Sanction should throw when inactive
        CHECK_THROWS_WITH(gov.sanction(spy), "Inactive player cannot play."); // Specific message
        gov.set_active(true);                                    // Reactivate Governor

        game.set_turn_to(&spy);                                  // Turn is Spy's
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);       // Can't sanction if it's not your turn
        CHECK_THROWS_WITH(gov.sanction(spy), "Not your turn.");  // Specific error

        game.set_turn_to(&gov);                                  // Set turn to Governor
        spy.deactivate();                                        // Deactivate target (Spy)
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);       // Can't sanction inactive player
        CHECK_THROWS_WITH(gov.sanction(spy), "Target player is not active."); // Specific error
        spy.set_active(true);                                    // Reactivate Spy

        gov.deduct_coins(gov.coins());                           // Empty Governor's wallet
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);       // Not enough coins for sanction
        CHECK_THROWS_WITH(gov.sanction(spy), "Not enough coins for sanction."); // Specific error

        gov.add_coins(3);                                        // Add only 3 coins – enough for regular, not Judge
        CHECK_THROWS_AS(gov.sanction(judge), runtime_error);     // Judge requires more coins
        CHECK_THROWS_WITH(gov.sanction(judge), "Not enough coins for sanction."); // Specific message
    }

    SUBCASE("Player::coup - שגיאות") {
        gov.deactivate();                                        // Deactivate Governor
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);           // Coup can't be performed by inactive player
        CHECK_THROWS_WITH(gov.coup(spy), "Inactive player cannot play."); // Specific error
        gov.set_active(true);                                    // Reactivate Governor

        game.set_turn_to(&spy);                                  // Turn is Spy's
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);           // Not your turn – should throw
        CHECK_THROWS_WITH(gov.coup(spy), "Not your turn.");      // Specific error

        game.set_turn_to(&gov);                                  // Governor's turn
        spy.deactivate();                                        // Target already eliminated
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);           // Can't coup a deactivated player
        CHECK_THROWS_WITH(gov.coup(spy), "Target already out."); // Specific message
        spy.set_active(true);                                    // Reactivate Spy

        gov.deduct_coins(gov.coins());                           // No coins left
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);           // Not enough coins to coup
        CHECK_THROWS_WITH(gov.coup(spy), "Not enough coins to perform coup."); // Specific error
    }

    SUBCASE("Player::deduct_coins - שגיאות") {
        gov.deduct_coins(gov.coins());                           // Set coin count to 0
        CHECK_THROWS_AS(gov.deduct_coins(1), runtime_error);     // Attempt to deduct more than available
        CHECK_THROWS_WITH(gov.deduct_coins(1), "Not enough coins."); // Specific error
    }

    SUBCASE("Baron::invest - שגיאות") {
        Game game;                                               // New game instance
        Baron baron(game, "Tamar");                              // Create Baron
        Spy spy(game, "Shai");                                   // Create Spy

        game.add_player(&baron);                                 // Add one player
        CHECK_THROWS_WITH(baron.invest(), "Game has not started – need at least 2 players."); // Game needs 2 players

        game.add_player(&spy);                                   // Add second player
        game.set_turn_to(&baron);                                // Set turn to Baron

        baron.deactivate();                                      // Deactivate Baron
        CHECK_THROWS_WITH(baron.invest(), "Baron is not active."); // Inactive Baron can't invest

        baron.set_active(true);                                  // Reactivate Baron
        game.set_turn_to(&spy);                                  // Change turn to Spy
        CHECK_THROWS_WITH(baron.invest(), "Not your turn.");     // Not Baron's turn

        game.set_turn_to(&baron);                                // Set turn back to Baron
        baron.add_coins(10);                                     // Add too many coins
        CHECK_THROWS_WITH(baron.invest(), "Must coup with 10 coins."); // Baron must coup

        baron.deduct_coins(baron.coins());                       // Empty Baron's coins
        CHECK_THROWS_WITH(baron.invest(), "Not enough coins to invest."); // Not enough to invest
    }

        SUBCASE("Governor::tax - שגיאות") {
        gov.deactivate();                                        // הופך את Governor ללא פעיל
        CHECK_THROWS_AS(gov.tax(), runtime_error);              // צפוי לזרוק חריגה כששחקן לא פעיל מנסה tax
        CHECK_THROWS_WITH(gov.tax(), "Inactive player cannot act."); // בודק את הודעת החריגה המדויקת
        gov.set_active(true);                                    // מחזיר את השחקן למצב פעיל

        game.set_turn_to(&spy);                                  // מגדיר שהתור של Spy
        CHECK_THROWS_AS(gov.tax(), runtime_error);              // Governor לא יכול לפעול בתור של מישהו אחר
        CHECK_THROWS_WITH(gov.tax(), "Not your turn.");         // בדיקה להודעת השגיאה הנכונה

        game.set_turn_to(&gov);                                  // עכשיו תורו של Governor
        gov.add_coins(10);                                       // מוסיף לו 10 מטבעות
        CHECK_THROWS_AS(gov.tax(), runtime_error);              // חייב לבצע coup לפני כל פעולה אחרת
        CHECK_THROWS_WITH(gov.tax(), "Must perform coup with 10 coins.");
        gov.deduct_coins(10);                                    // מסיר את המטבעות כדי להמשיך

        gov.set_sanctioned(true);                                // מסמן שהשחקן תחת סנקציה
        CHECK_THROWS_AS(gov.tax(), runtime_error);              // שחקן בסנקציה לא יכול לבצע tax
        CHECK_THROWS_WITH(gov.tax(), "You are under sanction and cannot tax.");
    }

    SUBCASE("Governor::block_tax - שגיאות") {
        Game game;
        Governor gov(game, "Gili");                              // יוצר Governor חדש
        Spy spy(game, "Shai");                                   // יוצר Spy חדש

        game.add_player(&gov);                                   // מוסיף רק את Governor
        CHECK_THROWS_WITH(gov.block_tax(spy), "Game has not started – need at least 2 players.");
                                                                // משחק עם שחקן אחד לא נחשב כמתחיל

        game.add_player(&spy);                                   // מוסיף את Spy כדי להתחיל את המשחק

        spy.clear_last_action();                                 // מאפס את הפעולה האחרונה של Spy
        CHECK_THROWS_WITH(gov.block_tax(spy), "Target didn't tax."); // לא ניתן לחסום tax שלא בוצע

        game.set_turn_to(&spy);                                  // מגדיר שהתור של Spy
        CHECK(game.turn() == "Shai");                            // ודא שהתור באמת של Shai
        spy.add_coins(2);                                        // מוסיף לו מספיק מטבעות
        spy.tax();                                               // מבצע tax – פעולה לגיטימית
        spy.deduct_coins(spy.coins());                           // מאפס את המטבעות – כדי להפעיל את התנאי הבא
        CHECK_THROWS_WITH(gov.block_tax(spy), "Not enough coins to undo tax.");
                                                                // לא ניתן לחסום פעולה אם אין מה להחזיר
    }

    SUBCASE("Governor::skip_tax_block - שגיאות") {
        Game game;
        Governor gov(game, "Gili");                              // יוצר Governor
        game.add_player(&gov);                                   // מוסיף רק אותו
        CHECK_THROWS_WITH(gov.skip_tax_block(), "Game has not started – need at least 2 players.");
                                                                // פעולה אסורה לפני שהמשחק התחיל
    }

    SUBCASE("Judge::block_bribe - שגיאות") {
        Game game;
        Judge judge(game, "Lior");                               // יוצר Judge
        Spy spy(game, "Shai");                                   // יוצר Spy

        game.add_player(&judge);                                 // מוסיף רק את השופט
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Game has not started – need at least 2 players.");
                                                                // לא ניתן לחסום לפני שהמשחק התחיל

        game.add_player(&spy);                                   // מוסיף גם את Spy

        judge.deactivate();                                      // משבית את השופט
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Judge is not active."); // לא פעיל = לא חוסם
        judge.set_active(true);                                  // מחזיר אותו לפעיל

        spy.deactivate();                                        // השחקן שנחסם מושבת
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target is not active."); // לא ניתן לחסום שחקן לא פעיל
        spy.set_active(true);                                    // מחזיר את Spy לפעיל

        spy.clear_last_action();                                 // מאפס את הפעולה האחרונה
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target did not perform bribe.");
                                                                // אין פעולה של bribe, אז אין מה לחסום
    }

    SUBCASE("Merchant::בונוס בתחילת תור עם 3 מטבעות") {
        Game game;
        Merchant merch(game, "Mark");                            // יוצר Merchant
        Spy spy(game, "Shai");                                   // יוצר Spy

        game.add_player(&merch);                                 // מוסיף את הסוחר
        game.add_player(&spy);                                   // מוסיף את המרגל

        merch.add_coins(3);                                      // מוסיף בדיוק 3 מטבעות לפני תחילת התור

        game.set_turn_to(&merch);                                // כשהתור מתחיל – אמור לקבל בונוס
        CHECK(merch.coins() == 4);                               // קיבל +1 בונוס על התחלת תור

        merch.gather();                                          // פעולה רגילה – אמורה לתת עוד +1
        CHECK(merch.coins() == 5);                               // סך הכול: 3 + 1 בונוס + 1 gather
    }


        SUBCASE("Spy::spy_on - שגיאות") {
        spy.deactivate();                                       // הופך את המרגל לבלתי פעיל
        CHECK_THROWS_AS(spy.spy_on(gov), runtime_error);        // קריאה ל־spy_on על ידי שחקן לא פעיל צריכה לזרוק חריגה
        CHECK_THROWS_WITH(spy.spy_on(gov), "Spy is not active."); // בדיקה שההודעה מתאימה
        spy.set_active(true);                                   // מחזיר את השחקן למצב פעיל

        gov.deactivate();                                       // הופך את ה־Governor לבלתי פעיל
        CHECK_THROWS_AS(spy.spy_on(gov), runtime_error);        // לא ניתן לרגל אחרי שחקן לא פעיל
        CHECK_THROWS_WITH(spy.spy_on(gov), "Target is not active.");
        gov.set_active(true);                                   // מחזיר את השחקן למצב פעיל
    }

    SUBCASE("Spy::block_arrest - שגיאות") {
        Game game;
        Spy spy(game, "Shai");
        Governor gov(game, "Gili");

        game.add_player(&spy);                                  // מוסיף רק שחקן אחד למשחק
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Game has not started – need at least 2 players.");

        game.add_player(&gov);                                  // מוסיף שחקן נוסף – המשחק החל

        spy.deactivate();                                       // משבית את המרגל
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Spy is not active.");
        spy.set_active(true);                                   // מחזיר אותו לפעיל

        gov.deactivate();                                       // משבית את המושל
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Target is not active.");
        gov.set_active(true);                                   // מחזיר אותו לפעיל

        CHECK_NOTHROW(spy.block_arrest(gov));                   // הפעולה צריכה להתבצע ללא חריגה
    }

    SUBCASE("Spy::clear_expired_blocks - שגיאות") {
        Game game;
        Spy spy(game, "Shai");

        game.add_player(&spy);                                  // משחק עם שחקן אחד
        CHECK_THROWS_WITH(spy.clear_expired_blocks(), "Game has not started – need at least 2 players.");
    }

    SUBCASE("Game::add_player - שגיאות") {
        Game full_game;
        std::vector<Player*> allocated;

        for (int i = 0; i < 6; ++i) {
            Player* p = new Player(full_game, "Player" + to_string(i), "");
            allocated.push_back(p);
            full_game.add_player(p);                            // מוסיף 6 שחקנים (מגבלת המשחק)
        }

        Player* extra = new Player(full_game, "Extra", "");     // ניסיון להוסיף שחקן שביעי
        allocated.push_back(extra);
        CHECK_THROWS_WITH(full_game.add_player(extra), "Cannot add more than 6 players.");

        for (Player* p : allocated) {
            delete p;                                           // שחרור זיכרון כדי להימנע מזליגות
        }
    }

    SUBCASE("Game::turn - שגיאות") {
        Game empty_game;
        CHECK_THROWS_AS(empty_game.turn(), runtime_error);      // אין שחקנים – לא ניתן לקבל תור
        CHECK_THROWS_WITH(empty_game.turn(), "No players in the game.");

        Game one_player_game;
        Player p1(one_player_game, "P1", "");
        one_player_game.add_player(&p1);                        // רק שחקן אחד
        CHECK_THROWS_AS(one_player_game.turn(), runtime_error);
        CHECK_THROWS_WITH(one_player_game.turn(), "Game has not started – need at least 2 players.");

        Game game;
        Player p2(game, "P2", "");
        game.add_player(&p1);
        game.add_player(&p2);                                   // שני שחקנים – המשחק תקין
        CHECK_NOTHROW(game.turn());                             // כעת אמור לפעול בלי חריגה
    }

    SUBCASE("Game::skip_coup_block - שגיאות") {
        CHECK_THROWS_WITH(game.skip_coup_block(), "No coup block is pending.");
        // ניסיון לדלג על חסימת coup כשאין חסימה פעילה – צריך לזרוק חריגה
    }

    SUBCASE("Game::block_coup - שגיאות") {
        CHECK_THROWS_WITH(game.block_coup(&general), "No coup block is pending.");
        // ניסיון לחסום coup לפני שבוצע – שגיאה

        spy.add_coins(7);
        general.add_coins(5);                                   // הכנה לכך ש-General יוכל לחסום
        game.set_turn_to(&spy);
        spy.coup(baron);                                        // coup יוצר מצב של חסימה ממתינה
        CHECK(game.is_awaiting_coup_block());

        game.set_turn_to(&general);
        general.deactivate();
        general.set_active(false);                              // מוודא שהוא לא פעיל
        CHECK_THROWS_WITH(game.block_coup(&general), "Only an active General can block a coup.");
    }

    SUBCASE("Game::winner - שגיאות") {
        CHECK_THROWS_AS(game.winner(), runtime_error);          // המשחק עדיין לא נגמר
        CHECK_THROWS_WITH(game.winner(), "Game is still ongoing.");
    }

    SUBCASE("Game::set_turn_to - שגיאות") {
        // יצירת שחקן שלא נמצא במשחק הנוכחי
        Player non_existent(game, "NonExistent", "");
        // ניסיון לקבוע את תורו של שחקן שלא נמצא במשחק אמור לזרוק חריגה
        CHECK_THROWS_AS(game.set_turn_to(&non_existent), runtime_error);
        CHECK_THROWS_WITH(game.set_turn_to(&non_existent), "Player not found in game.");
    }

    SUBCASE("Game not started - שגיאות") {
        Game game;
        Governor gov(game, "Gili");
        Spy spy(game, "Shai");
        game.add_player(&gov); // רק שחקן אחד - המשחק טרם התחיל

        gov.add_coins(10); // מוסיף מטבעות כדי למנוע שגיאות "אין מספיק מטבעות"

        // כל הפעולות אמורות לזרוק חריגה על כך שהמשחק לא התחיל (פחות מ־2 שחקנים)
        CHECK_THROWS_WITH(gov.gather(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.tax(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.bribe(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.arrest(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.sanction(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.coup(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(spy.spy_on(gov), "Game has not started – need at least 2 players.");

        // הוספת שחקנים נוספים כדי להתחיל את המשחק
        Baron baron(game, "Bar");
        General general(game, "Gen");
        Judge judge(game, "Jud");
        game.add_player(&spy);
        game.add_player(&baron);
        game.add_player(&general);
        game.add_player(&judge);

        // פעולה חוקית אך לא בתור – אמורה לזרוק חריגה מתאימה
        CHECK_THROWS_WITH(baron.invest(), "Not your turn.");

        // ניסיון לחסום tax כאשר לא בוצע קודם tax
        CHECK_THROWS_WITH(gov.block_tax(gov), "Target didn't tax.");

        // ניסיון לחסום bribe כאשר היעד לא ביצע bribe
        CHECK_THROWS_WITH(judge.block_bribe(gov), "Target did not perform bribe.");
    }
}

TEST_CASE("בדיקת חסימת Tax בין Governors") {
    Game game;

    Governor gov(game, "Gili");
    Governor gov2(game, "Ariel");

    game.add_player(&gov);
    game.add_player(&gov2);

    SUBCASE("Tax Action") {
        game.set_turn_to(&gov); // קובע את תורו של גילי

        gov.tax(); // גילי מבצע tax

        // בדיקת התחלת מצב חסימת tax
        CHECK(game.is_awaiting_tax_block());
        CHECK(game.get_tax_target() == &gov);
        CHECK(game.get_tax_source() == &gov2);
        CHECK(game.turn() == "Ariel"); // התור עובר לגוברנור השני (Ariel)

        gov2.skip_tax_block(); // אריאל בוחר לא לחסום
        CHECK_FALSE(game.is_awaiting_tax_block()); // מצב חסימת tax אמור להסתיים
        CHECK(gov.coins() == 3); // קיבל 3 מטבעות על tax
        CHECK(gov.get_last_action() == "tax");

        game.set_turn_to(&gov2); // עכשיו תורו של אריאל

        gov2.tax(); // אריאל מבצע tax

        CHECK(game.is_awaiting_tax_block()); // התחלת מצב חסימת tax מחדש
        CHECK(game.get_tax_target() == &gov2);
        CHECK(game.get_tax_source() == &gov);

        // גוברנור הראשון חוסם את האריאל
        CHECK_NOTHROW(dynamic_cast<Governor*>(&gov)->block_tax(gov2));
        CHECK_FALSE(game.is_awaiting_tax_block()); // חסימה הצליחה, אז אין חסימה פעילה יותר
        CHECK(gov2.coins() == 0); // מטבעות הוחזרו
        CHECK(gov2.get_last_action().empty()); // הפעולה בוטלה

        SUBCASE("Tax Not Your Turn") {
            game.set_turn_to(&gov2); // קובע את תורו של אריאל
            CHECK_THROWS_AS(gov.tax(), std::runtime_error); // גוברנור הראשון לא בתור – לא יכול לפעול
        }

        gov2.deactivate(); // מושבת לצורך בדיקות עתידיות
    }
}

TEST_CASE("בדיקת חסימת Tax - Successful execution") {
    Game game;

    Governor gov(game, "Gili");
    Spy spy(game, "Shai");
    Governor gov2(game, "Ariel");
    Baron baron(game, "Tamar");

    game.add_player(&gov);
    game.add_player(&spy);
    game.add_player(&gov2);
    game.add_player(&baron);

    SUBCASE("Player::tax - Successful execution") {
        game.set_turn_to(&spy); // תורו של המרגל
        spy.add_coins(5); // מוודאים שפחות מ־10 מטבעות כדי לא להכריח coup
        int initial_coins = spy.coins();
        spy.tax(); // מבצע tax (תוספת של 2 מטבעות)

        CHECK(spy.coins() == initial_coins + 2);
        CHECK(game.is_awaiting_tax_block()); // שלב החסימה התחיל
        CHECK(game.get_tax_target() == &spy);
        CHECK(game.get_tax_source() == &gov);
        CHECK(game.turn() == "Gili"); // כעת תורו של גוברנור לחסום

        std::cout << "[TEST DEBUG] Before skip by Gili, turn is: " << game.turn() << "\n";
        gov.skip_tax_block(); // גוברנור לא חוסם
        std::cout << "[TEST DEBUG] After skip by Gili, turn is: " << game.turn() << "\n";

        CHECK(game.turn() == "Ariel"); // תורו של הגוברנור הבא

        std::cout << "[TEST DEBUG] Before skip by Ariel, turn is: " << game.turn() << "\n";
        gov2.skip_tax_block(); // גם הגוברנור השני לא חוסם
        std::cout << "[TEST DEBUG] After skip by Ariel, turn is: " << game.turn() << "\n";

        CHECK_FALSE(game.is_awaiting_tax_block()); // שלב החסימה הסתיים
        CHECK(game.turn() == "Ariel"); // התור עובר לאריאל – הבא בתור אחרי המרגל
    }
}

