#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace coup {

    class ActionButton {
    private:
        sf::RectangleShape shape;
        sf::Text label;
        std::function<void()> callback;
        bool enabled = true;
        std::string textLabel;
        bool visible = true;

    public:
        // Constructor: Initializes the button with a font, text, position, size, and click callback
        ActionButton(sf::Font& font, const std::string& text, sf::Vector2f position,
                     sf::Vector2f size, std::function<void()> onClick)
            : callback(std::move(onClick)), textLabel(text) {
            shape.setSize(size);
            shape.setPosition(position);
            shape.setFillColor(sf::Color(100, 100, 200));
            shape.setOutlineColor(sf::Color::White);
            shape.setOutlineThickness(2.f);

            label.setFont(font);
            label.setString(text);
            label.setCharacterSize(16);
            label.setFillColor(sf::Color::White);
            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
            label.setPosition(position.x + size.x / 2, position.y + size.y / 2);
        }

        // Draws the button's shape and label to the provided SFML window if visible
        void draw(sf::RenderWindow& window) {
            if (!visible) return;
            window.draw(shape);
            window.draw(label);
        }

        // Handles a click event by invoking the callback if the button is enabled and clicked
        void handleClick(sf::Vector2f mousePos) {
            if (enabled && shape.getGlobalBounds().contains(mousePos)) {
                callback();
            }
        }

        // Sets the button's enabled state and updates its color accordingly
        void setEnabled(bool value) {
            enabled = value;
            shape.setFillColor(enabled ? sf::Color(100, 100, 200) : sf::Color(80, 80, 80));
        }

        // Returns the button's text label
        const std::string& getLabel() const {
            return textLabel;
        }

        // Sets the button's visibility state
        void setVisible(bool value) { visible = value; }

        // Returns whether the button is visible
        bool isVisible() const { return visible; }

    };

} // namespace coup