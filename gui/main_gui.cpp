/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "../Game.hpp"
#include "GuiScreens.hpp"
using namespace coup;

// Entry point: Initializes the game and runs the sequence of GUI screens
int main() {
    Game game;

    if (!runOpeningScreen()) return 0;
    if (!runAddPlayersScreen(game)) return 0;
    if (!runAssignRolesScreen(game)) return 0;

    runGameLoopScreen(game);
    return 0;
}