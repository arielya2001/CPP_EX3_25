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
        std::vector<ActionButton> buttons;
        std::vector<ActionButton> targetButtons; // כפתורי בחירה ל-Coup
        std::vector<Player*> players;
        bool selectingCoupTarget = false;

        void updateTextEntries();

    public:
        GameRenderer(sf::Font& font, Game& game);

        void setPlayers(const std::vector<Player*>& players);
        void setTurn(const std::string& name);
        void draw(sf::RenderWindow& window);
        void handleClick(sf::Vector2f pos);
        void addButton(const std::string& label, sf::Vector2f pos, std::function<void()> onClick);
        Player* getCurrentPlayer();
    };

} // namespace coup
