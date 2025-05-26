/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
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
        // Constructor: Initializes the ActionLog with a font and optional position
        ActionLog(sf::Font& font, sf::Vector2f pos = {500.f, 140.f});

        // Adds a new message to the log, removing the oldest if the maximum line limit is reached
        void add(const std::string& message);

        // Updates the text entries for rendering, positioning each message vertically
        void updateTexts();

        // Draws all text entries in the log to the provided SFML window
        void draw(sf::RenderWindow& window);

    };

}