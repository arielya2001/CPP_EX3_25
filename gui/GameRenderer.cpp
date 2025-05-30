/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/  // Email comment for author reference

#include "GameRenderer.hpp"  // Include the header for GameRenderer
#include <iostream>          // For standard I/O operations
#include <sstream>           // For stringstream usage
#include "../Spy.hpp"        // Include the Spy class
#include "../Judge.hpp"      // Include the Judge class
#include "../Governor.hpp"   // Include the Governor class
#include "../Baron.hpp"      // Include the Baron class

namespace coup  // Start of the coup namespace
{

    // Constructor: Initializes the renderer with a font and game, sets up UI elements and action buttons
    GameRenderer::GameRenderer(sf::Font& font, Game& game) : font(font), game(game), current_turn_name("")
    {
        turnText.setFont(font);                      // Set font for the turn display text
        turnText.setCharacterSize(32);               // Set text size for turn display
        turnText.setFillColor(sf::Color::Cyan);      // Set turn text color to cyan
        turnText.setPosition(640 - 150, 20);         // Position the turn text near top-center

        infoText.setFont(font);                      // Set font for the info text
        infoText.setCharacterSize(28);               // Set text size for info display
        infoText.setFillColor(sf::Color::White);     // Set info text color to white
        infoText.setPosition(640 - 300, 600);        // Position info text near the bottom center
        infoText.setString("");                      // Initialize with empty string

        // Smart centering and spacing of main action buttons
        float buttonWidth = 150.f;                   // Width of each action button
        float buttonHeight = 50.f;                   // Height of each action button
        float spacing = 20.f;                        // Space between buttons
        std::vector<std::string> mainActions = { "Gather", "Tax", "Coup", "Bribe", "Arrest", "Sanction" }; // Main actions
        float totalWidth = mainActions.size() * buttonWidth + (mainActions.size() - 1) * spacing; // Total width of all buttons + spacing
        float startX = 640 - totalWidth / 2.0f;      // Start X to center buttons horizontally
        float y = 360.f;                             // Y position for action buttons

        for (size_t i = 0; i < mainActions.size(); ++i) {  // Iterate over each main action
            std::string label = mainActions[i];            // Get current action label
            sf::Vector2f position(startX + i * (buttonWidth + spacing), y);  // Calculate position for each button

            // Lambda to create target selection buttons for actions like Coup or Arrest
            auto createTargetButtons = [this, &label, &game, &font](Player* source, void (Player::*action)(Player&)) {
                selectingCoupTarget = true;                // Mark that we're selecting a target
                targetButtons.clear();                     // Clear any previous target buttons
                float x = 100.f;                           // X position for target buttons
                float y = 500.f;                           // Y position for target buttons
                for (auto* target : players) {             // Iterate over all players
                    if (target->is_active() && target != source) {  // Only add active, non-self targets
                        targetButtons.emplace_back(font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                            [this, source, target, action, &game]() {  // Callback on target selection
                                try {
                                    (source->*action)(*target);       // Perform the action on selected target
                                    updateTextEntries();              // Update action log
                                    setTurn(game.turn());             // Advance turn in UI
                                    updateButtonStates();             // Refresh button enable states
                                } catch (const std::exception& e) {
                                    std::cerr << "Action error: " << e.what() << std::endl;  // Print any errors
                                }
                                selectingCoupTarget = false;         // Stop target selection mode
                            });
                        x += 120;                                   // Move X position for next button
                    }
                }
            };

            if (label == "Gather") {  // Add Gather button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                        p->gather();                   // Perform gather action
                        updateTextEntries();           // Update log
                        setTurn(game.turn());          // Advance turn
                        updateButtonStates();          // Refresh button states
                    }
                });
            } else if (label == "Tax") {  // Add Tax button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    if (auto* p = getCurrentPlayer(); p && p->is_active()) {
                        p->tax();                      // Perform tax action
                        updateTextEntries();           // Update log
                        setTurn(game.turn());          // Advance turn
                        updateButtonStates();          // Refresh button states
                    }
                });
            } else if (label == "Coup") {  // Add Coup button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, createTargetButtons]() {
                    Player* attacker = getCurrentPlayer();
                    if (attacker && attacker->is_active()) {
                        createTargetButtons(attacker, &Player::coup);  // Trigger target selection for coup
                    }
                });
            } else if (label == "Bribe") {  // Add Bribe button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, &game]() {
                    Player* p = getCurrentPlayer();
                    if (!p || !p->is_active()) return;
                    try {
                        p->bribe();                 // Perform bribe action
                        updateTextEntries();        // Update log
                        setTurn(game.turn());       // Advance turn
                        updateButtonStates();       // Refresh button states
                    } catch (const std::exception& e) {
                        std::cerr << "Bribe error: " << e.what() << std::endl;  // Print error if bribe fails
                    }
                });
            } else if (label == "Arrest") {  // Add Arrest button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this, createTargetButtons]() {
                    Player* arrestingPlayer = getCurrentPlayer();
                    if (arrestingPlayer && arrestingPlayer->is_active()) {
                        createTargetButtons(arrestingPlayer, &Player::arrest);  // Target selection for arrest
                    }
                });
            }
            else if (label == "Sanction") {  // Add Sanction button
                buttons.emplace_back(font, label, position, sf::Vector2f(buttonWidth, buttonHeight), [this]() {
                    Player* attacker = getCurrentPlayer();
                    if (!attacker || !attacker->is_active()) return;
                    selectingCoupTarget = true;      // Enable target selection
                    targetButtons.clear();           // Clear previous targets
                    float x = 100.f;                 // Initial X for buttons
                    float y = 540.f;                 // Y position for sanction target buttons
                    for (auto* target : players) {
                        if (target->is_active() && target != attacker) {
                            targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                                [this, attacker, target]() {
                                    try {
                                        attacker->sanction(*target);      // Perform sanction
                                        updateTextEntries();              // Update log
                                        setTurn(this->game.turn());       // Advance turn
                                        updateButtonStates();             // Refresh buttons
                                    } catch (const std::exception& e) {
                                        std::cerr << "Sanction error: " << e.what() << std::endl;  // Print error if sanction fails
                                    }
                                    selectingCoupTarget = false;         // Disable target mode
                                });
                            x += 120;   // Position next target button
                        }
                    }
                });
            }

        } // End of mainActions loop





        // === כפתור Block Bribe ===
        buttons.emplace_back(font, "Block Bribe", sf::Vector2f(640 - 150.f / 2, 430.f), sf::Vector2f(150, 50), [this]() {
            Player* judge = getCurrentPlayer();                                 // Get the current player (expected to be Judge)
            Player* briber = this->game.get_bribing_player();                   // Get the player who performed the bribe
            if (!judge || judge->role() != "Judge" || !briber) return;          // Exit if current player isn't a Judge or no briber

            try {
                auto* realJudge = dynamic_cast<Judge*>(judge);                  // Safely cast to Judge*
                if (!realJudge) throw std::runtime_error("Invalid judge cast."); // Throw error if casting fails
                realJudge->block_bribe(*briber);                                // Call Judge's block_bribe function
                this->game.set_awaiting_bribe_block(false);                     // Mark that no more bribe blocks are awaited
                this->game.set_bribing_player(nullptr);                         // Reset the briber pointer
                setTurn(this->game.turn());                                     // Update the UI turn
            } catch (const std::exception& e) {
                std::cerr << "BlockBribe error: " << e.what() << std::endl;     // Log error if exception occurs
            }
        });
        buttons.back().setVisible(false);                                       // Initially hide the button
        buttons.back().setEnabled(false);                                       // Initially disable the button

        // === כפתור Skip (Bribe) ===
        buttons.emplace_back(font, "Skip", sf::Vector2f(640 - 150.f / 2, 490.f), sf::Vector2f(150, 50), [this, &game]() {
            game.advance_bribe_block_queue();                                   // Advance to the next judge or back to briber
            setTurn(game.turn());                                               // Update the displayed turn
            updateButtonStates();                                               // Refresh button enabled/disabled states
        });
        buttons.back().setVisible(false);                                       // Initially hide the button
        buttons.back().setEnabled(false);                                       // Initially disable the button

        // === כפתור Block Tax ===
        buttons.emplace_back(font, "Block Tax", sf::Vector2f(300, 430.f), sf::Vector2f(150, 50), [this]() {
            Player* governor = getCurrentPlayer();                              // Get the current player (expected to be Governor)
            Player* taxed = this->game.get_tax_target();                        // Get the player who performed tax

            std::cout << "[DEBUG] Block Tax button pressed by: " << (governor ? governor->name() : "null") << std::endl;
            std::cout << "[DEBUG] Taxed target: " << (taxed ? taxed->name() : "null") << std::endl;

            if (!governor || governor->role() != "Governor" || !governor->is_active()) {
                std::cout << "[DEBUG] Governor is invalid or inactive.\n";
                return;                                                         // Exit if governor is invalid
            }

            if (!taxed || !taxed->is_active()) {
                std::cout << "[DEBUG] Taxed player is invalid or inactive.\n";
                return;                                                         // Exit if taxed player is invalid
            }

            std::cout << "[DEBUG] Taxed player's last action: " << taxed->get_last_action() << std::endl;

            if (taxed->get_last_action() != "tax") {
                std::cout << "[DEBUG] Taxed player did not perform tax.\n";
                return;                                                         // Exit if the last action wasn't tax
            }

            try {
                auto* realGov = dynamic_cast<Governor*>(governor);              // Try casting to Governor
                if (!realGov) {
                    std::cerr << "Invalid governor cast.\n";
                    return;                                                     // Exit on failed cast
                }

                std::cout << "[DEBUG] Calling realGov->block_tax()\n";
                realGov->block_tax(*taxed);                                     // Call Governor's block_tax method

                this->game.set_awaiting_tax_block(false);                       // Stop waiting for tax blocks
                updateTextEntries();                                            // Update log entries
                setTurn(this->game.turn());                                     // Update turn
                updateButtonStates();                                           // Refresh UI button states
            } catch (const std::exception& e) {
                std::cerr << "BlockTax error: " << e.what() << std::endl;       // Print error if something goes wrong
            }
        });

        // === כפתור Skip Tax Block ===
        buttons.emplace_back(font, "Skip Tax Block", sf::Vector2f(300, 490.f), sf::Vector2f(150, 50), [this, &game]() {
            Player* gov = getCurrentPlayer();                                   // Get current player
            if (!gov || gov->role() != "Governor") return;                      // Ensure the current player is a Governor

            Governor* realGov = dynamic_cast<Governor*>(gov);                   // Attempt to cast to Governor*
            if (!realGov) return;                                               // Exit if cast fails

            realGov->skip_tax_block();                                          // Call skip_tax_block on the Governor
            setTurn(game.turn());                                               // Update the UI turn
        });


        // === Reveal Coins Button – Spy reveals coin count of another player ===
        buttons.emplace_back(font, "Reveal Coins", sf::Vector2f(980, 430.f), sf::Vector2f(150, 50), [this]() {
            selectingCoupTarget = true;                                        // Start target selection mode
            targetButtons.clear();                                             // Clear existing target buttons

            Player* spy = getCurrentPlayer();                                  // Get the current player
            if (!spy || spy->role() != "Spy" || !spy->is_active()) return;     // Ensure player is an active Spy

            float x = 100.f;
            float y = 540.f;

            for (Player* target : players) {
                if (target->is_active() && target != spy) {
                    targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, spy, target]() {
                            try {
                                Spy* realSpy = dynamic_cast<Spy*>(spy);        // Cast to Spy*
                                if (!realSpy) throw std::runtime_error("Invalid spy cast.");
                                int coins = realSpy->spy_on(*target);          // Call spy_on to reveal coin count
                                std::ostringstream oss;
                                oss << "🕵️ " << target->name() << " has " << coins << " coins.";
                                infoText.setString(oss.str());                 // Display result in info text
                                selectingCoupTarget = false;                   // Exit selection mode
                            } catch (const std::exception& e) {
                                std::cerr << "RevealCoins error: " << e.what() << std::endl;
                                selectingCoupTarget = false;
                            }
                        });
                    x += 120;
                }
            }

            if (targetButtons.empty()) selectingCoupTarget = false;            // Exit mode if no targets
        });
        buttons.back().setVisible(false);                                      // Initially hidden
        buttons.back().setEnabled(false);                                      // Initially disabled

        // === Block Arrest Button – Spy can block another player's next arrest ===
        buttons.emplace_back(font, "Block Arrest", sf::Vector2f(980, 490.f), sf::Vector2f(150, 50), [this]() {
            selectingCoupTarget = true;                                        // Start target selection
            targetButtons.clear();                                             // Clear buttons

            Player* spy = getCurrentPlayer();                                  // Get current player
            if (!spy || spy->role() != "Spy" || !spy->is_active()) return;     // Check active Spy

            float x = 100.f;
            float y = 570.f;

            for (Player* target : players) {
                if (target->is_active() && target != spy) {
                    targetButtons.emplace_back(this->font, target->name(), sf::Vector2f(x, y), sf::Vector2f(100, 30),
                        [this, spy, target]() {
                            try {
                                Spy* realSpy = dynamic_cast<Spy*>(spy);        // Cast to Spy*
                                if (!realSpy) throw std::runtime_error("Invalid spy cast.");
                                realSpy->block_arrest(*target);                // Block arrest from this target
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

            if (targetButtons.empty()) selectingCoupTarget = false;            // Exit if no valid targets
        });
        buttons.back().setVisible(false);                                      // Initially hidden
        buttons.back().setEnabled(false);                                      // Initially disabled

        // === Invest Button – Baron invests coins for double the return ===
        buttons.emplace_back(font, "Invest", sf::Vector2f(300, 490.f), sf::Vector2f(150, 50), [this]() {
            Player* current = getCurrentPlayer();                              // Get current player
            if (!current || current->role() != "Baron" || !current->is_active()) return;

            try {
                Baron* baron = dynamic_cast<Baron*>(current);                  // Cast to Baron*
                if (!baron) throw std::runtime_error("Invalid baron cast.");
                baron->invest();                                               // Call invest (pay 3, get 6)
                std::cout << baron->name() << " invested: paid 3 coins, received 6.\n";

                updateTextEntries();                                           // Update log
                setTurn(this->game.turn());                                    // Update turn
                updateButtonStates();                                          // Refresh buttons
            } catch (const std::exception& e) {
                std::cerr << "Invest error: " << e.what() << std::endl;
            }
        });
        buttons.back().setVisible(false);                                      // Hidden initially
        buttons.back().setEnabled(false);                                      // Disabled initially

        // === Block Coup Button – General blocks a coup ===
        buttons.emplace_back(font, "Block Coup", sf::Vector2f(640 - 150.f / 2, 550.f), sf::Vector2f(150, 50), [this]() {
            Player* general = getCurrentPlayer();                              // Get current player
            try {
                this->game.block_coup(general);                                // Let General block a coup
                setTurn(this->game.turn());                                    // Update the turn
                updateTextEntries();                                           // Update action log
                updateButtonStates();                                          // Update available buttons
            } catch (const std::exception& e) {
                std::cerr << "BlockCoup error: " << e.what() << std::endl;     // Log any errors
            }
        });
        buttons.back().setVisible(false);                                      // Hidden by default
        buttons.back().setEnabled(false);                                      // Disabled by default

        // === Skip Coup Block Button – Skip blocking a coup ===
        buttons.emplace_back(font, "Skip Coup Block", sf::Vector2f(640 - 150.f / 2, 610.f), sf::Vector2f(150, 50), [this]() {
            try {
                this->game.skip_coup_block();                                  // Skip blocking the coup
                setTurn(this->game.turn());                                    // Update turn
                updateButtonStates();                                          // Refresh buttons
            } catch (const std::exception& e) {
                std::cerr << "SkipCoupBlock error: " << e.what() << std::endl; // Log error
            }
        });
        buttons.back().setVisible(false);                                      // Start hidden
        buttons.back().setEnabled(false);                                      // Start disabled
    }

    // Sets the list of players and updates their text representations
    void GameRenderer::setPlayers(const std::vector<Player*>& playerList) {
        players = playerList;
        updateTextEntries(); // Refresh player status display
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
                coinDisplay = std::to_string(players[i]->coins()); // Show coin count only for current player
            } else {
                coinDisplay = "Hidden";
            }

            oss << players[i]->name() << " (" << players[i]->role() << ") - Coins: "
                << coinDisplay << " - Status: " << (players[i]->is_active() ? "Alive" : "Out");

            sf::Text txt(oss.str(), font, 32);
            txt.setFillColor(players[i]->is_active() ? sf::Color::Yellow : sf::Color(150, 0, 0)); // Red if out

            // Center text horizontally on screen
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
            updateTextEntries();       // Refresh player info display
            infoText.setString("");    // Clear action info
        }
        turnText.setString("Current Turn: " + name);
        updateButtonStates();          // Enable/disable buttons based on new turn
    }



    // Returns the player whose turn it is, or nullptr if not found
    Player* GameRenderer::getCurrentPlayer() {                      // Get the player whose name matches the current turn
        for (auto* p : players) {                                   // Iterate over all players
            if (p->name() == game.turn()) return p;                // Return the player if their name matches the game's current turn
        }
        return nullptr;                                             // Return nullptr if no match found
    }


    // Draws all UI elements (turn text, player texts, buttons, and info text) to the window
    void GameRenderer::draw(sf::RenderWindow& window) {             // Render all visible elements to the given window
        window.draw(turnText);                                      // Draw the text showing whose turn it is
        for (auto& txt : playerTexts) {                             // Loop through all player status texts
            window.draw(txt);                                       // Draw each player's status
        }
        for (auto& btn : buttons) {                                 // Loop through all main buttons
            btn.draw(window);                                       // Draw each action button
        }
        if (selectingCoupTarget) {                                  // If the game is currently asking the user to select a target
            for (auto& tbtn : targetButtons) {                      // Loop through all target selection buttons
                tbtn.draw(window);                                  // Draw each target button
            }
        }
        window.draw(infoText);                                      // Draw the info box (e.g., messages or results of actions)
    }



    // Handles mouse clicks by passing them to the appropriate buttons
void GameRenderer::handleClick(sf::Vector2f pos) {                         // Handles mouse click at the given position
    if (selectingCoupTarget) {                                            // If the game is in a state where a target must be selected
        for (auto& tbtn : targetButtons) {                                // Iterate over all target buttons
            tbtn.handleClick(pos);                                        // Pass the click to each target button
        }
    } else {                                                              // If not selecting a target
        for (auto& btn : buttons) {                                       // Iterate over main action buttons
            btn.handleClick(pos);                                         // Pass the click to each button
        }
    }
}

// Updates the visibility and enabled state of buttons based on game state and player role
void GameRenderer::updateButtonStates() {                                 // Refresh button states according to game logic
    Player* current = getCurrentPlayer();                                 // Get the current player
    if (!current) return;                                                 // Exit if no current player

    // If in coup block phase, only Generals with 5+ coins see Block/Skip buttons
    if (game.is_awaiting_coup_block()) {                                  // Check if the game is waiting for a coup block
        for (auto& btn : buttons) {                                       // Iterate over buttons
            const std::string& label = btn.getLabel();                    // Get button label
            bool isGeneral = current->role() == "General" && current->coins() >= 5; // Check if player is a valid general
            bool show = isGeneral && (label == "Block Coup" || label == "Skip Coup Block"); // Show only relevant buttons
            btn.setVisible(show);                                         // Set visibility
            btn.setEnabled(show);                                         // Set enabled state
        }
        return;                                                           // Return early to avoid overriding settings
    }

    // If player has 10+ coins, they must coup
    bool mustCoup = (current->coins() >= 10);                             // Check if the player must coup
    std::cout << "[Debug] Player " << current->name()                     // Print debug information
              << " | Coins: " << current->coins()
              << " | Under sanction? " << current->is_sanctioned()
              << " | Gather blocked? " << current->is_gather_blocked()
              << "\n";

    for (auto& btn : buttons) {                                           // Iterate over all buttons
        const std::string& label = btn.getLabel();                        // Get label of button

        // Handle must-coup scenario
        if (mustCoup) {                                                   // If player must coup
            bool show = (label == "Coup");                                // Only Coup button should be visible
            btn.setVisible(show);                                         // Set visibility
            btn.setEnabled(show);                                         // Set enabled state
            continue;                                                     // Move to next button
        }

        // Handle tax block phase
        if (game.is_awaiting_tax_block()) {                               // If waiting for tax block
            bool show = current->role() == "Governor" && (label == "Block Tax" || label == "Skip Tax Block");
            btn.setVisible(show);                                         // Show/hide accordingly
            btn.setEnabled(show);                                         // Enable/disable accordingly
            continue;
        }

        // Handle bribe block phase
        if (game.is_awaiting_bribe_block()) {                             // If waiting for bribe block
            bool show = current->role() == "Judge" && (label == "Block Bribe" || label == "Skip");
            btn.setVisible(show);                                         // Show/hide accordingly
            btn.setEnabled(show);                                         // Enable/disable accordingly
            continue;
        }

        // Hide block/skip buttons if not in their respective phases
        if ((label == "Block Bribe" || label == "Skip") && !game.is_awaiting_bribe_block()) {
            btn.setVisible(false);                                        // Hide button
            continue;
        }
        if ((label == "Block Tax" || label == "Skip Tax Block") && !game.is_awaiting_tax_block()) {
            btn.setVisible(false);                                        // Hide button
            continue;
        }
        if ((label == "Block Coup" || label == "Skip Coup Block") && !game.is_awaiting_coup_block()) {
            btn.setVisible(false);                                        // Hide button
            continue;
        }

        // Role-specific buttons
        if (label == "Block Tax") {
            bool isGovernor = current->role() == "Governor";              // Check if current player is Governor
            btn.setVisible(isGovernor);                                   // Show only to Governor
            btn.setEnabled(isGovernor);                                   // Enable only for Governor
            continue;
        }

        if (label == "Reveal Coins" || label == "Block Arrest") {
            bool isSpy = current->role() == "Spy";                        // Check if current player is Spy
            btn.setVisible(isSpy);                                        // Show only to Spy
            btn.setEnabled(isSpy);                                        // Enable only for Spy
            continue;
        }

        if (label == "Invest") {
            bool isBaron = current->role() == "Baron" && current->coins() >= 3; // Check if Baron with 3+ coins
            btn.setVisible(isBaron);                                      // Show if valid
            btn.setEnabled(isBaron);                                      // Enable if valid
            continue;
        }

        // Regular action buttons
        btn.setVisible(true);                                             // Default: show all other buttons

        if (label == "Gather") {
            btn.setEnabled(!current->is_gather_blocked() && !current->is_sanctioned()); // Enable if not blocked or sanctioned
        } else if (label == "Tax") {
            btn.setEnabled(!current->is_sanctioned());                    // Enable if not sanctioned
        } else if (label == "Coup") {
            btn.setEnabled(current->coins() >= 7);                        // Enable if player has at least 7 coins
        } else if (label == "Bribe") {
            btn.setEnabled(current->coins() >= 4);                        // Enable if player has at least 4 coins
        } else if (label == "Sanction") {
            btn.setEnabled(current->coins() >= 3);                        // Enable if player has at least 3 coins
        } else if (label == "Arrest") {
            bool blocked = false;                                         // Check if player is blocked by a Spy
            for (auto* p : players) {                                     // Loop through all players
                if (p->role() == "Spy" && p->is_active()) {               // Check if player is an active Spy
                    Spy* spyChecker = dynamic_cast<Spy*>(p);              // Cast to Spy*
                    if (spyChecker && spyChecker->is_arrest_blocked(current)) {
                        blocked = true;                                   // Mark as blocked
                        break;
                    }
                }
            }
            btn.setEnabled(!blocked);                                     // Enable only if not blocked
        } else {
            btn.setEnabled(true);                                         // Default: enable button
        }
    }
}
}