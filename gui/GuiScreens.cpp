#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Governor.hpp"
#include "../Spy.hpp"
#include "../Baron.hpp"
#include "../General.hpp"
#include "../Judge.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "GuiScreens.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

namespace coup
{
    std::string popupSelectTarget(sf::RenderWindow& parentWindow, const std::vector<Player*>& players, sf::Font& font) {
        sf::RenderWindow popup(sf::VideoMode(600, 400), "Select Target", sf::Style::Titlebar | sf::Style::Close);
        sf::Text title("Select a Player", font, 36);
        title.setFillColor(sf::Color::White);
        title.setPosition(180, 20);

        while (popup.isOpen()) {
            sf::Event event;
            while (popup.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return "";
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(popup);
                    for (size_t i = 0; i < players.size(); ++i) {
                        sf::FloatRect box(100, 80 + i * 50, 400, 40);
                        if (box.contains((float)mousePos.x, (float)mousePos.y)) {
                            popup.close();
                            return players[i]->name();
                        }
                    }
                }
            }
            popup.clear(sf::Color(50, 50, 50));
            popup.draw(title);
            for (size_t i = 0; i < players.size(); ++i) {
                sf::RectangleShape box(sf::Vector2f(400, 40));
                box.setFillColor(sf::Color(100, 100, 200));
                box.setPosition(100, 80 + i * 50);
                popup.draw(box);

                sf::Text name(players[i]->name(), font, 28);
                name.setFillColor(sf::Color::White);
                name.setPosition(120, 85 + i * 50);
                popup.draw(name);
            }
            popup.display();
        }
        return "";
    }

    bool popupYesNo(sf::RenderWindow& parentWindow, sf::Font& font, const std::string& question) {
        sf::RenderWindow popup(sf::VideoMode(500, 200), "Need a target?", sf::Style::Titlebar | sf::Style::Close);
        sf::Text questionText(question, font, 28);
        questionText.setFillColor(sf::Color::White);
        questionText.setPosition(50, 50);

        sf::RectangleShape yesButton(sf::Vector2f(100, 50));
        yesButton.setFillColor(sf::Color::Green);
        yesButton.setPosition(70, 120);

        sf::Text yesText("Yes", font, 24);
        yesText.setFillColor(sf::Color::Black);
        yesText.setPosition(95, 135);

        sf::RectangleShape noButton(sf::Vector2f(100, 50));
        noButton.setFillColor(sf::Color::Red);
        noButton.setPosition(230, 120);

        sf::Text noText("No", font, 24);
        noText.setFillColor(sf::Color::Black);
        noText.setPosition(265, 135);

        while (popup.isOpen()) {
            sf::Event event;
            while (popup.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return false;
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(popup);
                    if (yesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        popup.close();
                        return true;
                    }
                    if (noButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        popup.close();
                        return false;
                    }
                }
            }
            popup.clear(sf::Color(50, 50, 50));
            popup.draw(questionText);
            popup.draw(yesButton);
            popup.draw(yesText);
            popup.draw(noButton);
            popup.draw(noText);
            popup.display();
        }
        return false;
    }

    bool runOpeningScreen() {
        sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game");
        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("resources/Coup-Game-Contents.jpg")) {
            std::cerr << "Failed to load opening background image" << std::endl;
            return false;
        }
        sf::Sprite background(backgroundTexture);
        float scaleX = (float)window.getSize().x / backgroundTexture.getSize().x;
        float scaleY = (float)window.getSize().y / backgroundTexture.getSize().y;
        background.setScale(scaleX, scaleY);

        sf::RectangleShape startButton(sf::Vector2f(300, 80));
        startButton.setFillColor(sf::Color(255, 215, 0));
        startButton.setPosition(490, 600);  // אמצע רוחבית, נמוך לגובה סביר


        sf::Font font;
        if (!font.loadFromFile("resources/font.ttf")) {
            std::cerr << "Failed to load font.ttf" << std::endl;
            return false;
        }

        sf::Text startText("Start", font, 36);
        startText.setFillColor(sf::Color::Black);
        startButton.setPosition(490, 600); // אמצע החלון פחות או יותר
        startText.setPosition(540, 625);    // בתוך הכפתור

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    return false; // סוגר הכל
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (startButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                        window.close();
                        return true; // ממשיכים למשחק
                    }
                }
            }
            window.clear();
            window.draw(background);
            window.draw(startButton);
            window.draw(startText);
            window.display();
        }
        return true; // המשך תקין
    }
    bool runAddPlayersScreen(Game& game) {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Add Players");

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font\n";
        return false;
    }

    std::vector<std::string> playerNames;
    std::string currentInput;

    sf::Text title("Add Players", font, 80);
    title.setFillColor(sf::Color::White);
    title.setPosition(550, 40);

    sf::Text instruction("(type name + press ENTER)", font, 50);
    instruction.setFillColor(sf::Color::White);
    instruction.setPosition(440, 140);

    sf::Text inputText("", font, 40);
    inputText.setFillColor(sf::Color::Black);
    sf::RectangleShape inputBox(sf::Vector2f(500, 60));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setPosition(400, 230);
    inputText.setPosition(410, 235);

    sf::Text playersLabel("Players:", font, 54);
    playersLabel.setFillColor(sf::Color::Yellow);
    playersLabel.setPosition(400, 320);

    sf::RectangleShape startButton(sf::Vector2f(300, 90));
    startButton.setFillColor(sf::Color(128, 128, 128));
        startButton.setPosition(490, 600);  // אמצע רוחבית, נמוך לגובה סביר
    sf::Text startText("Start Game", font, 42);
    startText.setFillColor(sf::Color::Black);
    startText.setPosition(540, 625);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return false;
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (!currentInput.empty())
                        currentInput.pop_back();
                } else if (event.text.unicode == '\r') {
                    if (!currentInput.empty()) {
                        bool exists = false;
                        for (const auto& name : playerNames) {
                            if (name == currentInput) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists && playerNames.size() < 6) {
                            playerNames.push_back(currentInput);
                        }
                        currentInput.clear();
                    }
                } else if (event.text.unicode < 128) {
                    currentInput += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Start button
                if (startButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    if (playerNames.size() >= 2 && playerNames.size() <= 6) {
                        for (const auto& name : playerNames) {
                            game.add_player(new Player(game, name, ""));
                        }
                        window.close();
                    }
                }

                // Remove buttons
                float listStartY = 430;
                for (size_t i = 0; i < playerNames.size(); ++i) {
                    float boxX = 700;
                    float boxY = listStartY + i * 60;

                    sf::FloatRect removeBox(boxX, boxY, 30, 30);
                    if (removeBox.contains((float)mousePos.x, (float)mousePos.y)) {
                        playerNames.erase(playerNames.begin() + i);
                        break;
                    }
                }
            }
        }

        inputText.setString(currentInput);

        window.clear(sf::Color(150, 90, 40));
        window.draw(title);
        window.draw(instruction);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(playersLabel);

        float listStartY = 430;
        for (size_t i = 0; i < playerNames.size(); ++i) {
            sf::Text playerText("- " + playerNames[i], font, 44);
            playerText.setFillColor(sf::Color::White);
            playerText.setPosition(400, listStartY + i * 60);
            window.draw(playerText);

            float boxX = 700;
            float boxY = listStartY + i * 60;

            sf::RectangleShape removeBox(sf::Vector2f(30, 30));
            removeBox.setFillColor(sf::Color::Red);
            removeBox.setPosition(boxX, boxY);
            window.draw(removeBox);

            sf::Text xText("X", font, 24);
            xText.setFillColor(sf::Color::White);
            float xCenter = boxX + (30 - xText.getLocalBounds().width) / 2;
            float yCenter = boxY + (30 - xText.getLocalBounds().height) / 2 - 5;
            xText.setPosition(xCenter, yCenter);
            window.draw(xText);
        }

        if (playerNames.size() >= 2 && playerNames.size() <= 6)
            startButton.setFillColor(sf::Color(0, 200, 0));
        else
            startButton.setFillColor(sf::Color(128, 128, 128));

        window.draw(startButton);
        window.draw(startText);
        window.display();
    }
        return true;
}
bool runAssignRolesScreen(Game& game) {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Assign Roles");

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font.ttf" << std::endl;
        return false;  // ✅ תקין!
    }

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/Coup-Game-Contents.jpg")) {
        std::cerr << "Failed to load background image" << std::endl;
        return false;
    }

    sf::Sprite background(backgroundTexture);
    background.setScale(
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Text title("Assigning Roles", font, 60);
    title.setFillColor(sf::Color::White);
    title.setPosition(450, 40);

    const std::vector<Player*>& players = game.get_all_players();
    size_t currentPlayerIndex = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::string> roleNames = {
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };

    // כפתור "Start Game"
    sf::RectangleShape startButton(sf::Vector2f(300, 90));
    startButton.setFillColor(sf::Color(100, 255, 100));
        startButton.setPosition(490, 620);

    sf::Text startText("Start Game", font, 42);
    startText.setFillColor(sf::Color::Black);
        startText.setPosition(540, 645);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return false;
            }

            // הקצאת תפקיד עם ENTER
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (currentPlayerIndex < players.size()) {
                    std::uniform_int_distribution<> dis(0, roleNames.size() - 1);
                    std::string role = roleNames[dis(gen)];
                    players[currentPlayerIndex]->set_role_name(role);
                    currentPlayerIndex++;
                }
            }

            // לחיצה על כפתור התחלה
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    if (currentPlayerIndex >= players.size()) {
                        window.close();  // יציאה למסך המשחק
                        return true;
                    }
                }
            }
        }

        window.clear();
        window.draw(background);
        window.draw(title);

        float yOffset = 180;
        for (size_t i = 0; i < players.size(); ++i) {
            std::string displayText = players[i]->name();
            if (i < currentPlayerIndex) {
                displayText += " - " + players[i]->role();
            } else if (i == currentPlayerIndex) {
                displayText += " (Press ENTER)";
            }

            sf::Text playerText(displayText, font, 36);
            playerText.setFillColor(i < currentPlayerIndex ? sf::Color::Green : sf::Color::White);
            playerText.setPosition(150, yOffset);
            window.draw(playerText);
            yOffset += 60;
        }

        if (currentPlayerIndex >= players.size()) {
            window.draw(startButton);
            window.draw(startText);
        }

        window.display();
    }
}

    void runGameLoopScreen(Game& game) {
        sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game - GUI");

        sf::Font font;
        if (!font.loadFromFile("resources/font.ttf")) {
            std::cerr << "Failed to load font.ttf" << std::endl;
            return;
        }

        // יצירת renderer והכנת השחקנים
        GameRenderer renderer(font, game);
        renderer.setPlayers(game.get_all_players());
        renderer.setTurn(game.turn());

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);
                    renderer.handleClick(clickPos);
                }
            }

            window.clear(sf::Color(30, 30, 30));
            renderer.draw(window);
            window.display();
        }
    }
}

