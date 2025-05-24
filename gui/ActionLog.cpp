#include "ActionLog.hpp"

namespace coup {

    // Constructor: Initializes the ActionLog with a font and position for text display
    ActionLog::ActionLog(sf::Font& font, sf::Vector2f pos)
        : font(font), position(pos) {}

    // Adds a new message to the log, removing the oldest if the maximum line limit is reached
    void ActionLog::add(const std::string& message) {
        if (messages.size() >= maxLines) {
            messages.pop_front();
        }
        messages.push_back(message);
        updateTexts();
    }

    // Updates the text entries for rendering, positioning each message vertically
    void ActionLog::updateTexts() {
        textEntries.clear();
        float yOffset = 0;
        for (const std::string& msg : messages) {
            sf::Text t(msg, font, 16);
            t.setFillColor(sf::Color::White);
            t.setPosition(position.x, position.y + yOffset);
            textEntries.push_back(t);
            yOffset += 22.f;
        }
    }

    // Draws all text entries in the log to the provided SFML window
    void ActionLog::draw(sf::RenderWindow& window) {
        for (auto& txt : textEntries) {
            window.draw(txt);
        }
    }

}