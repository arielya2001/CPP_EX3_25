/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "../Game.hpp"                // Includes the core Game logic
#include "GuiScreens.hpp"            // Includes all GUI screen functions
using namespace coup;               // Use the coup namespace to simplify class references

// Entry point: Initializes the game and runs the sequence of GUI screens
int main() {
    Game game;                      // Create a new Game instance

    if (!runOpeningScreen()) return 0;        // Show opening screen; exit if user closes
    if (!runAddPlayersScreen(game)) return 0; // Show screen to input player names; exit if closed
    if (!runAssignRolesScreen(game)) return 0; // Show role selection screen; exit if closed

    runGameLoopScreen(game);                 // Run the main game loop (GUI-based)
    return 0;                                // Exit program cleanly
}
