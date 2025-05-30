/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#pragma once  // Prevents multiple inclusions of this header file

#include <SFML/Graphics.hpp>     // Includes SFML graphics components
#include <functional>            // For std::function (used as callback)
#include <string>                // For using std::string

namespace coup {                 // Namespace for the game logic

    class ActionButton {         // Class representing a clickable action button in the GUI
    private:
        sf::RectangleShape shape;           // Graphical shape of the button (rectangle)
        sf::Text label;                     // Text displayed on the button
        std::function<void()> callback;     // Function to call when the button is clicked
        bool enabled = true;                // Whether the button is currently clickable
        std::string textLabel;              // The string label associated with the button
        bool visible = true;                // Whether the button is currently visible

    public:
        // Constructor: Initializes the button's text, font, position, size, and on-click callback
        ActionButton(sf::Font& font, const std::string& text, sf::Vector2f position,
                     sf::Vector2f size, std::function<void()> onClick)
            : callback(std::move(onClick)), textLabel(text) {
            shape.setSize(size);                           // Set the button size
            shape.setPosition(position);                   // Set the button position
            shape.setFillColor(sf::Color(100, 100, 200));  // Set the background color
            shape.setOutlineColor(sf::Color::White);       // Set outline color
            shape.setOutlineThickness(2.f);                // Set outline thickness

            label.setFont(font);                           // Assign the font to the label
            label.setString(text);                         // Set the text string
            label.setCharacterSize(16);                    // Set text size
            label.setFillColor(sf::Color::White);          // Set text color
            sf::FloatRect bounds = label.getLocalBounds(); // Get text bounds
            label.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2); // Center text
            label.setPosition(position.x + size.x / 2, position.y + size.y / 2);              // Position text
        }

        // Renders the button (if visible) to the given SFML window
        void draw(sf::RenderWindow& window) {
            if (!visible) return;        // Skip drawing if invisible
            window.draw(shape);          // Draw the rectangle
            window.draw(label);          // Draw the label
        }

        // Handles mouse click: invokes the callback if the button is enabled and clicked
        void handleClick(sf::Vector2f mousePos) {
            if (enabled && shape.getGlobalBounds().contains(mousePos)) {
                callback();              // Execute the assigned function
            }
        }

        // Enables or disables the button and changes its color accordingly
        void setEnabled(bool value) {
            enabled = value;
            shape.setFillColor(enabled ? sf::Color(100, 100, 200) : sf::Color(80, 80, 80)); // Dim if disabled
        }

        // Returns the label text associated with the button
        const std::string& getLabel() const {
            return textLabel;
        }

        // Shows or hides the button
        void setVisible(bool value) { visible = value; }

        // Returns whether the button is currently visible
        bool isVisible() const { return visible; }

    };

} // namespace coup
