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
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
    CHECK_THROWS_WITH(gov.arrest(spy), "Inactive player cannot play.");
    gov.set_active(true);

    // לא תורו של השחקן
    game.set_turn_to(&spy);
    CHECK(game.turn() == "Shai");
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
    CHECK_THROWS_WITH(gov.arrest(spy), "Not your turn.");

    // יעד לא פעיל
    game.set_turn_to(&gov);
    CHECK(game.turn() == "Gili");
    spy.deactivate();
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
    CHECK_THROWS_WITH(gov.arrest(spy), "Target player is not active.");
    spy.set_active(true);

    // מעצר אותו שחקן פעמיים ברצף (תור רגיל)
    game.set_turn_to(&gov);
    CHECK(game.turn() == "Gili");
    gov.add_coins(2);
    spy.add_coins(2);
    gov.arrest(spy); // arrest ראשון
    CHECK(game.turn() == "Shai");
    game.set_turn_to(&gov); // מחזירים תור לגילי
    CHECK(game.turn() == "Gili");
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
    CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row.");

    // מעצר אותו שחקן פעמיים עם bribe (תור נוסף)
    game.set_turn_to(&gov);
    CHECK(game.turn() == "Gili");
    gov.add_coins(4); // מספיק ל-bribe
    spy.add_coins(2);
    gov.bribe(); // מתחיל שלב חסימת שוחד
    CHECK(game.is_awaiting_bribe_block());
    game.advance_bribe_block_queue(); // מוציא את Lior ומחזיר תור ל-Gili
    CHECK(game.turn() == "Gili");
    gov.arrest(spy); // arrest ראשון בתור נוסף
    CHECK(game.turn() == "Gili"); // תור נוסף ממשיך
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
    CHECK_THROWS_WITH(gov.arrest(spy), "Cannot arrest the same player twice in a row.");

    // חסימת arrest ע"י Spy
    game.set_turn_to(&gov);
    CHECK(game.turn() == "Gili");
    gov.add_coins(2);
    spy.block_arrest(gov);
    CHECK_THROWS_AS(gov.arrest(spy), std::runtime_error);
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
        // שחקן לא פעיל
        baron.deactivate();
        CHECK_THROWS_AS(baron.invest(), std::runtime_error);
        CHECK_THROWS_WITH(baron.invest(), "Baron is not active.");
        baron.set_active(true);

        // לא תורו של השחקן
        game.set_turn_to(&spy);
        CHECK(game.turn() == "Shai"); // בדיקת תור
        CHECK_THROWS_AS(baron.invest(), std::runtime_error);
        CHECK_THROWS_WITH(baron.invest(), "Not your turn.");

        // אין מספיק מטבעות
        game.set_turn_to(&baron);
        CHECK(game.turn() == "Tamar"); // בדיקת תור
        baron.deduct_coins(baron.coins());
        CHECK_THROWS_AS(baron.invest(), std::runtime_error);
        CHECK_THROWS_WITH(baron.invest(), "Not enough coins to invest.");

        // יותר מדי מטבעות (חייב לעשות coup)
        game.set_turn_to(&baron);
        CHECK(game.turn() == "Tamar"); // בדיקת תור
        baron.add_coins(10);
        CHECK_THROWS_AS(baron.invest(), std::runtime_error);
        CHECK_THROWS_WITH(baron.invest(), "Must coup with 10 coins.");
    }

    SUBCASE("General::protect_from_coup - שגיאות") {
        // שחקן לא פעיל
        general.deactivate();
        CHECK_THROWS_AS(general.protect_from_coup(spy), runtime_error);
        CHECK_THROWS_WITH(general.protect_from_coup(spy), "General is not active.");
        general.set_active(true);

        // יעד לא פעיל
        spy.deactivate();
        CHECK_THROWS_AS(general.protect_from_coup(spy), runtime_error);
        CHECK_THROWS_WITH(general.protect_from_coup(spy), "Target is not active.");
        spy.set_active(true);

        // אין מספיק מטבעות
        general.deduct_coins(general.coins());
        CHECK_THROWS_AS(general.protect_from_coup(spy), runtime_error);
        CHECK_THROWS_WITH(general.protect_from_coup(spy), "Not enough coins to protect.");
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
        // יעד לא ביצע tax
        spy.clear_last_action();
        CHECK_THROWS_AS(gov.block_tax(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.block_tax(spy), "Target didn't tax.");

        // אין מספיק מטבעות ליעד
        game.set_turn_to(&spy);
        CHECK(game.turn() == "Shai"); // בדיקת תור
        spy.add_coins(2); // וידוא שיש מספיק מטבעות ל-tax
        spy.tax(); // מגדיר last_action כ-"tax"
        spy.deduct_coins(spy.coins()); // הסרת כל המטבעות לאחר tax
        CHECK_THROWS_AS(gov.block_tax(spy), std::runtime_error);
        CHECK_THROWS_WITH(gov.block_tax(spy), "Not enough coins to undo tax.");
    }

    SUBCASE("Judge::block_bribe - שגיאות") {
        // שחקן לא פעיל
        judge.deactivate();
        CHECK_THROWS_AS(judge.block_bribe(spy), runtime_error);
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Judge is not active.");
        judge.set_active(true);

        // יעד לא פעיל
        spy.deactivate();
        CHECK_THROWS_AS(judge.block_bribe(spy), runtime_error);
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target is not active.");
        spy.set_active(true);

        // יעד לא ביצע bribe
        spy.clear_last_action();
        CHECK_THROWS_AS(judge.block_bribe(spy), runtime_error);
        CHECK_THROWS_WITH(judge.block_bribe(spy), "Target did not perform bribe.");
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
}