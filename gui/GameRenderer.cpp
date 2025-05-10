#include "GameRenderer.hpp"
#include <iostream>
#include <sstream>

namespace coup {

GameRenderer::GameRenderer(sf::Font& font, Game& game) : font(font), game(game), current_turn_name("") {
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


    buttons.emplace_back(font, "Block Coup", sf::Vector2f(500, 490), sf::Vector2f(120, 35), [this, &game]() {
        Player* general = getCurrentPlayer();
        Player* attacker = game.get_coup_attacker();

        if (!general || general->role() != "General" || !attacker) return;

        std::cout << general->name() << " blocked the coup from " << attacker->name() << std::endl;

        game.set_awaiting_coup_block(false);

        // מצא את השחקן הפעיל הבא אחרי התוקף
        int attackerIndex = game.get_player_index(attacker);
        const std::vector<Player*>& players = game.get_all_players();

        size_t nextIndex = (attackerIndex + 1) % players.size();
        while (!players[nextIndex]->is_active()) {
            nextIndex = (nextIndex + 1) % players.size();
        }

        game.set_turn_to(players[nextIndex]);

        game.set_coup_attacker(nullptr);
        game.set_coup_target(nullptr);

        setTurn(game.turn());
    });
    buttons.back().setVisible(false);
    buttons.back().setEnabled(false);


    buttons.emplace_back(font, "Skip Coup Block", sf::Vector2f(640, 490), sf::Vector2f(160, 35), [this, &game]() {
        Player* attacker = game.get_coup_attacker();
        Player* target = game.get_coup_target();

        if (!attacker || !target) return;

        std::cout << "General skipped coup block.\n";

        // מבצעים את ההפיכה כרגיל
        target->set_couped(true);
        target->deactivate();
        updateTextEntries();  // 👈 זה מרענן את רשימת השחקנים


        std::cout << attacker->name() << " performed coup on " << target->name() << std::endl;

        game.set_awaiting_coup_block(false);
        game.set_coup_attacker(nullptr);
        game.set_coup_target(nullptr);

        // עוברים לתור שאחרי התוקף
        int attackerIndex = game.get_player_index(attacker);
        do {
            attackerIndex = (attackerIndex + 1) % game.num_players();
        } while (!game.get_all_players()[attackerIndex]->is_active());
        game.set_turn_to(game.get_all_players()[attackerIndex]);

        setTurn(game.turn());
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

    // אם יש מנצח
    try {
        std::string winnerName = game.winner();
        sf::Text winText("🏆 Winner: " + winnerName, font, 36);
        winText.setFillColor(sf::Color::Green);
        winText.setPosition(100, 140 + players.size() * 30 + 20);
        playerTexts.push_back(winText);

        // הצג popup אם השחקן הנוכחי הוא המנצח
        if (getCurrentPlayer() && getCurrentPlayer()->name() == winnerName) {
            sf::RenderWindow popup(sf::VideoMode(400, 200), "Victory!");
            sf::Text text("🏆 Winner: " + winnerName, font, 36);
            text.setFillColor(sf::Color::Green);
            text.setPosition(70, 80);

            sf::Clock clock;
            while (popup.isOpen()) {
                sf::Event event;
                while (popup.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        popup.close();
                }

                popup.clear(sf::Color::Black);
                popup.draw(text);
                popup.display();

                if (clock.getElapsedTime().asSeconds() > 3) {
                    popup.close();  // סגור אוטומטית אחרי 3 שניות
                }
            }

            exit(0);  // סיים את המשחק
        }
    } catch (const std::exception&) {
        // אין מנצח עדיין
    }
}



    void GameRenderer::setTurn(const std::string& name) {
    if (name != current_turn_name) { // קרא ל-on_turn_start רק אם התור השתנה
        Player* current = getCurrentPlayer();
        if (current) {
            current->on_turn_start();
        }
        current_turn_name = name;
    }
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
    // אם לשחקן יש 10 מטבעות – הוא חייב לעשות coup
    bool mustCoup = (current->coins() >= 10);
        std::cout << "[Debug] Player " << current->name()
          << " | Coins: " << current->coins()
          << " | Under sanction? " << current->is_sanctioned()
          << " | Gather blocked? " << current->is_gather_blocked()
          << "\n";


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
            if (current->role() == "General" && current->coins() >= 5) {
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
            // חייב לבצע coup – כל הכפתורים מושבתים חוץ מ־Coup
            if (label == "Coup") {
                btn.setEnabled(true);
            } else {
                btn.setEnabled(false);
            }
            continue;
        }
        if (label == "Gather") {
            std::cout << "[Debug] Gather button enabled: " << (!current->is_gather_blocked() && !current->is_sanctioned()) << "\n";
            btn.setEnabled(!current->is_gather_blocked() && !current->is_sanctioned());
        } else if (label == "Tax") {
            std::cout << "[Debug] Tax button enabled: " << (!current->is_sanctioned()) << "\n";
            btn.setEnabled(!current->is_sanctioned());
        }
        else if (label == "Coup") {
            btn.setEnabled(current->coins() >= 7);
        } else {
            btn.setEnabled(true);
        }
    }
}










} // namespace coup