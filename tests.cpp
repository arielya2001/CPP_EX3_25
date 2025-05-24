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

    SUBCASE("Player::undo - שגיאות") {
        // פעולה לא חוקית עבור שחקן רגיל
        CHECK_THROWS_AS(spy.undo(gov), runtime_error);
        CHECK_THROWS_WITH(spy.undo(gov), "This role cannot perform undo.");
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

    SUBCASE("Governor::undo - שגיאות") {
        Game game;
        Governor gov(game, "Gili");
        Spy spy(game, "Shai");

        // בדיקת "Game has not started" עם שחקן אחד בלבד
        game.add_player(&gov);
        CHECK_THROWS_WITH(gov.undo(spy), "Game has not started – need at least 2 players.");

        // הוספת שחקן נוסף כדי שהמשחק יתחיל
        game.add_player(&spy);

        // שחקן לא פעיל
        gov.deactivate();
        CHECK_THROWS_AS(gov.undo(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.undo(spy), "Governor is not active.");
        gov.set_active(true);

        // יעד לא פעיל
        spy.deactivate();
        CHECK_THROWS_AS(gov.undo(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.undo(spy), "Target is not active.");
        spy.set_active(true);

        // הפעולה האחרונה של היעד אינה tax
        spy.clear_last_action();
        CHECK_THROWS_AS(gov.undo(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.undo(spy), "Cannot undo: last action was not tax.");

        // אין מספיק מטבעות ליעד
        game.set_turn_to(&spy);
        CHECK(game.turn() == "Shai"); // בדיקת תור
        spy.add_coins(2); // וידוא שיש מספיק מטבעות ל-tax
        spy.tax(); // מגדיר last_action כ-"tax"
        spy.deduct_coins(spy.coins()); // הסרת כל המטבעות לאחר tax
        CHECK_THROWS_AS(gov.undo(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.undo(spy), "Target doesn't have enough coins to undo.");
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
        for (int i = 0; i < 6; ++i) {
            full_game.add_player(new Player(full_game, "Player" + to_string(i), ""));
        }
        CHECK_THROWS_AS(full_game.add_player(new Player(full_game, "Extra", "")), runtime_error);
        CHECK_THROWS_WITH(full_game.add_player(new Player(full_game, "Extra", "")), "Cannot add more than 6 players.");
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
        CHECK_THROWS_WITH(game.block_coup(&general), "No coup to block.");

        // לא General פעיל
        spy.add_coins(7);
        general.add_coins(5); // וידוא ש-General יכול לחסום (מפעיל awaiting_coup_block)
        game.set_turn_to(&spy);
        spy.coup(baron); // מפעיל awaiting_coup_block
        CHECK(game.is_awaiting_coup_block()); // וידוא שמצב החסימה פעיל
        game.set_turn_to(&general);
        general.deactivate();
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
        CHECK_THROWS_WITH(gov.undo(gov), "Cannot undo: last action was not tax.");
        CHECK_THROWS_WITH(gov.block_tax(gov), "Target didn't tax.");
        CHECK_THROWS_WITH(judge.block_bribe(gov), "Target did not perform bribe.");
    }
}