#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <string>

namespace coup {

    class ActionLog {
    private:
        sf::Font& font;
        std::deque<std::string> messages;
        std::vector<sf::Text> textEntries;
        const size_t maxLines = 10;
        sf::Vector2f position;

    public:
        ActionLog(sf::Font& font, sf::Vector2f pos = {500.f, 140.f});

        void add(const std::string& message);
        void updateTexts();
        void draw(sf::RenderWindow& window);
    };

}
