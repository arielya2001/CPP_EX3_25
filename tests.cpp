#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>

TEST_CASE("Sanction blocks gather and tax for one turn only") {
    coup::Game game;
    coup::Player p1(game, "Player1", "Governor");
    coup::Player p2(game, "Player2", "Judge");

    // תור של p1: מבצע sanction על p2
    p1.add_coins(3); // כדי שיהיה לו מספיק מטבעות
    game.set_turn_to(&p1); // מבטיח ש-p1 בתור
    p1.sanction(p2);

    // ודא ש-p2 חסום
    CHECK(p2.is_under_sanction() == true);
    CHECK(p2.is_gather_blocked() == true);

    // תור של p2: החסימות אמורות להימשך
    game.set_turn_to(&p2); // מבטיח ש-p2 בתור
    CHECK_THROWS_AS(p2.gather(), std::runtime_error);
    CHECK_THROWS_AS(p2.tax(), std::runtime_error);

    // תור חוזר ל-p2 לאחר סיבוב: החסימות אמורות להתבטל
    game.next_turn(); // חזרה ל-p1
    game.next_turn(); // חזרה ל-p2, שיאפס את החסימות
    game.set_turn_to(&p2); // מבטיח ש-p2 בתור
    CHECK(p2.is_under_sanction() == false);
    CHECK(p2.is_gather_blocked() == false);
    game.set_turn_to(&p2); // מבטיח ש-p2 בתור לפני gather
    CHECK_NOTHROW(p2.gather());
    game.set_turn_to(&p2); // מבטיח ש-p2 בתור לפני tax
    CHECK_NOTHROW(p2.tax());
}
