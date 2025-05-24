# Coup Game - OOP Design in C++
### Mail - ariel.yaacobi@msmail.ariel.ac.il

## Overview
This project implements an object-oriented simulation of the board game **Coup**, designed for 2–6 players. It showcases inheritance, polymorphism (both regular and virtual), state machines, and interaction between multiple roles with unique abilities. It includes:

- Multiple role classes inheriting from a shared `Player` base.
- A central `Game` controller that manages turn flow and player interactions.
- A graphical user interface (GUI) built with SFML.
- A full suite of unit tests using the `doctest` framework.
- A complete Makefile for building, testing, and memory debugging.

## File Structure
```
CPP_EX3_25/
├── Game.hpp / Game.cpp         # Core game logic and turn management
├── Player.hpp / Player.cpp     # Base class for all roles
├── <Role>.hpp / <Role>.cpp     # Specific roles: Governor, Spy, Baron, etc.
├── Demo.cpp                    # Console-based demo game flow
├── main.cpp                    # Optional additional demo entry
├── tests.cpp                   # Doctest-based unit tests
├── gui/                        # GUI implementation using SFML
│   ├── GameRenderer.cpp/.hpp
│   ├── ActionLog.cpp/.hpp
│   ├── GuiScreens.cpp
│   └── ActionButton.hpp
├── Makefile                    # Build, test, and memory check commands
└── README.md                   # Project documentation (this file)
```

## Class Design

### `Game`
- Tracks the list of players, current turn index, and overall game state.
- Handles all role-neutral actions and control flow: `turn()`, `next_turn()`, `winner()`.

### `Player` (Base Class)
- Abstract class from which all roles inherit.
- Provides basic functionality: `gather()`, `tax()`, `coup()`, `bribe()`, etc.
- Manages player state (active/sanctioned/couped) and coin balance.

### Derived Roles
Each role overrides or adds functionality:

- **Governor** – `tax()` gives 3 coins instead of 2; can `undo()` or `block_tax()` for others.
- **Spy** – Can `spy_on()` other players and `block_arrest()` against themselves or others.
- **Baron** – Can `invest()` 3 coins for 6; gets 1 coin refund if sanctioned.
- **General** – Can `block_coup()` (if has 5+ coins); refunds coin when arrested.
- **Judge** – Can `block_bribe()`; imposes extra penalty on attacker when sanctioned.
- **Merchant** – Gains bonus coin at start of turn if holding ≥3 coins; loses 2 coins to pot if arrested.

## Features
- Full turn-based logic with support for interrupts (e.g., bribe/coup blocking).
- Exception-safe: throws on illegal actions like arresting the same target twice, insufficient coins, or acting out of turn.
- Turn preservation during interactive blocks (e.g., judge, general).
- GUI version using **SFML** with player displays, logs, and action buttons.
- Comprehensive unit tests and memory safety via `valgrind`.

## Usage

### Build and Run (Console)
```
make
./coup_game
```

### Build and Run (Graphical)
```
make run-gui
```

### Run Tests
```
make test
./test_exec
```

### Memory Check (Valgrind)
```
make valgrind-test
```

### Clean Project
```
make clean
```

## Notes
- The GUI supports one shared screen with turn-dependent menus.
- If a player has 10 coins at the start of their turn, they must perform a coup.
- Bribe, tax, and coup can be blocked by specific roles **outside** the player's regular turn.
- Arrest cannot be performed twice in a row on the same target.
- Game ends automatically when only one active player remains.

## Author & Date
- Developed by: Ariel Ya'acobi
- Assignment: Object-Oriented Programming - Coup Game
- Date: May 2025