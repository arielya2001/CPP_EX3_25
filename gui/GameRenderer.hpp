/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include "ActionButton.hpp"
#include "../Player.hpp"
#include "../Game.hpp"


namespace coup {

    class GameRenderer {
    private:
        Game& game;
        sf::Font& font;
        std::vector<sf::Text> playerTexts;
        sf::Text turnText;
        sf::Text infoText;
        std::vector<ActionButton> buttons;
        std::vector<ActionButton> targetButtons; // כפתורי בחירה ל-Coup
        std::vector<Player*> players;
        bool selectingCoupTarget = false;
        std::string current_turn_name; // משתנה לעקוב אחר התור

        // Updates the text entries for each player, showing name, role, coins, and status
        void updateTextEntries();

    public:
        // Constructor: Initializes the renderer with a font and game, sets up UI elements and action buttons
        GameRenderer(sf::Font& font, Game& game);

        // Sets the list of players and updates their text representations
        void setPlayers(const std::vector<Player*>& players);

        // Sets the current turn, updates turn text, and refreshes button states
        void setTurn(const std::string& name);

        // Draws all UI elements (turn text, player texts, buttons, and info text) to the window
        void draw(sf::RenderWindow& window);

        // Handles mouse clicks by passing them to the appropriate buttons
        void handleClick(sf::Vector2f pos);

        // Adds a new action button with a label, position, and click callback
        void addButton(const std::string& label, sf::Vector2f pos, std::function<void()> onClick);

        // Returns the player whose turn it is, or nullptr if not found
        Player* getCurrentPlayer();

        // Updates the visibility and enabled state of buttons based on game state and player role
        void updateButtonStates();
    };

} // namespace coup