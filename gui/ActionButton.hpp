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

    public:
        ActionButton(sf::Font& font, const std::string& text, sf::Vector2f position,
                     sf::Vector2f size, std::function<void()> onClick)
            : callback(std::move(onClick)) {
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

        void draw(sf::RenderWindow& window) {
            window.draw(shape);
            window.draw(label);
        }

        void handleClick(sf::Vector2f mousePos) {
            if (shape.getGlobalBounds().contains(mousePos)) {
                callback();
            }
        }
    };

} // namespace coup
