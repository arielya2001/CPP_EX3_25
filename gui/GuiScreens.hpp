/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once
#include <SFML/Graphics.hpp>
#include "../Game.hpp"
#include "../Player.hpp"
#include <string>
#include <vector>
namespace coup {
    // Displays a popup window to select a target player and returns the chosen player's name
    std::string popupSelectTarget(sf::RenderWindow& parentWindow, const std::vector<Player*>& players, sf::Font& font);

    // Displays a yes/no popup with a given question and returns true for "Yes" or false for "No"
    bool popupYesNo(sf::RenderWindow& parentWindow, sf::Font& font, const std::string& question);

    // Runs the opening screen with a start button, returning true if the game should start
    bool runOpeningScreen();

    // Runs the screen for adding players, allowing name input and returning true if players are added successfully
    bool runAddPlayersScreen(Game& game);

    // Runs the screen for assigning roles to players, allowing role selection and returning true on completion
    bool runAssignRolesScreen(Game& game);

    // Runs the main game loop, handling user interactions and displaying the game state
    void runGameLoopScreen(Game& game);
}