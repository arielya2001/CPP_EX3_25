/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once  // Ensure the file is included only once during compilation

#include <SFML/Graphics.hpp>  // For SFML graphics components like Font, Text, RenderWindow
#include <deque>              // For maintaining a scrollable queue of messages
#include <string>             // For using std::string

namespace coup {  // All classes/functions are inside the 'coup' namespace

    class ActionLog {
    private:
        sf::Font& font;                       // Reference to the font used for rendering text
        std::deque<std::string> messages;     // Stores the recent log messages
        std::vector<sf::Text> textEntries;    // Stores visual representations of messages
        const size_t maxLines = 10;           // Max number of messages shown in the log
        sf::Vector2f position;                // Top-left position of the log on the screen

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

}  // End of namespace coup
