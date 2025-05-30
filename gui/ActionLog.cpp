/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include "ActionLog.hpp"  // Include the header file for the ActionLog class

namespace coup {  // Define everything inside the 'coup' namespace

    // Constructor: Initializes the ActionLog with a font and starting position for the log
    ActionLog::ActionLog(sf::Font& font, sf::Vector2f pos)
        : font(font), position(pos) {}  // Store font and position

    // Adds a new message to the log; if at maxLines, remove the oldest one
    void ActionLog::add(const std::string& message) {
        if (messages.size() >= maxLines) {  // If log is full
            messages.pop_front();           // Remove the oldest message
        }
        messages.push_back(message);        // Add the new message to the end
        updateTexts();                      // Refresh visual text entries
    }

    // Rebuilds the visual representation of messages in the log
    void ActionLog::updateTexts() {
        textEntries.clear();        // Clear the previous text entries
        float yOffset = 0;          // Start from the top offset
        for (const std::string& msg : messages) {           // For each message
            sf::Text t(msg, font, 16);                      // Create SFML text with size 16
            t.setFillColor(sf::Color::White);               // Set text color to white
            t.setPosition(position.x, position.y + yOffset); // Position each message vertically
            textEntries.push_back(t);                       // Add to list of renderable texts
            yOffset += 22.f;                                // Increment Y position for next line
        }
    }

    // Draws all the messages in the action log onto the window
    void ActionLog::draw(sf::RenderWindow& window) {
        for (auto& txt : textEntries) {  // Iterate through prepared text entries
            window.draw(txt);            // Draw each one to the window
        }
    }

}  // End of namespace coup
