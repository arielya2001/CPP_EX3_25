#include "ActionLog.hpp"

namespace coup {

    ActionLog::ActionLog(sf::Font& font, sf::Vector2f pos)
        : font(font), position(pos) {}

    void ActionLog::add(const std::string& message) {
        if (messages.size() >= maxLines) {
            messages.pop_front();
        }
        messages.push_back(message);
        updateTexts();
    }

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

    void ActionLog::draw(sf::RenderWindow& window) {
        for (auto& txt : textEntries) {
            window.draw(txt);
        }
    }

}
