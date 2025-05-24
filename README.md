# Coup Game Project
# Mail - ariel.yaacobi@msmail.ariel.ac.il

## Overview
This project implements the board game *Coup* in C++ with two interfaces: a command-line version and a graphical user interface (GUI) using the SFML library. The game supports 2–6 players, each assigned a unique role (e.g., Governor, Spy, Baron, General, Judge, Merchant) with specific abilities. Players perform actions like gathering coins, taxing, bribing, or staging a coup to eliminate opponents, with the last player standing declared the winner. The project includes:
- Core game logic implementation (`Game` and `Player` classes with derived role-specific classes).
- A GUI for interactive gameplay (`GameRenderer`, `ActionButton`, `ActionLog`, and `GuiScreens`).
- A command-line interface for text-based gameplay (`Demo.cpp`).
- A test suite for validating game logic (`tests.cpp`).
- Comprehensive class hierarchy for managing game state and user interactions.
- Main programs for orchestrating game flow (`main.cpp` for command-line, `main_gui.cpp` for GUI).

## File Structure
- `Game.hpp`, `Game.cpp`: Defines and implements the game logic, managing players, turns, and game state.
- `Player.hpp`, `Player.cpp`: Base class for players, handling common actions and state.
- `Baron.hpp`, `Baron.cpp`, `Governor.hpp`, `Governor.cpp`, `Spy.hpp`, `Spy.cpp`, `General.hpp`, `General.cpp`, `Judge.hpp`, `Judge.cpp`, `Merchant.hpp`, `Merchant.cpp`: Role-specific player classes with unique abilities.
- `ActionButton.hpp`, `ActionButton.cpp`: Implements interactive buttons for player actions in the GUI.
- `ActionLog.hpp`, `ActionLog.cpp`: Manages and displays a log of game actions in the GUI.
- `GameRenderer.hpp`, `GameRenderer.cpp`: Renders the game state for the GUI, including player info, buttons, and action log.
- `GuiScreens.hpp`, `GuiScreens.cpp`: Implements GUI screens for opening, player addition, role assignment, and game loop.
- `main_gui.cpp`: Orchestrates the GUI game flow by running the sequence of GUI screens.
- `main.cpp`: Orchestrates the command-line game flow by running sample games.
- `tests.cpp`: Contains unit tests for the game logic using the doctest framework.
- `Demo.cpp`: Implements a command-line interface for demonstrating game functionality.
- `Makefile`: Provides build and run commands for the command-line game, GUI game, tests, and memory checks.

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

### Demo
- Provides a command-line interface for playing the game, demonstrating core mechanics without a graphical interface.
- Used in `main.cpp` to run sample games and showcase game logic.

### Tests
- Contains unit tests for the game logic, implemented in `tests.cpp` using the doctest framework.
- Validates player actions, role-specific abilities, and game state transitions.

## Features
- **Dynamic Game Logic**: Supports 2–6 players with role-specific abilities and complex interactions (e.g., sanctions, bribe blocking).
- **Dual Interfaces**: Offers both a command-line version (via `Demo.cpp`) and an interactive GUI (via `GuiScreens.cpp`).
- **Robust Error Handling**: Validates actions with exceptions for invalid turns, insufficient coins, or inactive players.
- **Modular Design**: Separates game logic, rendering, and GUI screens for maintainability and extensibility.
- **Customizable Roles**: Each role has unique actions, implemented via inheritance for flexibility.
- **Unit Testing**: Includes a test suite (`tests.cpp`) to ensure correctness of game mechanics.
- **Memory Safety**: Designed with proper resource management, verifiable with `valgrind`.

## Usage
Ensure SFML is installed for the GUI version and the following resources are in the `resources/` directory:
- `resources/font.ttf`: Font file for text rendering.
- `resources/opening_screen.png`: Background image for GUI screens.

Build and run the project using the provided Makefile commands:

### Build and Run the Command-Line Game
```
make
./coup_game
```
- Builds the command-line version using `Demo.cpp` and core game logic.
- Runs a text-based version of the game.

### Build and Run the GUI Game
```
make run-gui
```
- Builds the GUI version using SFML (`main_gui.cpp`, `GuiScreens.cpp`, etc.).
- Runs the interactive GUI game with the following flow:
  1. Opening screen: Click "Let the Games Begin" to start.
  2. Add players: Enter 2–6 player names and click "Gather the Players."
  3. Assign roles: Select a role for each player via popups.
  4. Game loop: Interact via buttons to perform actions until a winner is declared.

### Build and Run Unit Tests
```
make run-test
```
- Builds and runs the unit tests (`tests.cpp`) using the doctest framework.
- Validates game logic and role-specific actions.

### Build and Run Sample Games (main.cpp)
```
make main
./main
```
- Builds and runs `main.cpp` with core game logic to demonstrate sample games.

### Check for Memory Leaks
```
make valgrind-test
```
- Runs the test binary (`test_exec`) under `valgrind` to check for memory leaks.

```
make valgrind-main
```
- Runs the `main.cpp` binary under `valgrind` to check for memory leaks.

### Clean Build Artifacts
```
make clean
```
- Removes all compiled binaries (`coup_game`, `gui_game`, `test_exec`, `main`).

## Notes
- The game requires SFML for the GUI version and assumes resource files are in the `resources/` directory.
- Role selection in `runAssignRolesScreen` is manual via popups for testing; a random assignment option is commented out.
- Players with 10 or more coins must perform a coup, enforced by button state logic in the GUI.
- The GUI is designed for a 1280x720 resolution; other resolutions may require adjustments.
- The command-line version (`Demo.cpp`) is independent of SFML, making it portable for non-graphical environments.
- The test suite (`tests.cpp`) focuses on core game logic and does not test GUI components.

## Author & Date
- Developed by: Ariel Ya'acobi
- Assignment: Coup Game Project
- Date: April 2025