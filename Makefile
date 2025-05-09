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

# קבצי מקור לגרפיקה
GUI_SRC = \
    gui/main_gui.cpp \
    gui/GameRenderer.cpp \
    gui/ActionLog.cpp

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

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# גרסה גרפית – בלי Demo.cpp
$(GUI_TARGET): $(CORE_SRC) $(GUI_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(GUI_TARGET) $(CORE_SRC) $(GUI_SRC) -lsfml-graphics -lsfml-window -lsfml-system

# טסטים עם Doctest – ללא Demo.cpp!
test: $(CORE_SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(CORE_SRC) $(TEST_SRC)

run-test: test
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

run-gui: $(GUI_TARGET)
	./$(GUI_TARGET)

clean:
	rm -f $(TARGET) $(GUI_TARGET) $(TEST_TARGET)
