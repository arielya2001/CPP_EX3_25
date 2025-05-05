#include "GameRenderer.hpp"
#include <iostream>
#include <sstream>

namespace coup {

    GameRenderer::GameRenderer(sf::Font& font) : font(font) {
        turnText.setFont(font);
        turnText.setCharacterSize(24);
        turnText.setFillColor(sf::Color::Cyan);
        turnText.setPosition(250, 80);

        // הוספת כפתורים
        buttons.emplace_back(font, "Gather", sf::Vector2f(100, 500), sf::Vector2f(100, 35), [this]() {
            std::cout << "Gather clicked\n";
            if (currentPlayerIndex < players.size()) {
                players[currentPlayerIndex].coins++;
                updateTextEntries();
            }
        });

        buttons.emplace_back(font, "Tax", sf::Vector2f(220, 500), sf::Vector2f(100, 35), [this]() {
            std::cout << "Tax clicked\n";
            if (currentPlayerIndex < players.size()) {
                players[currentPlayerIndex].coins += 2;
                updateTextEntries();
            }
        });
    }

    void GameRenderer::setPlayers(const std::vector<PlayerInfo>& playerList) {
        players = playerList;
        updateTextEntries();
    }

    void GameRenderer::updateTextEntries() {
        playerTexts.clear();
        for (size_t i = 0; i < players.size(); ++i) {
            std::ostringstream oss;
            oss << players[i].name << " (" << players[i].role << ") - Coins: "
                << players[i].coins << " - Status: " << (players[i].isActive ? "Alive" : "Out");


            sf::Text txt(oss.str(), font, 20);
            txt.setPosition(100, 140 + i * 30);
            txt.setFillColor(sf::Color::Yellow);
            playerTexts.push_back(txt);
        }
    }

    void GameRenderer::setTurn(const std::string& name) {
        turnText.setString("Current Turn: " + name);
        for (size_t i = 0; i < players.size(); ++i) {
            std::string fullName = players[i].name + " (" + players[i].role + ")";
            if (fullName == name) {
                currentPlayerIndex = i;
                break;
            }
        }
    }

    void GameRenderer::draw(sf::RenderWindow& window) {
        window.draw(turnText);
        for (auto& txt : playerTexts) {
            window.draw(txt);
        }
        for (auto& btn : buttons) {
            btn.draw(window);
        }
    }

    void GameRenderer::handleClick(sf::Vector2f pos) {
        for (auto& btn : buttons) {
            btn.handleClick(pos);
        }
    }

}
