CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# קבצי מקור רגילים
SRC = \
    Demo.cpp \
    Game.cpp \
    Player.cpp \
    Governor.cpp \
    Spy.cpp \
    Baron.cpp \
    General.cpp \
    Judge.cpp \
    Merchant.cpp

# גרעין המשחק בלי Demo
CORE_SRC = \
    Game.cpp \
    Player.cpp \
    Governor.cpp \
    Spy.cpp \
    Baron.cpp \
    General.cpp \
    Judge.cpp \
    Merchant.cpp

# קבצי מקור לגרפיקה (כולל מסכי פתיחה, הזנת שמות וכו')
GUI_SRC = \
    gui/main_gui.cpp \
    gui/GameRenderer.cpp \
    gui/ActionLog.cpp \
    gui/GuiScreens.cpp

# כל ההאדרים
HEADERS = \
    Game.hpp \
    Player.hpp \
    Governor.hpp \
    Spy.hpp \
    Baron.hpp \
    General.hpp \
    Judge.hpp \
    Merchant.hpp \
    gui/GameRenderer.hpp \
    gui/ActionLog.hpp \
    gui/ActionButton.hpp

# קובץ טסטים
TEST_SRC = tests.cpp
TEST_TARGET = test_exec

# קבצי הפלט
TARGET = coup_game
GUI_TARGET = gui_game

## Default target – builds the regular command-line game
all: $(TARGET)

## Builds the command-line version of the game (includes Demo.cpp)
$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

## Builds the GUI version of the game using SFML (no Demo.cpp)
$(GUI_TARGET): $(CORE_SRC) $(GUI_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(GUI_TARGET) $(CORE_SRC) $(GUI_SRC) -lsfml-graphics -lsfml-window -lsfml-system

## Builds the unit tests using doctest (no Demo.cpp)
test: $(CORE_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(CORE_SRC) $(TEST_SRC)

## Builds the main.cpp file for running sample games
main: main.cpp $(CORE_SRC)
	$(CXX) $(CXXFLAGS) -o main main.cpp $(CORE_SRC)

## Runs the test binary
run-test: test
	./$(TEST_TARGET)

## Runs the command-line version of the game
run: $(TARGET)
	./$(TARGET)

## Runs the GUI version of the game
run-gui: $(GUI_TARGET)
	./$(GUI_TARGET)

## Runs valgrind to check for memory leaks in the test binary
valgrind-test: test
	valgrind --leak-check=full --track-origins=yes ./$(TEST_TARGET)

## Runs valgrind to check for memory leaks in main.cpp binary
valgrind-main: main
	valgrind --leak-check=full --track-origins=yes ./main

## Cleans all build outputs
clean:
	rm -f $(TARGET) $(GUI_TARGET) $(TEST_TARGET) main
