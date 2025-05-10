#include <SFML/Graphics.hpp>
#include "../Game.hpp"
#include "GuiScreens.hpp"  // ודא שהקובץ הזה כולל את ההצהרות לפונקציות שלך
#include <iostream>
using namespace coup;

int main() {
    Game game;

    if (!runOpeningScreen()) return 0;
    if (!runAddPlayersScreen(game)) return 0;
    if (!runAssignRolesScreen(game)) return 0;

    runGameLoopScreen(game);
    return 0;
}
