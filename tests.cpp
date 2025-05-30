/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.hpp"
#include "Player.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Governor.hpp"
#include <stdexcept>

using namespace coup;
using namespace std;

TEST_CASE("בדיקת שגיאות עבור פעולות שחקן") {
    Game game;

    Governor gov(game, "Gili");
    Spy spy(game, "Shai");
    Baron baron(game, "Tamar");
    Merchant merch(game, "Noam");
    General general(game, "Dana");
    Judge judge(game, "Lior");

    game.add_player(&gov);
    game.add_player(&spy);
    game.add_player(&baron);
    game.add_player(&merch);
    game.add_player(&general);
    game.add_player(&judge);

    SUBCASE("Player::gather - Successful execution") {
        game.set_turn_to(&gov);
        gov.add_coins(5); // Ensure less than 10 coins
        int initial_coins = gov.coins();
        gov.gather();
        CHECK(gov.coins() == initial_coins + 1); // Adds 1 coin
        CHECK(game.turn() == "Shai"); // Advances to next player
    }
    SUBCASE("Governor::tax - Successful execution") {
        game.set_turn_to(&gov);
        gov.add_coins(5); // Ensure less than 10 coins
        int initial_coins = gov.coins();
        gov.tax();
        CHECK(gov.coins() == initial_coins + 3); // Adds 3 coins
        CHECK_FALSE(game.is_awaiting_tax_block()); // No block phase (no other Governors)
        CHECK(game.get_tax_target() == nullptr); // No target
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Player::bribe - Successful execution") {
        game.set_turn_to(&gov);
        gov.add_coins(4); // Enough for bribe
        int initial_coins = gov.coins();
        gov.bribe();
        CHECK(gov.coins() == initial_coins - 4); // Deducts 4 coins
        CHECK(gov.has_bonus_turn()); // Grants bonus turn
        CHECK(game.is_awaiting_bribe_block()); // Sets bribe block state
        CHECK(game.turn() == "Lior"); // Judge’s turn to block
        game.advance_bribe_block_queue(); // Advance to briber
        CHECK(game.turn() == "Gili"); // Returns to Gili
    }

    SUBCASE("Player::arrest - Successful execution on General") {
        game.set_turn_to(&gov);
        general.add_coins(2); // Ensure General has coins
        int gov_initial_coins = gov.coins();
        int general_initial_coins = general.coins();
        gov.arrest(general);
        CHECK(gov.coins() == gov_initial_coins); // No net coin change
        CHECK(general.coins() == general_initial_coins); // General refunds 1 coin
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Player::arrest - Successful execution on Merchant") {
        game.set_turn_to(&gov);
        merch.add_coins(2); // Ensure Merchant has coins
        int gov_initial_coins = gov.coins();
        int merch_initial_coins = merch.coins();
        gov.arrest(merch);
        CHECK(gov.coins() == gov_initial_coins); // No coin gain
        CHECK(merch.coins() == merch_initial_coins - 2); // Merchant loses 2 coins
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Player::sanction - Successful execution on Baron") {
        game.set_turn_to(&gov);
        gov.add_coins(3); // Enough for sanction
        baron.add_coins(2); // Ensure Baron has coins
        int gov_initial_coins = gov.coins();
        int baron_initial_coins = baron.coins();
        gov.sanction(baron);
        CHECK(gov.coins() == gov_initial_coins - 3); // Deducts 3 coins
        CHECK(baron.coins() == baron_initial_coins + 1); // Baron gains 1 coin
        CHECK(baron.is_sanctioned()); // Baron is sanctioned
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Player::sanction - Successful execution on Judge") {
        game.set_turn_to(&gov);
        gov.add_coins(5); // Enough for sanction (4) + penalty (1)
        judge.add_coins(2);
        int gov_initial_coins = gov.coins();
        int judge_initial_coins = judge.coins();
        gov.sanction(judge);
        CHECK(gov.coins() == gov_initial_coins - 4); // Deducts 4 + 1 penalty
        CHECK(judge.is_sanctioned()); // Judge is sanctioned
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Player::coup - Successful execution") {
        game.set_turn_to(&gov);
        gov.add_coins(7); // Enough for coup
        general.add_coins(5);
        int initial_coins = gov.coins();
        gov.coup(spy);
        CHECK(gov.coins() == initial_coins - 7); // Deducts 7 coins
        CHECK(game.is_awaiting_coup_block()); // Sets coup block state
        CHECK(game.get_coup_target() == &spy); // Target is Spy
        CHECK(game.turn() == "Dana"); // General’s turn to block
        game.skip_coup_block();
        CHECK(!spy.is_active()); // Spy is deactivated
        CHECK_FALSE(game.is_awaiting_coup_block());
    }

    SUBCASE("Baron::invest - Successful execution") {
        game.set_turn_to(&baron);
        baron.add_coins(3); // Enough for investment
        int initial_coins = baron.coins();
        baron.invest();
        CHECK(baron.coins() == initial_coins + 3); // Deducts 3, adds 6
        CHECK(game.turn() == "Noam"); // Advances to next player
    }

    SUBCASE("Spy::spy_on - Successful execution") {
        game.set_turn_to(&spy);
        gov.add_coins(5); // Target has coins
        int spy_initial_coins = spy.coins();
        int gov_initial_coins = gov.coins();
        int coins_seen = spy.spy_on(gov);
        CHECK(coins_seen == gov_initial_coins); // Sees correct coin count
        CHECK(spy.coins() == spy_initial_coins); // No coin change
        CHECK(gov.coins() == gov_initial_coins); // No coin change
        CHECK(game.turn() == "Shai"); // No turn advancement
    }

    SUBCASE("Governor::block_tax - Successful execution") {
        game.set_turn_to(&spy);
        spy.add_coins(5); // Ensure enough coins
        int initial_coins = spy.coins();
        spy.tax();
        game.set_turn_to(&gov); // Governor’s turn to block
        gov.block_tax(spy);
        CHECK(spy.coins() == initial_coins); // Tax coins (2) removed
        CHECK_FALSE(game.is_awaiting_tax_block()); // Clears tax block state
        CHECK(game.turn() == "Tamar"); // Advances to next player
    }

    SUBCASE("Judge::block_bribe - Successful execution") {
        game.set_turn_to(&gov);
        gov.add_coins(4); // Enough for bribe
        gov.bribe();
        int judge_initial_coins = judge.coins();
        game.set_turn_to(&judge); // Judge’s turn to block
        judge.block_bribe(gov);
        CHECK(judge.coins() == judge_initial_coins); // No coin gain
        CHECK_FALSE(game.is_awaiting_bribe_block()); // Clears bribe block state
        CHECK_FALSE(gov.has_bonus_turn()); // Bonus turn removed
        CHECK(game.turn() == "Shai"); // Advances to next player
    }

    SUBCASE("Spy::block_arrest - Successful execution") {
        game.set_turn_to(&spy);
        gov.add_coins(2); // Ensure Governor can attempt arrest
        spy.block_arrest(gov); // Block Gov’s future arrest
        CHECK(spy.is_arrest_blocked(&gov)); // Arrest is blocked
        game.set_turn_to(&gov);
        CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error); // Arrest fails
        CHECK(game.turn() == "Gili"); // Turn remains with Gov
    }

    SUBCASE("General::block_coup - Successful execution") {
        game.set_turn_to(&spy);
        spy.add_coins(7); // Enough for coup
        general.add_coins(5); // Enough to block
        spy.coup(baron);
        int general_initial_coins = general.coins();
        game.block_coup(&general);
        CHECK(general.coins() == general_initial_coins - 5); // Deducts 5 coins
        CHECK(baron.is_active()); // Baron remains active
        CHECK_FALSE(game.is_awaiting_coup_block()); // Clears coup block state
    }

    SUBCASE("Merchant::turn_start_bonus - Successful execution") {
        game.set_turn_to(&merch);
        merch.add_coins(3); // Enough for bonus
        int initial_coins = merch.coins();
        game.set_turn_to(&merch); // Triggers bonus
        CHECK(merch.coins() == initial_coins + 1); // Adds 1 coin
        CHECK(game.turn() == "Noam"); // Turn remains with Merchant
    }



    SUBCASE("Player::gather - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_WITH(gov.gather(), "Inactive player cannot play.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_WITH(gov.gather(), "Not your turn.");

        // יותר מדי מטבעות (חייב לעשות coup)
        game.set_turn_to(&gov);
        gov.add_coins(10);
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_WITH(gov.gather(), "Must perform coup with 10 coins.");
        gov.deduct_coins(10);

        // תחת סנקציה
        gov.set_sanctioned(true);
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_WITH(gov.gather(), "You are sanctioned and cannot gather.");
        gov.set_sanctioned(false);
    }

    SUBCASE("Player::tax - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Inactive player cannot act."); // תוקן
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Not your turn.");

        // יותר מדי מטבעות (חייב לעשות coup)
        game.set_turn_to(&gov);
        gov.add_coins(10);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Must perform coup with 10 coins.");
        gov.deduct_coins(10);

        // תחת סנקציה
        gov.set_sanctioned(true);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "You are under sanction and cannot tax."); // תוקן
        gov.set_sanctioned(false);
    }

    SUBCASE("Player::bribe - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.bribe(), runtime_error);
        CHECK_THROWS_WITH(gov.bribe(), "Inactive player cannot play.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.bribe(), runtime_error);
        CHECK_THROWS_WITH(gov.bribe(), "Not your turn.");

        // אין מספיק מטבעות
        game.set_turn_to(&gov);
        gov.deduct_coins(gov.coins());
        CHECK_THROWS_AS(gov.bribe(), runtime_error);
        CHECK_THROWS_WITH(gov.bribe(), "Not enough coins for bribe.");
    }

    SUBCASE("Player::arrest - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_WITH(gov.arrest(spy), "Inactive player cannot play.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK(game.turn() == "Shai");
        CHECK_THROWS_WITH(gov.arrest(spy), "Not your turn.");

        // יעד לא פעיל
        game.set_turn_to(&gov);
        spy.deactivate();
        CHECK_THROWS_WITH(gov.arrest(spy), "Target player is not active.");
        spy.set_active(true);

        // arrest ראשון - תקין
        game.set_turn_to(&gov);
        gov.add_coins(2);
        spy.add_coins(2);
        gov.arrest(spy); // arrest ראשון
        CHECK(game.turn() == "Shai");

        // arrest כפול ברצף - צריך לזרוק חריגה
        game.set_turn_to(&gov);
        CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row.");
        gov.gather();
        spy.gather();

        // arrest כפול עם bribe - גם לא תקין
        game.set_turn_to(&gov);
        gov.add_coins(4);
        gov.bribe();  // מתחיל שלב חסימת שוחד
        CHECK(game.turn() == "Lior");
        game.advance_bribe_block_queue();
        gov.arrest(spy);  // arrest ראשון בתור הבונוס
        CHECK(game.turn() == "Gili");
        CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row.");

        // בדיקת חסימת arrest – נדרשת "reset" של היעד הקודם
        game.set_turn_to(&gov);
        gov.add_coins(2);
        baron.add_coins(2);
        gov.arrest(baron);  // arrest חדש לשחקן אחר
        CHECK(game.turn() != "Gili");
        game.set_turn_to(&gov);
        spy.block_arrest(gov);
        CHECK_THROWS_WITH(gov.arrest(spy), "Arrest blocked by Spy.");
    }




    SUBCASE("Player::sanction - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);
        CHECK_THROWS_WITH(gov.sanction(spy), "Inactive player cannot play.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);
        CHECK_THROWS_WITH(gov.sanction(spy), "Not your turn.");

        // יעד לא פעיל
        game.set_turn_to(&gov);
        spy.deactivate();
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);
        CHECK_THROWS_WITH(gov.sanction(spy), "Target player is not active.");
        spy.set_active(true);

        // אין מספיק מטבעות (רגיל)
        gov.deduct_coins(gov.coins());
        CHECK_THROWS_AS(gov.sanction(spy), runtime_error);
        CHECK_THROWS_WITH(gov.sanction(spy), "Not enough coins for sanction.");

        // אין מספיק מטבעות (נגד Judge)
        gov.add_coins(3); // מספיק לסנקציה רגילה, אבל לא נגד Judge
        CHECK_THROWS_AS(gov.sanction(judge), runtime_error);
        CHECK_THROWS_WITH(gov.sanction(judge), "Not enough coins for sanction.");
    }

    SUBCASE("Player::coup - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
        CHECK_THROWS_WITH(gov.coup(spy), "Inactive player cannot play.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
        CHECK_THROWS_WITH(gov.coup(spy), "Not your turn.");

        // יעד לא פעיל
        game.set_turn_to(&gov);
        spy.deactivate();
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
        CHECK_THROWS_WITH(gov.coup(spy), "Target already out.");
        spy.set_active(true);

        // אין מספיק מטבעות
        gov.deduct_coins(gov.coins());
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
        CHECK_THROWS_WITH(gov.coup(spy), "Not enough coins to perform coup.");
    }

    SUBCASE("Player::deduct_coins - שגיאות") {
        // ניסיון להפחית יותר מטבעות ממה שיש
        gov.deduct_coins(gov.coins());
        CHECK_THROWS_AS(gov.deduct_coins(1), runtime_error);
        CHECK_THROWS_WITH(gov.deduct_coins(1), "Not enough coins.");
    }

    SUBCASE("Baron::invest - שגיאות") {
        Game game;
        Baron baron(game, "Tamar");
        Spy spy(game, "Shai");

        // בדיקת "Game has not started" עם שחקן אחד בלבד
        game.add_player(&baron);
        CHECK_THROWS_WITH(baron.invest(), "Game has not started – need at least 2 players.");

        // הוספת שחקן נוסף כדי שהמשחק יתחיל
        game.add_player(&spy);
        game.set_turn_to(&baron);

        // בדיקת "Baron is not active"
        baron.deactivate();
        CHECK_THROWS_WITH(baron.invest(), "Baron is not active.");

        // החזרת baron למצב פעיל לבדיקות הבאות
        baron.set_active(true);
        game.set_turn_to(&spy);
        CHECK_THROWS_WITH(baron.invest(), "Not your turn.");

        game.set_turn_to(&baron);
        baron.add_coins(10);
        CHECK_THROWS_WITH(baron.invest(), "Must coup with 10 coins.");

        baron.deduct_coins(baron.coins());
        CHECK_THROWS_WITH(baron.invest(), "Not enough coins to invest.");
    }
    SUBCASE("Governor::tax - שגיאות") {
        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Inactive player cannot act.");
        gov.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Not your turn.");

        // יותר מדי מטבעות (חייב לעשות coup)
        game.set_turn_to(&gov);
        gov.add_coins(10);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "Must perform coup with 10 coins.");
        gov.deduct_coins(10);

        // תחת סנקציה
        gov.set_sanctioned(true);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        CHECK_THROWS_WITH(gov.tax(), "You are under sanction and cannot tax.");
    }
    SUBCASE("Governor::block_tax - שגיאות") {
        Game game;
        Governor gov(game, "Gili");
        Spy spy(game, "Shai");

        // בדיקת "Game has not started" עם שחקן אחד בלבד
        game.add_player(&gov);
        CHECK_THROWS_WITH(gov.block_tax(spy), "Game has not started – need at least 2 players.");

        // הוספת שחקן נוסף כדי שהמשחק יתחיל
        game.add_player(&spy);

        // יעד לא ביצע tax
        spy.clear_last_action();
        CHECK_THROWS_WITH(gov.block_tax(spy), "Target didn't tax.");

        // אין מספיק מטבעות ליעד
        game.set_turn_to(&spy);
        CHECK(game.turn() == "Shai"); // בדיקת תור
        spy.add_coins(2); // וידוא שיש מספיק מטבעות ל-tax
        spy.tax(); // מגדיר last_action כ-"tax"
        spy.deduct_coins(spy.coins()); // הסרת כל המטבעות לאחר tax
        CHECK_THROWS_WITH(gov.block_tax(spy), "Not enough coins to undo tax.");
    }
    SUBCASE("Governor::skip_tax_block - שגיאות") {
        Game game;
        Governor gov(game, "Gili");
        game.add_player(&gov); // רק שחקן אחד
        CHECK_THROWS_WITH(gov.skip_tax_block(), "Game has not started – need at least 2 players.");
    }

    SUBCASE("Judge::block_bribe - שגיאות") {
        Game game;
        Judge judge(game, "Lior");
        Spy spy(game, "Shai");

        // בדיקת "Game has not started" עם שחקן אחד בלבד
        game.add_player(&judge);
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Game has not started – need at least 2 players.");

        // הוספת שחקן נוסף כדי שהמשחק יתחיל
        game.add_player(&spy);

        // שחקן לא פעיל
        judge.deactivate();
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Judge is not active.");
        judge.set_active(true);

        // יעד לא פעיל
        spy.deactivate();
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target is not active.");
        spy.set_active(true);

        // יעד לא ביצע bribe
        spy.clear_last_action();
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target did not perform bribe.");
    }
    SUBCASE("Merchant::בונוס בתחילת תור עם 3 מטבעות") {
        Game game;
        Merchant merch(game, "Mark");
        Spy spy(game, "Shai");

        game.add_player(&merch);
        game.add_player(&spy);

        // נתן למרצ'נט 3 מטבעות לפני תחילת התור שלו
        merch.add_coins(3);

        game.set_turn_to(&merch); // כאן יתבצע הבונוס אם יש 3 או יותר
        CHECK(merch.coins() == 4); // +1 בונוס

        merch.gather();
        CHECK(merch.coins() == 5); // +1 gather
    }


    SUBCASE("Spy::spy_on - שגיאות") {
        // שחקן לא פעיל
        spy.deactivate();
        CHECK_THROWS_AS(spy.spy_on(gov), runtime_error);
        CHECK_THROWS_WITH(spy.spy_on(gov), "Spy is not active.");
        spy.set_active(true);

        // יעד לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(spy.spy_on(gov), runtime_error);
        CHECK_THROWS_WITH(spy.spy_on(gov), "Target is not active.");
        gov.set_active(true);
    }
    SUBCASE("Spy::block_arrest - שגיאות") {
        Game game;
        Spy spy(game, "Shai");
        Governor gov(game, "Gili");
        game.add_player(&spy);
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Game has not started – need at least 2 players.");
        game.add_player(&gov); // started = true

        // שחקן לא פעיל
        spy.deactivate();
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Spy is not active.");
        spy.set_active(true);

        // יעד לא פעיל
        gov.deactivate();
        CHECK_THROWS_WITH(spy.block_arrest(gov), "Target is not active.");
        gov.set_active(true);

        // בדיקה שהפעולה עובדת כשאין שגיאות
        CHECK_NOTHROW(spy.block_arrest(gov));
    }
    SUBCASE("Spy::clear_expired_blocks - שגיאות") {
        Game game;
        Spy spy(game, "Shai");
        game.add_player(&spy); // רק שחקן אחד
        CHECK_THROWS_WITH(spy.clear_expired_blocks(), "Game has not started – need at least 2 players.");
    }

    SUBCASE("Game::add_player - שגיאות") {
        // הוספת שחקן כשהמשחק מלא
        Game full_game;
        std::vector<Player*> allocated;

        for (int i = 0; i < 6; ++i) {
            Player* p = new Player(full_game, "Player" + to_string(i), "");
            allocated.push_back(p);
            full_game.add_player(p);
        }
        Player* extra = new Player(full_game, "Extra", "");
        allocated.push_back(extra);
        CHECK_THROWS_WITH(full_game.add_player(extra), "Cannot add more than 6 players.");

        for (Player* p : allocated) {
            delete p;
        }
    }
    SUBCASE("Game::turn - שגיאות") {
        // משחק ללא שחקנים
        Game empty_game;
        CHECK_THROWS_AS(empty_game.turn(), runtime_error);
        CHECK_THROWS_WITH(empty_game.turn(), "No players in the game.");

        // משחק עם שחקן אחד (לא התחיל)
        Game one_player_game;
        Player p1(one_player_game, "P1", "");
        one_player_game.add_player(&p1);
        CHECK_THROWS_AS(one_player_game.turn(), runtime_error);
        CHECK_THROWS_WITH(one_player_game.turn(), "Game has not started – need at least 2 players.");

        // משחק עם שני שחקנים (התחיל)
        Game game;
        Player p2(game, "P2", "");
        game.add_player(&p1);
        game.add_player(&p2);
        CHECK_NOTHROW(game.turn()); // צריך לעבוד כי started == true
    }
    SUBCASE("Game::skip_coup_block - שגיאות") {
        // אין חסימת coup ממתינה
        CHECK_THROWS_WITH(game.skip_coup_block(), "No coup block is pending.");
    }

    SUBCASE("Game::block_coup - שגיאות") {
        // אין coup לחסום
        CHECK_THROWS_WITH(game.block_coup(&general), "No coup block is pending.");

        // לא General פעיל
        spy.add_coins(7);
        general.add_coins(5); // וידוא ש-General יכול לחסום (מפעיל awaiting_coup_block)
        game.set_turn_to(&spy);
        spy.coup(baron); // מפעיל awaiting_coup_block
        CHECK(game.is_awaiting_coup_block()); // וידוא שמצב החסימה פעיל
        game.set_turn_to(&general);
        general.deactivate();
        general.set_active(false);
        CHECK_THROWS_WITH(game.block_coup(&general), "Only an active General can block a coup.");
    }
    SUBCASE("Game::winner - שגיאות") {
        // משחק שעדיין לא נגמר
        CHECK_THROWS_AS(game.winner(), runtime_error);
        CHECK_THROWS_WITH(game.winner(), "Game is still ongoing.");
    }

    SUBCASE("Game::set_turn_to - שגיאות") {
        // שחקן לא קיים במשחק
        Player non_existent(game, "NonExistent", "");
        CHECK_THROWS_AS(game.set_turn_to(&non_existent), runtime_error);
        CHECK_THROWS_WITH(game.set_turn_to(&non_existent), "Player not found in game.");
    }

    SUBCASE("Game not started - שגיאות") {
        // משחק עם שחקן אחד בלבד
        Game game;
        Governor gov(game, "Gili");
        Spy spy(game, "Shai");
        game.add_player(&gov); // רק שחקן אחד

        // ודא שיש מספיק מטבעות כדי למנוע חריגות של "Not enough coins"
        gov.add_coins(10);

        // בדיקת פעולות של Player
        CHECK_THROWS_WITH(gov.gather(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.tax(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.bribe(), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.arrest(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.sanction(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(gov.coup(gov), "Game has not started – need at least 2 players.");
        CHECK_THROWS_WITH(spy.spy_on(gov), "Game has not started – need at least 2 players.");

        // הוספת שחקנים נוספים למשחק
        Baron baron(game, "Bar");
        General general(game, "Gen");
        Judge judge(game, "Jud");
        game.add_player(&spy);
        game.add_player(&baron);
        game.add_player(&general);
        game.add_player(&judge);

        // בדיקת פעולות ספציפיות לתפקידים עם מספיק שחקנים
        CHECK_THROWS_WITH(baron.invest(), "Not your turn.");
        CHECK_THROWS_WITH(gov.block_tax(gov), "Target didn't tax.");
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
        game.set_turn_to(&gov);

        // Gili (Governor ראשון) מבצע tax
        gov.tax();

        CHECK(game.is_awaiting_tax_block());
        CHECK(game.get_tax_target() == &gov);
        CHECK(game.get_tax_source() == &gov2);
        CHECK(game.turn() == "Ariel");

        // Ariel בוחר לדלג על חסימה
        gov2.skip_tax_block();
        CHECK_FALSE(game.is_awaiting_tax_block());
        CHECK(gov.coins() == 3);
        CHECK(gov.get_last_action() == "tax");

        game.set_turn_to(&gov2);

        // Ariel מבצע tax
        gov2.tax();

        CHECK(game.is_awaiting_tax_block());
        CHECK(game.get_tax_target() == &gov2);
        CHECK(game.get_tax_source() == &gov);
        CHECK_NOTHROW(dynamic_cast<Governor*>(&gov)->block_tax(gov2));
        CHECK_FALSE(game.is_awaiting_tax_block());
        CHECK(gov2.coins() == 0);
        CHECK(gov2.get_last_action().empty());

        SUBCASE("Tax Not Your Turn") {
            game.set_turn_to(&gov2);
            CHECK_THROWS_AS(gov.tax(), std::runtime_error);
        }

        gov2.deactivate();
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
        game.set_turn_to(&spy);
        spy.add_coins(5); // Ensure less than 10 coins
        int initial_coins = spy.coins();
        spy.tax();
        CHECK(spy.coins() == initial_coins + 2); // Adds 2 coins
        CHECK(game.is_awaiting_tax_block()); // Tax block phase
        CHECK(game.get_tax_target() == &spy); // Target is Spy
        CHECK(game.get_tax_source() == &gov); // Source is Governor (Gili)
        CHECK(game.turn() == "Gili"); // Gili’s turn to block
        std::cout << "[TEST DEBUG] Before skip by Gili, turn is: " << game.turn() << "\n";
        gov.skip_tax_block(); // Skip Gili’s block
        std::cout << "[TEST DEBUG] After skip by Gili, turn is: " << game.turn() << "\n";
        CHECK(game.turn() == "Ariel"); // Ariel’s turn to block
        std::cout << "[TEST DEBUG] Before skip by Ariel, turn is: " << game.turn() << "\n";
        gov2.skip_tax_block(); // Skip Ariel’s block
        std::cout << "[TEST DEBUG] After skip by Ariel, turn is: " << game.turn() << "\n";
        CHECK_FALSE(game.is_awaiting_tax_block()); // Block phase ends
        CHECK(game.turn() == "Ariel"); // Advances to next player after Spy
    }
}
