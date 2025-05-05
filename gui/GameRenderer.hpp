#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include "ActionButton.hpp"

namespace coup {

    struct PlayerInfo {
        std::string name;
        std::string role;
        int coins;
        bool isActive;
    };

    class GameRenderer {
    private:
        sf::Font& font;
        std::vector<sf::Text> playerTexts;
        sf::Text turnText;
        std::vector<ActionButton> buttons;
        std::vector<PlayerInfo> players;
        size_t currentPlayerIndex = 0;
        void updateTextEntries();

    public:
        GameRenderer(sf::Font& font);

        void setPlayers(const std::vector<PlayerInfo>& players);
        void setTurn(const std::string& name);
        void draw(sf::RenderWindow& window);
        void handleClick(sf::Vector2f pos);
        void addButton(const std::string& label, sf::Vector2f pos, std::function<void()> onClick);
    };

}
