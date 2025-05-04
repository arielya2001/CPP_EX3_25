CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

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

HEADERS = \
    Game.hpp \
    Player.hpp \
    Governor.hpp \
    Spy.hpp \
    Baron.hpp \
    General.hpp \
    Judge.hpp \
    Merchant.hpp

TARGET = coup_game

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
