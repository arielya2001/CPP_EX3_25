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

    SUBCASE("שחקן לא פעיל לא יכול לפעול") {
        game.set_turn_to(&gov);
        gov.set_active(false);
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
        gov.set_active(true);
    }

    SUBCASE("שחקן שלא בתורו לא יכול לפעול") {
        game.set_turn_to(&gov);
        CHECK(game.turn() == "Gili");
        CHECK_THROWS_AS(spy.gather(), runtime_error);
        CHECK_THROWS_AS(spy.tax(), runtime_error);
    }

    SUBCASE("שחקן עם 10 מטבעות חייב לבצע coup") {
        game.set_turn_to(&gov);
        for (int i = 0; i < 4; i++) {
            gov.tax();  // +12
            game.set_turn_to(&gov); // נחזיר את התור לגילי כדי שיבצע שוב
        }
        CHECK_THROWS_AS(gov.gather(), runtime_error);
        CHECK_THROWS_AS(gov.tax(), runtime_error);
    }

    SUBCASE("שחקן תחת סנקציה לא יכול לעשות gather/tax") {
        game.set_turn_to(&gov);
        gov.add_coins(3);
        gov.sanction(spy);
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(spy.gather(), runtime_error);
        CHECK_THROWS_AS(spy.tax(), runtime_error);
    }

    SUBCASE("שוחד דורש 4 מטבעות") {
        game.set_turn_to(&spy);
        spy.add_coins(3);
        CHECK_THROWS_AS(spy.bribe(gov), runtime_error);
    }

    SUBCASE("אי אפשר לעצור שחקן פעמיים ברציפות") {
        spy.add_coins(6);
        baron.add_coins(2);
        game.set_turn_to(&spy);
        spy.arrest(baron);
        game.set_turn_to(&spy);
        CHECK_THROWS_AS(spy.arrest(baron), runtime_error);
    }


    SUBCASE("Spy חוסם מעצר") {
        spy.block_arrest(gov);
        game.set_turn_to(&gov);
        CHECK_THROWS_AS(gov.arrest(spy), runtime_error);
    }

    SUBCASE("Sanction על שופט דורש מטבע נוסף") {
        game.set_turn_to(&gov);
        gov.add_coins(3);
        CHECK_THROWS_AS(gov.sanction(judge), runtime_error); // צריך 4
        gov.add_coins(1);
        CHECK_NOTHROW(gov.sanction(judge));
    }

    SUBCASE("Coup דורש לפחות 7 מטבעות") {
        game.set_turn_to(&gov);
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
        gov.add_coins(7);
        CHECK_NOTHROW(gov.coup(spy));
    }

    SUBCASE("Coup נכשל אם השחקן כבר הודח") {
        game.set_turn_to(&gov);
        gov.add_coins(7);
        spy.deactivate();
        CHECK_THROWS_AS(gov.coup(spy), runtime_error);
    }
}
