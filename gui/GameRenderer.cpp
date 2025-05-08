#include "GameRenderer.hpp"
#include <iostream>
#include <sstream>

namespace coup {

    GameRenderer::GameRenderer(sf::Font& font, Game& game) : font(font), game(game) {
        turnText.setFont(font);
        turnText.setCharacterSize(24);
        turnText.setFillColor(sf::Color::Cyan);
        turnText.setPosition(250, 80);

        // Gather button
        buttons.emplace_back(font, "Gather", sf::Vector2f(100, 450), sf::Vector2f(100, 35), [this]() {
            std::cout << "Gather clicked\n";
            if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                p->gather();
                updateTextEntries();
                setTurn(p->getGame().turn()); // עדכון טקסט תור
            }
        });

        // Tax button
        buttons.emplace_back(font, "Tax", sf::Vector2f(220, 450), sf::Vector2f(100, 35), [this]() {
            std::cout << "Tax clicked\n";
            if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                p->tax();
                updateTextEntries();
                setTurn(p->getGame().turn());
            }
        });

        // Coup button
        buttons.emplace_back(font, "Coup", sf::Vector2f(340, 450), sf::Vector2f(100, 35), [this, &font]() {
            std::cout << "Coup clicked\n";
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* attacker = getCurrentPlayer();
            if (!attacker || !attacker->is_active()) return;

            float x = 100.f;
            float y = 550.f;
            for (auto* target : players) {
                if (target->is_active() && target != attacker) {
                    std::string targetName = target->name();
                    targetButtons.emplace_back(font, targetName, sf::Vector2f(x, 500), sf::Vector2f(100, 30),
                        [this, attacker, target]() {
                            try {
                                attacker->coup(*target);
                                std::cout << attacker->name() << " performed coup on " << target->name() << std::endl;
                                updateTextEntries();
                                setTurn(this->game.turn());
                            } catch (const std::exception& e) {
                                std::cerr << "Coup error: " << e.what() << std::endl;
                            }
                            selectingCoupTarget = false;
                        }
                    );
                    x += 120;
                }
            }
        });
        // Bribe button
        buttons.emplace_back(font, "Bribe", sf::Vector2f(460, 450), sf::Vector2f(100, 35), [this, &game]() {
            std::cout << "Bribe clicked\n";
            Player* current = getCurrentPlayer();
            if (!current || !current->is_active()) return;

            try {
                current->bribe(*current);
                std::cout << current->name() << " performed bribe.\n";
                updateTextEntries();
                setTurn(game.turn());
            } catch (const std::exception& e) {
                std::cerr << "Bribe error: " << e.what() << std::endl;
            }
        });
        // בתוך GameRenderer::GameRenderer
        buttons.emplace_back(font, "Arrest", sf::Vector2f(580, 450), sf::Vector2f(100, 35), [this, &font]() {
            std::cout << "Arrest clicked\n";
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* arrestingPlayer = getCurrentPlayer();
            if (!arrestingPlayer || !arrestingPlayer->is_active()) return;

            float x = 100.f;
            float y = 590.f;
            for (auto* target : players) {
                if (target->is_active() && target != arrestingPlayer) {
                    std::string targetName = target->name();
                    targetButtons.emplace_back(font, targetName, sf::Vector2f(x, 500), sf::Vector2f(100, 30),
                        [this, arrestingPlayer, target]() {
                            try {
                                arrestingPlayer->arrest(*target);
                                std::cout << arrestingPlayer->name() << " arrested " << target->name() << std::endl;
                                updateTextEntries();
                                setTurn(this->game.turn());
                            } catch (const std::exception& e) {
                                std::cerr << "Arrest error: " << e.what() << std::endl;
                            }
                            selectingCoupTarget = false;
                        }
                    );
                    x += 120;
                }
            }
        });






    }

    void GameRenderer::setPlayers(const std::vector<Player*>& playerList) {
        players = playerList;
        updateTextEntries();
    }

    void GameRenderer::updateTextEntries() {
        playerTexts.clear();
        for (size_t i = 0; i < players.size(); ++i) {
            std::ostringstream oss;
            oss << players[i]->name() << " (" << players[i]->role() << ") - Coins: "
                << players[i]->coins() << " - Status: " << (players[i]->is_active() ? "Alive" : "Out");

            sf::Text txt(oss.str(), font, 20);
            txt.setPosition(100, 140 + i * 30);
            txt.setFillColor(sf::Color::Yellow);
            playerTexts.push_back(txt);
        }
    }

    void GameRenderer::setTurn(const std::string& name) {
        turnText.setString("Current Turn: " + name);
    }

    Player* GameRenderer::getCurrentPlayer() {
        for (auto* p : players) {
            if (p->name() == game.turn()) return p;
        }
        return nullptr;
    }

    void GameRenderer::draw(sf::RenderWindow& window) {
        window.draw(turnText);
        for (auto& txt : playerTexts) {
            window.draw(txt);
        }
        for (auto& btn : buttons) {
            btn.draw(window);
        }
        if (selectingCoupTarget) {
            for (auto& tbtn : targetButtons) {
                tbtn.draw(window);
            }
        }
    }

    void GameRenderer::handleClick(sf::Vector2f pos) {
        if (selectingCoupTarget) {
            for (auto& tbtn : targetButtons) {
                tbtn.handleClick(pos);
            }
        } else {
            for (auto& btn : buttons) {
                btn.handleClick(pos);
            }
        }
    }

} // namespace coup
