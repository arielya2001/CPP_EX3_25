#include <SFML/Graphics.hpp>
#include "ActionLog.hpp"
#include "GameRenderer.hpp"
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Governor.hpp"
#include "../Spy.hpp"
#include "../Baron.hpp"
#include "../General.hpp"
#include "../Judge.hpp"

using namespace coup;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        return -1;
    }

    sf::Text title("Coup Game", font, 36);
    title.setPosition(280, 20);
    title.setFillColor(sf::Color::White);

    Game game;
    Governor moshe(game, "Moshe");
    Spy yossi(game, "Yossi");
    Baron meirav(game, "Meirav");
    General reut(game, "Reut");
    Judge gilad(game, "Gilad");

    std::vector<Player*> players = { &moshe, &yossi, &meirav, &reut, &gilad };

    GameRenderer renderer(font,game);
    renderer.setPlayers(players);
    renderer.setTurn("Meirav (Baron)");

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
        renderer.draw(window);
        window.display();
    }

    return 0;
}
