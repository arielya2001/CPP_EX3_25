# Coup Game Project
# Mail - ariel.yaacobi@msmail.ariel.ac.il

## Overview
This project implements the board game *Coup* in C++ using the SFML library for graphical user interface (GUI) rendering. The game supports 2–6 players, each assigned a unique role (e.g., Governor, Spy, Baron, General, Judge, Merchant) with specific abilities. Players perform actions like gathering coins, taxing, bribing, or staging a coup to eliminate opponents, with the last player standing declared the winner. The project includes:
- A core game logic implementation (`Game` and `Player` classes with derived role-specific classes).
- A GUI for interactive gameplay (`GameRenderer`, `ActionButton`, `ActionLog`, and `GuiScreens`).
- Comprehensive class hierarchy for managing game state and user interactions.
- A main program orchestrating the game flow through multiple screens (opening, player addition, role assignment, and game loop).

## File Structure
- `Game.hpp`, `Game.cpp`: Defines and implements the game logic, managing players, turns, and game state.
- `Player.hpp`, `Player.cpp`: Base class for players, handling common actions and state.
- `Baron.hpp`, `Baron.cpp`, `Governor.hpp`, `Governor.cpp`, `Spy.hpp`, `Spy.cpp`, `General.hpp`, `General.cpp`, `Judge.hpp`, `Judge.cpp`, `Merchant.hpp`, `Merchant.cpp`: Role-specific player classes with unique abilities.
- `ActionButton.hpp`, `ActionButton.cpp`: Implements interactive buttons for player actions in the GUI.
- `ActionLog.hpp`, `ActionLog.cpp`: Manages and displays a log of game actions.
- `GameRenderer.hpp`, `GameRenderer.cpp`: Renders the game state, including player info, buttons, and action log.
- `GuiScreens.hpp`, `GuiScreens.cpp`: Implements GUI screens for opening, player addition, role assignment, and game loop.
- `main.cpp`: Orchestrates the game flow by running the sequence of GUI screens.

## Class Design

### Game
- Manages the game state, including the player list, current turn, and special states (e.g., awaiting bribe or tax blocks).
- Tracks game progress and determines the winner when only one player remains active.
- Provides methods for adding players, advancing turns, and handling special actions like coup blocking.

### Player (and Derived Classes)
- Base class `Player` handles common attributes (name, coins, role, status) and actions (e.g., gather, tax, coup).
- Derived classes (`Baron`, `Governor`, `Spy`, `General`, `Judge`, `Merchant`) implement role-specific actions, such as investing (Baron), blocking taxes (Governor), or spying on coins (Spy).
- Ensures action validation (e.g., sufficient coins, correct turn) with exception handling.

### GameRenderer
- Renders the game state using SFML, displaying player information, current turn, action buttons, and an action log.
- Manages dynamic button visibility and enabling based on game state and player role.
- Uses `ActionButton` for interactive buttons and `ActionLog` for displaying game events.

### ActionButton
- Represents a clickable button with a label, position, and callback function for actions like "Gather" or "Block Tax."
- Supports visibility and enabling toggles for context-sensitive interactions.

### ActionLog
- Maintains a deque of recent game messages, displaying up to 10 lines of text with SFML.
- Updates dynamically as actions occur, ensuring smooth rendering of game events.

### GuiScreens
- Implements multiple GUI screens: opening screen, player addition, role assignment, and main game loop.
- Uses popups for role selection, target selection, and yes/no prompts, enhancing user interaction.

## Features
- **Dynamic Game Logic**: Supports 2–6 players with role-specific abilities and complex interactions (e.g., sanctions, bribe blocking).
- **Interactive GUI**: Built with SFML, featuring clickable buttons, action logs, and popups for intuitive gameplay.
- **Robust Error Handling**: Validates actions with exceptions for invalid turns, insufficient coins, or inactive players.
- **Modular Design**: Separates game logic, rendering, and GUI screens for maintainability and extensibility.
- **Customizable Roles**: Each role has unique actions, implemented via inheritance for flexibility.

## Usage
Build and run the project with:
```
make
./coup
```

Ensure SFML is installed and configured properly. The game requires the following resources:
- `resources/font.ttf`: Font file for text rendering.
- `resources/opening_screen.png`: Background image for GUI screens.

The game flow:
1. Opening screen: Click "Let the Games Begin" to start.
2. Add players: Enter 2–6 player names and click "Gather the Players."
3. Assign roles: Select a role for each player via popups.
4. Game loop: Interact via buttons to perform actions until a winner is declared.

## Notes
- The game requires SFML for rendering and assumes resource files are in the `resources/` directory.
- Role selection in `runAssignRolesScreen` is manual via popups for testing; a random assignment option is commented out.
- Players with 10 or more coins must perform a coup, enforced by button state logic.
- The GUI is designed for a 1280x720 resolution; other resolutions may require adjustments.
- No explicit memory leak checks (e.g., via `valgrind`) are included in the provided Makefile, but SFML and standard C++ practices ensure proper resource management.

## Author & Date
- Developed by: Ariel Ya'acobi
- Assignment: Coup Game Project
- Date: April 2025