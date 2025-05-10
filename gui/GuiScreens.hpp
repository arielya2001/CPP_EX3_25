#pragma once
#include <SFML/Graphics.hpp>
#include "../Game.hpp"
#include "../Player.hpp"
#include <string>
#include <vector>
#include "GameRenderer.hpp"
namespace coup {
    std::string popupSelectTarget(sf::RenderWindow& parentWindow, const std::vector<Player*>& players, sf::Font& font);
    bool popupYesNo(sf::RenderWindow& parentWindow, sf::Font& font, const std::string& question);
    bool runOpeningScreen();
    bool runAddPlayersScreen(Game& game);
    bool runAssignRolesScreen(Game& game);
    void runGameLoopScreen(Game& game);
}
