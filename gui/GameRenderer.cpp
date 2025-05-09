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
                updateButtonStates();
            }
        });

        // Tax button
        buttons.emplace_back(font, "Tax", sf::Vector2f(220, 450), sf::Vector2f(100, 35), [this]() {
            std::cout << "Tax clicked\n";
            if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                p->tax();
                updateTextEntries();
                setTurn(p->getGame().turn());
                updateButtonStates();

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
                                updateButtonStates();
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
                updateButtonStates();
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
                                updateButtonStates();
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
        // Sanction button
        buttons.emplace_back(font, "Sanction", sf::Vector2f(100, 490), sf::Vector2f(100, 35), [this, &font]() {
            std::cout << "Sanction clicked\n";
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* attacker = getCurrentPlayer();
            if (!attacker || !attacker->is_active()) return;

            float x = 100.f;
            float y = 540.f;
            for (auto* target : players) {
                if (target->is_active() && target != attacker) {
                    std::string targetName = target->name();
                    targetButtons.emplace_back(font, targetName, sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, attacker, target]() {
                            try {
                                attacker->sanction(*target);
                                std::cout << attacker->name() << " sanctioned " << target->name() << std::endl;
                                updateTextEntries();
                                setTurn(this->game.turn());
                                updateButtonStates();
                            } catch (const std::exception& e) {
                                std::cerr << "Sanction error: " << e.what() << std::endl;
                            }
                            selectingCoupTarget = false;
                        }
                    );
                    x += 120;
                }
            }
        });
        // כפתור חסימת שוחד על ידי שופט
        buttons.emplace_back(font, "Block Bribe", sf::Vector2f(220, 490), sf::Vector2f(120, 35), [this, &game]() {
            Player* judge = getCurrentPlayer();
            Player* briber = game.get_bribing_player();
            if (!judge || judge->role() != "Judge" || !briber) return;

            try {
                Judge* realJudge = dynamic_cast<Judge*>(judge);
                if (!realJudge) throw std::runtime_error("Invalid judge cast.");
                realJudge->block_bribe(*briber);

                std::cout << judge->name() << " blocked bribe by " << briber->name() << std::endl;

                game.set_awaiting_bribe_block(false);
                game.set_bribing_player(nullptr);
                setTurn(game.turn());
            } catch (const std::exception& e) {
                std::cerr << "BlockBribe error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);  // מוסתר כברירת מחדל
        buttons.back().setEnabled(false);


        // כפתור דילוג על חסימה
        buttons.emplace_back(font, "Skip", sf::Vector2f(360, 490), sf::Vector2f(100, 35), [this, &game]() {
            std::cout << "Judge skipped bribe block\n";
            game.set_awaiting_bribe_block(false);
            Player* briber = game.get_bribing_player();
            game.set_bribing_player(nullptr);

            // מחזירים את התור לשחקן שביצע שוחד
            if (briber && briber->is_active()) {
                game.set_turn_to(briber); // נניח שמימשת את זה
            }
            setTurn(game.turn());

        });
        buttons.back().setVisible(false);  // מוסתר כברירת מחדל
        buttons.back().setEnabled(false);

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
                        selectingCoupTarget = false;
                        updateTextEntries();
                        setTurn(this->game.turn());
                        updateButtonStates();
                    } catch (const std::exception& e) {
                        std::cerr << "Coup error: " << e.what() << std::endl;
                        selectingCoupTarget = false;
                    }
                }
            );
            x += 120;
        }
    }
});
        // כפתור חסימת Tax ע"י נציב
            buttons.emplace_back(font, "Block Tax", sf::Vector2f(480, 490), sf::Vector2f(120, 35), [this, &game, &font]() {
            std::cout << "Block Tax clicked\n";
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* governor = getCurrentPlayer();
            if (!governor || governor->role() != "Governor" || !governor->is_active()) return;

            float x = 100.f;
            float y = 530.f;

            for (Player* target : players) {
                if (target->is_active() && target != governor && target->get_last_action() == "tax") {
                    std::string targetName = target->name();
                    targetButtons.emplace_back(font, targetName, sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, &game, &font, governor, target]() {
                            try {
                                Governor* realGov = dynamic_cast<Governor*>(governor);
                                if (!realGov) throw std::runtime_error("Invalid governor cast.");
                                realGov->undo(*target);


                                std::cout << governor->name() << " blocked tax of " << target->name() << std::endl;

                                selectingCoupTarget = false;
                                updateTextEntries();
                                game.next_turn();
                                setTurn(game.turn());
                                updateButtonStates();
                            } catch (const std::exception& e) {
                                std::cerr << "BlockTax error: " << e.what() << std::endl;
                                selectingCoupTarget = false;
                            }
                        }
                    );
                    x += 120;
                }
            }

            if (targetButtons.empty()) {
                selectingCoupTarget = false;
            }
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

















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
        updateButtonStates();
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
    void GameRenderer::updateButtonStates() {
        Player* current = getCurrentPlayer();
        if (!current) return;

        for (auto& btn : buttons) {
            const std::string& label = btn.getLabel();

            // מצב מיוחד: שופט בתור חסימת שוחד
            if (game.is_awaiting_bribe_block()) {
                if (current->role() == "Judge") {
                    // מציג רק את בלוק ודילוג
                    bool show = (label == "Block Bribe" || label == "Skip");
                    btn.setVisible(show);
                    btn.setEnabled(show);
                } else {
                    btn.setVisible(false);  // שאר השחקנים לא רואים כפתורים
                }
                continue;  // <<< חשוב מאוד!
            }
            // מצב רגיל
            if (label == "Block Bribe" || label == "Skip") {
                btn.setVisible(false);
            }
            else if (label == "Block Tax") {
                bool isGovernor = current->role() == "Governor";
                btn.setVisible(isGovernor);
                btn.setEnabled(isGovernor);
            }
            else {
                btn.setVisible(true);

                if (label == "Gather") {
                    btn.setEnabled(!current->is_gather_blocked() && !current->is_under_sanction());
                } else if (label == "Tax") {
                    btn.setEnabled(!current->is_under_sanction());
                } else if (label == "Coup") {
                    btn.setEnabled(current->coins() >= 7);
                } else {
                    btn.setEnabled(true);
                }
            }

        }
    }






} // namespace coup