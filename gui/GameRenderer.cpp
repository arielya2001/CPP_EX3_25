#include "GameRenderer.hpp"
#include <iostream>
#include <sstream>

namespace coup
{
    // Constructor: Initializes the renderer with a font and game, sets up UI elements and action buttons
    GameRenderer::GameRenderer(sf::Font& font, Game& game) : font(font), game(game), current_turn_name("") {
        turnText.setFont(font);
        turnText.setCharacterSize(32);
        turnText.setFillColor(sf::Color::Cyan);
        turnText.setPosition(640 - 150, 20);  // ממרכז טיפה שמאלה

        infoText.setFont(font);
        infoText.setCharacterSize(28);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(640 - 300, 600);  // מיקום בתחתית המסך
        infoText.setString("");  // התחלה ריקה

        // מרכוז וריווח חכם של כפתורי פעולה ראשיים
        float buttonWidth = 150.f;
        float buttonHeight = 50.f;
        float spacing = 20.f;
        std::vector<std::string> mainActions = { "Gather", "Tax", "Coup", "Bribe", "Arrest", "Sanction" };
        float totalWidth = mainActions.size() * buttonWidth + (mainActions.size() - 1) * spacing;
        float startX = 640 - totalWidth / 2.0f;
        float y = 360.f;

        for (size_t i = 0; i < mainActions.size(); ++i) {
            std::string label = mainActions[i];
            sf::Vector2f position(startX + i * (buttonWidth + spacing), y);

            auto createTargetButtons = [this, &label, &game, &font](Player* source, void (Player::*action)(Player&)) {
                selectingCoupTarget = true;
                targetButtons.clear();
                float x = 100.f;
                float y = 500.f;
                for (auto* target : players) {
                    if (target->is_active() && target != source) {
                        targetButtons.emplace_back(font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                            [this, source, target, action, &game]() {
                                try {
                                    (source->*action)(*target);
                                    updateTextEntries();
                                    setTurn(game.turn());
                                    updateButtonStates();
                                } catch (const std::exception& e) {
                                    std::cerr << "Action error: " << e.what() << std::endl;
                                }
                                selectingCoupTarget = false;
                            });
                        x += 120;
                    }
                }
            };

            if (label == "Gather") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                        p->gather();
                        updateTextEntries();
                        setTurn(game.turn());
                        updateButtonStates();
                    }
                });
            } else if (label == "Tax") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                        p->tax();
                        updateTextEntries();
                        setTurn(game.turn());
                        updateButtonStates();
                    }
                });
            } else if (label == "Coup") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, createTargetButtons]() {
                    Player* attacker = getCurrentPlayer();
                    if (attacker && attacker->is_active()) {
                        createTargetButtons(attacker, &Player::coup);
                    }
                });
            } else if (label == "Bribe") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    Player* p = getCurrentPlayer();
                    if (!p || !p->is_active()) return;
                    try {
                        p->bribe();
                        updateTextEntries();
                        setTurn(game.turn());
                        updateButtonStates();
                    } catch (const std::exception& e) {
                        std::cerr << "Bribe error: " << e.what() << std::endl;
                    }
                });
            } else if (label == "Arrest") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, createTargetButtons]() {
                    Player* arrestingPlayer = getCurrentPlayer();
                    if (arrestingPlayer && arrestingPlayer->is_active()) {
                        createTargetButtons(arrestingPlayer, &Player::arrest);
                    }
                });
            }
            else if (label == "Sanction") {
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this]() {
                    Player* attacker = getCurrentPlayer();
                    if (!attacker || !attacker->is_active()) return;
                    selectingCoupTarget = true;
                    targetButtons.clear();
                    float x = 100.f;
                    float y = 540.f;
                    for (auto* target : players) {
                        if (target->is_active() && target != attacker) {
                            targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                                [this, attacker, target]() {
                                    try {
                                        attacker->sanction(*target);
                                        updateTextEntries();
                                        setTurn(this->game.turn());
                                        updateButtonStates();
                                    } catch (const std::exception& e) {
                                        std::cerr << "Sanction error: " << e.what() << std::endl;
                                    }
                                    selectingCoupTarget = false;
                                });
                            x += 120;
                        }
                    }
                });
            }

        }





        // === כפתור Block Bribe ===
        buttons.emplace_back(font, "Block Bribe", sf::Vector2f(640 - 150.f / 2, 430.f), sf::Vector2f(150, 50), [this]() {
            Player* judge = getCurrentPlayer();
            Player* briber = this->game.get_bribing_player();
            if (!judge || judge->role() != "Judge" || !briber) return;

            try {
                auto* realJudge = dynamic_cast<Judge*>(judge);
                if (!realJudge) throw std::runtime_error("Invalid judge cast.");
                realJudge->block_bribe(*briber);
                this->game.set_awaiting_bribe_block(false);
                this->game.set_bribing_player(nullptr);
                setTurn(this->game.turn());
            } catch (const std::exception& e) {
                std::cerr << "BlockBribe error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // === כפתור Skip (Bribe) ===
        buttons.emplace_back(font, "Skip", sf::Vector2f(640 - 150.f / 2, 490.f), sf::Vector2f(150, 50), [this, &game]() {
            game.advance_bribe_block_queue();  // מעביר את התור או מחזיר למשחד
            setTurn(game.turn());              // עדכון גרפי של התור
            updateButtonStates();             // עדכון מצב כפתורים
        });



        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // === כפתור Block Tax ===
        // === כפתור Block Tax ===
        buttons.emplace_back(font, "Block Tax", sf::Vector2f(300, 430.f), sf::Vector2f(150, 50), [this]() {
            Player* governor = getCurrentPlayer();
            Player* taxed = this->game.get_tax_target();

            std::cout << "[DEBUG] Block Tax button pressed by: " << (governor ? governor->name() : "null") << std::endl;
            std::cout << "[DEBUG] Taxed target: " << (taxed ? taxed->name() : "null") << std::endl;

            if (!governor || governor->role() != "Governor" || !governor->is_active()) {
                std::cout << "[DEBUG] Governor is invalid or inactive.\n";
                return;
            }

            if (!taxed || !taxed->is_active()) {
                std::cout << "[DEBUG] Taxed player is invalid or inactive.\n";
                return;
            }

            std::cout << "[DEBUG] Taxed player's last action: " << taxed->get_last_action() << std::endl;

            if (taxed->get_last_action() != "tax") {
                std::cout << "[DEBUG] Taxed player did not perform tax.\n";
                return;
            }

            try {
        auto* realGov = dynamic_cast<Governor*>(governor);
        if (!realGov) {
            std::cerr << "Invalid governor cast.\n";
            return;
        }

        std::cout << "[DEBUG] Calling realGov->block_tax()\n";
        realGov->block_tax(*taxed);

        this->game.set_awaiting_tax_block(false);  // ⬅️ מוודא שלא יהיה ניסיון נוסף
        updateTextEntries();
        setTurn(this->game.turn());
        updateButtonStates();  // ⬅️ מכבה את הכפתור
    } catch (const std::exception& e) {
        std::cerr << "BlockTax error: " << e.what() << std::endl;
    }


        });





        // === כפתור Skip Tax Block ===
        buttons.emplace_back(font, "Skip Tax Block", sf::Vector2f(300, 490.f), sf::Vector2f(150, 50), [this, &game]() {
            Player* gov = getCurrentPlayer();
            if (!gov || gov->role() != "Governor") return;

            Governor* realGov = dynamic_cast<Governor*>(gov);
            if (!realGov) return;

            realGov->skip_tax_block();
            setTurn(game.turn());
        });



        // === כפתור Reveal Coins – מציג כמה מטבעות יש לשחקן אחר ===
        buttons.emplace_back(font, "Reveal Coins", sf::Vector2f(980, 430.f), sf::Vector2f(150, 50), [this]() {
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* spy = getCurrentPlayer();
            if (!spy || spy->role() != "Spy" || !spy->is_active()) return;

            float x = 100.f;
            float y = 540.f;

            for (Player* target : players) {
                if (target->is_active() && target != spy) {
                    targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, spy, target]() {
                            try {
                                Spy* realSpy = dynamic_cast<Spy*>(spy);
                                if (!realSpy) throw std::runtime_error("Invalid spy cast.");
                                int coins = realSpy->spy_on(*target);
                                std::ostringstream oss;
                                oss << "🕵️ " << target->name() << " has " << coins << " coins.";
                                infoText.setString(oss.str());
                                selectingCoupTarget = false;
                            } catch (const std::exception& e) {
                                std::cerr << "RevealCoins error: " << e.what() << std::endl;
                                selectingCoupTarget = false;
                            }
                        });
                    x += 120;
                }
            }

            if (targetButtons.empty()) selectingCoupTarget = false;
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // === כפתור Block Arrest ===
        buttons.emplace_back(font, "Block Arrest", sf::Vector2f(980, 490.f), sf::Vector2f(150, 50), [this]() {
            selectingCoupTarget = true;
            targetButtons.clear();

            Player* spy = getCurrentPlayer();
            if (!spy || spy->role() != "Spy" || !spy->is_active()) return;

            float x = 100.f;
            float y = 570.f;

            for (Player* target : players) {
                if (target->is_active() && target != spy) {
                    targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, spy, target]() {
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
                        });
                    x += 120;
                }
            }

            if (targetButtons.empty()) selectingCoupTarget = false;
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // === כפתור Invest ===
        buttons.emplace_back(font, "Invest", sf::Vector2f(300, 490.f), sf::Vector2f(150, 50), [this]() {
            Player* current = getCurrentPlayer();
            if (!current || current->role() != "Baron" || !current->is_active()) return;

            try {
                Baron* baron = dynamic_cast<Baron*>(current);
                if (!baron) throw std::runtime_error("Invalid baron cast.");
                baron->invest();
                std::cout << baron->name() << " invested: paid 3 coins, received 6.\n";

                updateTextEntries();
                setTurn(this->game.turn());
                updateButtonStates();
            } catch (const std::exception& e) {
                std::cerr << "Invest error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);

        // === כפתור Block Coup ===
        buttons.emplace_back(font, "Block Coup", sf::Vector2f(640 - 150.f / 2, 550.f), sf::Vector2f(150, 50), [this]() {
            Player* general = getCurrentPlayer();
            try {
                this->game.block_coup(general);
                setTurn(this->game.turn());
                updateButtonStates();
            } catch (const std::exception& e) {
                std::cerr << "BlockCoup error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);


        // === כפתור Skip Coup Block ===
        buttons.emplace_back(font, "Skip Coup Block", sf::Vector2f(640 - 150.f / 2, 610.f), sf::Vector2f(150, 50), [this]() {
            try {
                this->game.skip_coup_block();
                setTurn(this->game.turn());
                updateButtonStates();
            } catch (const std::exception& e) {
                std::cerr << "SkipCoupBlock error: " << e.what() << std::endl;
            }
        });

        buttons.back().setVisible(false);
        buttons.back().setEnabled(false);


    }

    // Sets the list of players and updates their text representations
    void GameRenderer::setPlayers(const std::vector<Player*>& playerList) {
        players = playerList;
        updateTextEntries();
    }

    // Updates the text entries for each player, showing name, role, coins, and status
    void GameRenderer::updateTextEntries() {
        playerTexts.clear();
        float yOffset = 80;

        std::string currentTurn = game.turn();

        for (size_t i = 0; i < players.size(); ++i) {
            std::ostringstream oss;
            std::string coinDisplay;

            if (players[i]->name() == currentTurn) {
                coinDisplay = std::to_string(players[i]->coins());
            } else {
                coinDisplay = "Hidden";
            }

            oss << players[i]->name() << " (" << players[i]->role() << ") - Coins: "
                << coinDisplay << " - Status: " << (players[i]->is_active() ? "Alive" : "Out");

            sf::Text txt(oss.str(), font, 32);
            txt.setFillColor(players[i]->is_active() ? sf::Color::Yellow : sf::Color(150, 0, 0));

            // מרכוז על המסך
            sf::FloatRect bounds = txt.getLocalBounds();
            txt.setPosition(640 - bounds.width / 2, yOffset);
            yOffset += bounds.height + 10;

            playerTexts.push_back(txt);
        }

    }


    // Sets the current turn, updates turn text, and refreshes button states
    void GameRenderer::setTurn(const std::string& name) {
        if (name != current_turn_name) {
            current_turn_name = name;
            updateTextEntries();       // רק עדכון גרפי
            infoText.setString("");
        }
        turnText.setString("Current Turn: " + name);
        updateButtonStates();
    }


    // Returns the player whose turn it is, or nullptr if not found
    Player* GameRenderer::getCurrentPlayer() {
        for (auto* p : players) {
            if (p->name() == game.turn()) return p;
        }
        return nullptr;
    }

    // Draws all UI elements (turn text, player texts, buttons, and info text) to the window
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
        window.draw(infoText);
    }

    // Handles mouse clicks by passing them to the appropriate buttons
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

    // Updates the visibility and enabled state of buttons based on game state and player role
    void GameRenderer::updateButtonStates() {
        Player* current = getCurrentPlayer();
        if (!current) return;
        // אם לשחקן יש 10 מטבעות – הוא חייב לעשות coup
        bool mustCoup = (current->coins() >= 10);
        std::cout << "[Debug] Player " << current->name()
          << " | Coins: " << current->coins()
          << " | Under sanction? " << current->is_sanctioned()
          << " | Gather blocked? " << current->is_gather_blocked()
          << "\n";


        for (auto& btn : buttons) {
            const std::string& label = btn.getLabel();

            // מצב מיוחד: נציב בתור חסימת tax
            if (game.is_awaiting_tax_block()) {
                if (current->role() == "Governor") {
                    bool show = (label == "Block Tax" || label == "Skip Tax Block");
                    btn.setVisible(show);
                    btn.setEnabled(show);
                } else {
                    btn.setVisible(false);  // שאר השחקנים לא רואים כפתורים
                }
                continue;
            }


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
                if (current->role() == "General" && current->coins() >= 5) {
                    bool show = (label == "Block Coup" || label == "Skip Coup Block");
                    btn.setVisible(show);
                    btn.setEnabled(show);
                } else {
                    btn.setVisible(false);  // שאר השחקנים לא רואים כפתורים
                }
                continue;
            }



            // 👇 תיקון חשוב: לא להסתיר כפתורים מיוחדים אם הם במצב רלוונטי
            if ((label == "Block Bribe" || label == "Skip") && !game.is_awaiting_bribe_block()) {
                btn.setVisible(false);
                continue;
            }
            if ((label == "Block Tax" || label == "Skip Tax Block") && !game.is_awaiting_tax_block()) {
                btn.setVisible(false);
                continue;
            }

            if ((label == "Block Coup" || label == "Skip Coup Block") && !game.is_awaiting_coup_block()) {
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
            if (mustCoup) {
                btn.setEnabled(label == "Coup");
                continue;
            }

            if (label == "Gather") {
                btn.setEnabled(!current->is_gather_blocked() && !current->is_sanctioned());
            } else if (label == "Tax") {
                btn.setEnabled(!current->is_sanctioned());
            } else if (label == "Coup") {
                btn.setEnabled(current->coins() >= 7);
            } else if (label == "Bribe") {
                btn.setEnabled(current->coins() >= 4);
            } else if (label == "Sanction")
            {
                btn.setEnabled(current->coins() >= 3);
            }
            else if (label == "Arrest")
            {
                bool blocked = false;
                for (auto* p : players) {
                    if (p->role() == "Spy" && p->is_active()) {
                        Spy* spyChecker = dynamic_cast<Spy*>(p);
                        if (spyChecker && spyChecker->is_arrest_blocked(current)) {
                            blocked = true;
                            break;
                        }
                    }
                }
                btn.setEnabled(!blocked);
            }
            else {
                btn.setEnabled(true);
            }
        }
    }
}