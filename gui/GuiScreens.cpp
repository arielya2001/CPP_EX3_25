/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "../Game.hpp"
#include "../Player.hpp"
#include "../Governor.hpp"
#include "../Spy.hpp"
#include "../Baron.hpp"
#include "../General.hpp"
#include "../Judge.hpp"
#include "../Merchant.hpp"
#include "GuiScreens.hpp"
#include "GameRenderer.hpp"

namespace coup
{
    // Displays a popup window to select a player role and returns the chosen role
    std::string popupSelectRole(sf::RenderWindow& parentWindow, sf::Font& font) {
        sf::RenderWindow popup(sf::VideoMode(600, 400), "Choose Role", sf::Style::Titlebar | sf::Style::Close);

        std::vector<std::string> roles = {
            "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
        };

        sf::Text title("Choose Role", font, 36);
        title.setFillColor(sf::Color::White);
        title.setPosition(180, 20);

        while (popup.isOpen()) {
            sf::Event event;
            while (popup.pollEvent(event)) {
                if (event.type == sf::Event::Closed) return "";

                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(popup);
                    for (size_t i = 0; i < roles.size(); ++i) {
                        sf::FloatRect box(100, 80 + i * 50, 400, 40);
                        if (box.contains((float)mousePos.x, (float)mousePos.y)) {
                            popup.close();
                            return roles[i];
                        }
                    }
                }
            }

            popup.clear(sf::Color(40, 40, 40));
            popup.draw(title);

            for (size_t i = 0; i < roles.size(); ++i) {
                sf::RectangleShape box(sf::Vector2f(400, 40));
                box.setFillColor(sf::Color(70, 120, 200));
                box.setPosition(100, 80 + i * 50);
                popup.draw(box);

                sf::Text roleText(roles[i], font, 28);
                roleText.setFillColor(sf::Color::White);
                roleText.setPosition(120, 85 + i * 50);
                popup.draw(roleText);
            }

            popup.display();
        }

        return "";
    }

    // Displays a popup window to select a target player and returns the chosen player's name
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

    // Displays a yes/no popup with a given question and returns true for "Yes" or false for "No"
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

    // Runs the opening screen with a start button, returning true if the game should start
    bool runOpeningScreen() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game");

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) {
        std::cerr << "Failed to load opening background image" << std::endl;
        return false;
    }
    sf::Sprite background(backgroundTexture);
    background.setScale(
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) { // אפשר להחליף כאן לפונט דרמטי
        std::cerr << "Failed to load font.ttf" << std::endl;
        return false;
    }

    sf::Vector2f buttonSize(320, 70);
    sf::RectangleShape startButton(buttonSize);
    startButton.setFillColor(sf::Color::Black);
    startButton.setOutlineColor(sf::Color(255, 215, 0)); // Gold
    startButton.setOutlineThickness(4);
    startButton.setPosition(
        (window.getSize().x - buttonSize.x) / 2,
        window.getSize().y - 140
    );

    sf::Text startText("Let the Games Begin", font, 34);
    startText.setFillColor(sf::Color(255, 215, 0)); // Gold
    sf::FloatRect textBounds = startText.getLocalBounds();
    startText.setPosition(
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    window.close();
                    return true;
                }
            }
        }

        // Hover effect
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setOutlineColor(sf::Color::White);
            startText.setFillColor(sf::Color::White);
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0));
            startText.setFillColor(sf::Color(255, 215, 0));
        }

        window.clear();
        window.draw(background);
        window.draw(startButton);
        window.draw(startText);
        window.display();
    }

    return true;
    }

    // Runs the screen for adding players, allowing name input and returning true if players are added successfully
    bool runAddPlayersScreen(Game& game) {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Add Players");

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) {
        std::cerr << "Failed to load opening background image" << std::endl;
        return false;
    }
    sf::Sprite background(backgroundTexture);
    background.setScale(
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font\n";
        return false;
    }

    std::vector<std::string> playerNames;
    std::string currentInput;

    sf::Text title("Add Players", font, 64);
    title.setFillColor(sf::Color::White);
    title.setPosition(640 - title.getLocalBounds().width / 2, 30);

    sf::Text instruction("(type name + press ENTER)", font, 36);
    instruction.setFillColor(sf::Color::White);
    instruction.setPosition(640 - instruction.getLocalBounds().width / 2, 110);

    sf::Text inputText("", font, 30);
    inputText.setFillColor(sf::Color::Black);
    sf::RectangleShape inputBox(sf::Vector2f(500, 50));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setPosition(390, 170);
    inputText.setPosition(400, 180);

    sf::Text playersLabel("Players:", font, 40);
    playersLabel.setFillColor(sf::Color::Yellow);
    playersLabel.setPosition(390, 250);

    // Use consistent button size with opening screen
    sf::Vector2f buttonSize(310, 65);
    sf::RectangleShape startButton(buttonSize);
    startButton.setFillColor(sf::Color::Black);
    startButton.setOutlineColor(sf::Color(255, 215, 0));
    startButton.setOutlineThickness(4);
    startButton.setPosition((1280 - buttonSize.x) / 2, 600);

    sf::Text startText("Gather the Players", font, 34);
    startText.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect textBounds = startText.getLocalBounds();
    startText.setPosition(
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !currentInput.empty()) {
                    currentInput.pop_back();
                } else if (event.text.unicode == '\r') {
                    if (!currentInput.empty() && playerNames.size() < 6 &&
                        std::find(playerNames.begin(), playerNames.end(), currentInput) == playerNames.end()) {
                        playerNames.push_back(currentInput);
                    }
                    currentInput.clear();
                } else if (event.text.unicode < 128) {
                    currentInput += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    if (playerNames.size() >= 2 && playerNames.size() <= 6) {
                        for (const auto& name : playerNames) {
                            game.add_player(new Player(game, name, ""));
                        }
                        window.close();
                    }
                }
                float listStartY = 320;
                for (size_t i = 0; i < playerNames.size(); ++i) {
                    sf::FloatRect removeBox(700, listStartY + i * 45, 30, 30);
                    if (removeBox.contains(mousePos)) {
                        playerNames.erase(playerNames.begin() + i);
                        break;
                    }
                }
            }
        }

        // Hover effect
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setOutlineColor(sf::Color::White);
            startText.setFillColor(sf::Color::White);
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0));
            startText.setFillColor(sf::Color(255, 215, 0));
        }

        inputText.setString(currentInput);

        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(instruction);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(playersLabel);

        float listStartY = 320;
        for (size_t i = 0; i < playerNames.size(); ++i) {
            sf::Text playerText("- " + playerNames[i], font, 32);
            playerText.setFillColor(sf::Color::White);
            playerText.setPosition(390, listStartY + i * 45);
            window.draw(playerText);

            sf::RectangleShape removeBox(sf::Vector2f(30, 30));
            removeBox.setFillColor(sf::Color::Red);
            removeBox.setPosition(700, listStartY + i * 45);
            window.draw(removeBox);

            sf::Text xText("X", font, 22);
            xText.setFillColor(sf::Color::White);
            xText.setPosition(710, listStartY + i * 45);
            window.draw(xText);
        }

        window.draw(startButton);
        window.draw(startText);
        window.display();
    }

    return true;
    }

    // Runs the screen for assigning roles to players, allowing role selection and returning true on completion
    bool runAssignRolesScreen(Game& game) {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Assign Roles");

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font.ttf" << std::endl;
        return false;
    }

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) {
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
    title.setPosition(640 - title.getLocalBounds().width / 2, 40);

    std::vector<Player*>& players = const_cast<std::vector<Player*>&>(game.get_all_players());
    size_t currentPlayerIndex = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::string> roleNames = {
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };

    sf::Vector2f buttonSize(330, 70);
    sf::RectangleShape startButton(buttonSize);
    startButton.setFillColor(sf::Color::Black);
    startButton.setOutlineColor(sf::Color(255, 215, 0)); // זהב
    startButton.setOutlineThickness(4);
    startButton.setPosition((1280 - buttonSize.x) / 2, 600);

    sf::Text startText("The Deception Begins", font, 34);
    startText.setFillColor(sf::Color(255, 215, 0)); // זהב
    sf::FloatRect textBounds = startText.getLocalBounds();
    startText.setPosition(
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (currentPlayerIndex < players.size()) {
                    // רשימת התפקידים
                    std::vector<std::string> roleNames = {"Governor", "Spy", "Baron", "General", "Judge", "Merchant"};
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, roleNames.size() - 1);
                    std::string role = roleNames[dis(gen)]; // בחירה רנדומלית של תפקיד

                    std::string name = players[currentPlayerIndex]->name();
                    Player* newPlayer = nullptr;

                    if (role == "Governor") newPlayer = new Governor(game, name);
                    else if (role == "Spy") newPlayer = new Spy(game, name);
                    else if (role == "Baron") newPlayer = new Baron(game, name);
                    else if (role == "General") newPlayer = new General(game, name);
                    else if (role == "Judge") newPlayer = new Judge(game, name);
                    else if (role == "Merchant") newPlayer = new Merchant(game, name);

                    delete players[currentPlayerIndex];
                    players[currentPlayerIndex] = newPlayer;

                    currentPlayerIndex++;
                }
            }


            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
                if (startButton.getGlobalBounds().contains(mousePos) &&
                    currentPlayerIndex >= players.size()) {
                    window.close();
                    return true;
                }
            }
        }

        // עיצוב Hover
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setOutlineColor(sf::Color::White);
            startText.setFillColor(sf::Color::White);
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0));
            startText.setFillColor(sf::Color(255, 215, 0));
        }

        window.clear();
        window.draw(background);
        window.draw(title);

        float yOffset = 160;
        for (size_t i = 0; i < players.size(); ++i) {
            std::string displayText = players[i]->name();
            if (i < currentPlayerIndex) {
                displayText += " - " + players[i]->role();
            } else if (i == currentPlayerIndex) {
                displayText += " (Press ENTER)";
            }

            sf::Text playerText(displayText, font, 36);
            playerText.setFillColor(i < currentPlayerIndex ? sf::Color(255, 215, 0) : sf::Color::White);
            playerText.setPosition(160, yOffset);
            window.draw(playerText);
            yOffset += 55;
        }

        if (currentPlayerIndex >= players.size()) {
            window.draw(startButton);
            window.draw(startText);
        }

        window.display();
    }

    return true;
    }

    // Runs the main game loop, handling user interactions and displaying the game state
    void runGameLoopScreen(Game& game) {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game - GUI");

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) {
        std::cerr << "Failed to load background image" << std::endl;
        return;
    }
    sf::Sprite background(backgroundTexture);
    background.setScale(
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font.ttf" << std::endl;
        return;
    }

    GameRenderer renderer(font, game);
    renderer.setPlayers(game.get_all_players());
    renderer.setTurn(game.turn());

    bool gameEnded = false;
    bool showWinnerPopup = false;
    std::string winnerName;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameEnded && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);
                renderer.handleClick(clickPos);
            }

            if (showWinnerPopup && event.type == sf::Event::MouseButtonPressed) {
                window.close();  // סגור רק לאחר לחיצה, לא אוטומטית
            }
        }

        // Try to check for winner only if game not ended
        if (!gameEnded) {
            try {
                winnerName = game.winner();  // אם אין מנצח עדיין, תיזרק שגיאה
                gameEnded = true;
                showWinnerPopup = true;
            } catch (const std::exception&) {
                // No winner yet – ignore
            }
        }

        window.clear();
        window.draw(background);
        renderer.draw(window);

        if (showWinnerPopup) {
            // תיבת ניצחון באמצע המסך
            sf::RectangleShape popup(sf::Vector2f(500, 200));
            popup.setFillColor(sf::Color(0, 0, 0, 230));
            popup.setOutlineColor(sf::Color(255, 215, 0));
            popup.setOutlineThickness(4);
            popup.setPosition(390, 260);

            sf::Text winText(winnerName + " wins the game!", font, 36);
            winText.setFillColor(sf::Color(255, 215, 0));
            sf::FloatRect textBounds = winText.getLocalBounds();
            winText.setPosition(640 - textBounds.width / 2, 300);

            sf::Text exitText("Click anywhere to exit...", font, 24);
            exitText.setFillColor(sf::Color::White);
            sf::FloatRect exitBounds = exitText.getLocalBounds();
            exitText.setPosition(640 - exitBounds.width / 2, 370);

            window.draw(popup);
            window.draw(winText);
            window.draw(exitText);
        }

        window.display();
    }
    }

}