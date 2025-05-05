#include <SFML/Graphics.hpp>
#include "ActionLog.hpp"
#include "GameRenderer.hpp"
using namespace coup;

struct PlayerInfo {
    std::string name;
    std::string role;
    int coins;
    bool alive;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        return -1;
    }

    sf::Text title("Coup Game", font, 36);
    title.setPosition(280, 20);
    title.setFillColor(sf::Color::White);

    std::vector<PlayerInfo> players = {
        {"Moshe",  "Governor", 2, false},
        {"Yossi",  "Spy",      1, true},
        {"Meirav", "Baron",    7, true},
        {"Reut",   "General",  4, true},
        {"Gilad",  "Judge",    4, true},
    };

    GameRenderer renderer(font);
    renderer.setTurn("Meirav (Baron)");
    renderer.setPlayers(players);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);
                renderer.handleClick(clickPos);
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        renderer.draw(window); // כולל גם את הכפתורים
        window.display();
    }

    return 0;
}
