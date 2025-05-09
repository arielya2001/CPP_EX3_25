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
        buttons.emplace_back(font, "Gather", sf::Vector2f(100, 360), sf::Vector2f(100, 35), [this]() {
            std::cout << "Gather clicked\n";
            if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                p->gather();
                updateTextEntries();
                setTurn(p->getGame().turn()); // עדכון טקסט תור
                updateButtonStates();
            }
        });

        // Tax button
        buttons.emplace_back(font, "Tax", sf::Vector2f(220, 360), sf::Vector2f(100, 35), [this]() {
            std::cout << "Tax clicked\n";
            if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                p->tax();
                updateTextEntries();
                setTurn(p->getGame().turn());
                updateButtonStates();

            }
        });

        // Coup button
        buttons.emplace_back(font, "Coup", sf::Vector2f(340, 360), sf::Vector2f(100, 35), [this, &font]() {
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
        buttons.emplace_back(font, "Bribe", sf::Vector2f(460, 360), sf::Vector2f(100, 35), [this, &game]() {
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
        buttons.emplace_back(font, "Arrest", sf::Vector2f(580, 360), sf::Vector2f(100, 35), [this, &font]() {
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
        buttons.emplace_back(font, "Sanction", sf::Vector2f(100, 420), sf::Vector2f(100, 35), [this, &font]() {
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
        // כפתור חסימת Tax ע"י נציב
            buttons.emplace_back(font, "Block Tax", sf::Vector2f(480, 420), sf::Vector2f(120, 35), [this, &game, &font]() {
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

// כפתור Reveal Coins – מציג כמה מטבעות יש לשחקן אחר
buttons.emplace_back(font, "Reveal Coins", sf::Vector2f(100, 500), sf::Vector2f(120, 35), [this, &font]() {
    std::cout << "Reveal Coins clicked\n";
    selectingCoupTarget = true;
    targetButtons.clear();

    Player* spy = getCurrentPlayer();
    if (!spy || spy->role() != "Spy" || !spy->is_active()) return;

    float x = 100.f;
    float y = 540.f;

    for (Player* target : players) {
        if (target->is_active() && target != spy) {
            std::string label = target->name();
            targetButtons.emplace_back(font, label, sf::Vector2f(x, y), sf::Vector2f(100, 30),
                [this, &font, spy, target]() {
                    try {
                        Spy* realSpy = dynamic_cast<Spy*>(spy);
                        if (!realSpy) throw std::runtime_error("Invalid spy cast.");
                        int coins = realSpy->spy_on(*target);

                        std::cout << "🕵️ " << target->name() << " has " << coins << " coins.\n";

                        selectingCoupTarget = false;
                    } catch (const std::exception& e) {
                        std::cerr << "RevealCoins error: " << e.what() << std::endl;
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

// כפתור Block Arrest – חוסם arrest משחקן אחר בתורו הבא
buttons.emplace_back(font, "Block Arrest", sf::Vector2f(240, 500), sf::Vector2f(120, 35), [this, &font]() {
    std::cout << "Block Arrest clicked\n";
    selectingCoupTarget = true;
    targetButtons.clear();

    Player* spy = getCurrentPlayer();
    if (!spy || spy->role() != "Spy" || !spy->is_active()) return;

    float x = 100.f;
    float y = 570.f;

    for (Player* target : players) {
        if (target->is_active() && target != spy) {
            std::string label = target->name();
            targetButtons.emplace_back(font, label, sf::Vector2f(x, y), sf::Vector2f(100, 30),
                [this, &font, spy, target]() {
                    try {
                        Spy* realSpy = dynamic_cast<Spy*>(spy);
                        if (!realSpy) throw std::runtime_error("Invalid spy cast.");

                        realSpy->block_arrest(*target);

                        std::cout << "🛑 " << target->name() << " is blocked from arresting in next turn.\n";

                        selectingCoupTarget = false;
                    } catch (const std::exception& e) {
                        std::cerr << "BlockArrest error: " << e.what() << std::endl;
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

        // Baron - Invest button (3 coins → 6 coins)
        buttons.emplace_back(font, "Invest", sf::Vector2f(700, 360), sf::Vector2f(100, 35), [this, &game]() {
            std::cout << "Invest clicked\n";
            Player* current = getCurrentPlayer();
            if (!current || current->role() != "Baron" || !current->is_active()) return;

            try {
                Baron* baron = dynamic_cast<Baron*>(current);
                if (!baron) throw std::runtime_error("Invalid baron cast.");
                baron->invest();
                std::cout << baron->name() << " invested: paid 3 coins, received 6.\n";

                updateTextEntries();
                setTurn(game.turn());
                updateButtonStates();
            } catch (const std::exception& e) {
                std::cerr << "Invest error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // General - Protect from Coup
        // כפתור Protect from Coup – זז שמאלה ויותר למטה
        buttons.emplace_back(font, "Protect from Coup", sf::Vector2f(680, 460), sf::Vector2f(160, 35), [this, &game, &font]() {
                    std::cout << "Protect from Coup clicked\n";
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* general = getCurrentPlayer();
            if (!general || general->role() != "General" || !general->is_active()) return;

            float x = 100.f;
            float y = 570.f;

            for (Player* target : players) {
                if (target->is_active()) {
                    std::string label = target->name();
                    targetButtons.emplace_back(font, label, sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, general, target, &game, &font]() {
                            try {
                                General* realGen = dynamic_cast<General*>(general);
                                if (!realGen) throw std::runtime_error("Invalid general cast.");
                                if (realGen->coins() < 5) throw std::runtime_error("Not enough coins to protect.");

                                realGen->deduct_coins(5);
                                realGen->protected_players.insert(target);
                                std::cout << general->name() << " is now protecting " << target->name() << " from coup.\n";

                                updateTextEntries();
                                game.next_turn();
                                setTurn(game.turn());
                                updateButtonStates();
                            } catch (const std::exception& e) {
                                std::cerr << "ProtectCoup error: " << e.what() << std::endl;
                            }
                            selectingCoupTarget = false;
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
        Player* current = getCurrentPlayer();
        if (current) {
            current->on_turn_start(); // קריאה כאן
        }
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
                bool show = (label == "Block Bribe" || label == "Skip");
                btn.setVisible(show);
                btn.setEnabled(show);
            } else {
                btn.setVisible(false);  // שאר השחקנים לא רואים כפתורים
            }
            continue;
        }

        // מצב מיוחד: גנרל בתור חסימת coup
        if (game.is_awaiting_coup_block()) {
            if (current->role() == "General") {
                bool show = (label == "Block Coup" || label == "Skip Coup Block");
                btn.setVisible(show);
                btn.setEnabled(show);
            } else {
                btn.setVisible(false);  // שאר השחקנים לא רואים כפתורים
            }
            continue;
        }

        // כפתורים שמוסתרים תמיד חוץ מבמצבים מיוחדים
        if (label == "Block Bribe" || label == "Skip" || label == "Block Coup" || label == "Skip Coup Block") {
            btn.setVisible(false);
            continue;
        }

        // --- כפתורים ייחודיים לפי תפקיד ---

        if (label == "Block Tax") {
            bool isGovernor = current->role() == "Governor";
            btn.setVisible(isGovernor);
            btn.setEnabled(isGovernor);
            continue;
        }
        if (label == "Protect from Coup") {
            bool isGeneral = current->role() == "General" && current->coins() >= 5;
            btn.setVisible(isGeneral);
            btn.setEnabled(isGeneral);
            continue;
        }


        if (label == "Reveal Coins" || label == "Block Arrest") {
            bool isSpy = current->role() == "Spy";
            btn.setVisible(isSpy);
            btn.setEnabled(isSpy);
            continue;
        }

        if (label == "Invest") {
            bool isBaron = current->role() == "Baron" && current->coins() >= 3;
            btn.setVisible(isBaron);
            btn.setEnabled(isBaron);
            continue;
        }

        // --- כל שאר הכפתורים הרגילים ---
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










} // namespace coup