/*
Mail - ariel.yaacobi@msmail.ariel.ac.il
*/
#include <SFML/Graphics.hpp>                  // SFML library for graphics rendering
#include <iostream>                           // Standard I/O library
#include <vector>                             // Vector container from STL
#include <string>                             // String class from STL
#include <random>                             // Random number utilities
#include "../Game.hpp"                        // Game logic and state declarations
#include "../Player.hpp"                      // Player class
#include "../Governor.hpp"                    // Governor role implementation
#include "../Spy.hpp"                         // Spy role implementation
#include "../Baron.hpp"                       // Baron role implementation
#include "../General.hpp"                     // General role implementation
#include "../Judge.hpp"                       // Judge role implementation
#include "../Merchant.hpp"                    // Merchant role implementation
#include "GuiScreens.hpp"                     // GUI-related screen utilities
#include "GameRenderer.hpp"                   // Responsible for rendering the game to the window

namespace coup                                    // Define everything inside the coup namespace
{
    //Displays a popup window to select a player role and returns the chosen role
    std::string popupSelectRole(sf::RenderWindow& parentWindow, sf::Font& font) {  // Function to show popup for role selection
        sf::RenderWindow popup(sf::VideoMode(600, 400), "Choose Role", sf::Style::Titlebar | sf::Style::Close); // Create popup window

        std::vector<std::string> roles = {       // List of available player roles
            "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
        };

        sf::Text title("Choose Role", font, 36); // Title text configuration
        title.setFillColor(sf::Color::White);    // Set title text color
        title.setPosition(180, 20);              // Set title text position on screen

        while (popup.isOpen()) {                 // Main loop: runs while popup window is open
            sf::Event event;                     // Declare event variable to capture user actions
            while (popup.pollEvent(event)) {     // Poll events (e.g., mouse clicks, window close)
                if (event.type == sf::Event::Closed) return ""; // If window closed, return empty string

                if (event.type == sf::Event::MouseButtonPressed) { // If mouse is pressed
                    sf::Vector2i mousePos = sf::Mouse::getPosition(popup); // Get mouse position relative to popup
                    for (size_t i = 0; i < roles.size(); ++i) {            // Loop through roles
                        sf::FloatRect box(100, 80 + i * 50, 400, 40);      // Define clickable box for each role
                        if (box.contains((float)mousePos.x, (float)mousePos.y)) { // Check if mouse is inside a box
                            popup.close();         // Close popup
                            return roles[i];       // Return the selected role
                        }
                    }
                }
            }

            popup.clear(sf::Color(40, 40, 40));     // Clear popup window with dark gray background
            popup.draw(title);                     // Draw title on screen

            for (size_t i = 0; i < roles.size(); ++i) { // Draw each role as a clickable box
                sf::RectangleShape box(sf::Vector2f(400, 40));       // Create rectangle shape for role button
                box.setFillColor(sf::Color(70, 120, 200));           // Set button color
                box.setPosition(100, 80 + i * 50);                   // Set button position
                popup.draw(box);                                     // Draw the button

                sf::Text roleText(roles[i], font, 28);               // Create text for the role label
                roleText.setFillColor(sf::Color::White);             // Set text color
                roleText.setPosition(120, 85 + i * 50);              // Set text position
                popup.draw(roleText);                                // Draw the text
            }

            popup.display(); // Show everything that was drawn in this frame
        }

        return ""; // If popup was closed without selection, return empty string
    }


    // Displays a popup window to select a target player and returns the chosen player's name
std::string popupSelectTarget(sf::RenderWindow& parentWindow, const std::vector<Player*>& players, sf::Font& font) { // Function for selecting a target player via popup
    sf::RenderWindow popup(sf::VideoMode(600, 400), "Select Target", sf::Style::Titlebar | sf::Style::Close);       // Create a new popup window for selecting a target
    sf::Text title("Select a Player", font, 36);            // Create title text
    title.setFillColor(sf::Color::White);                   // Set title text color to white
    title.setPosition(180, 20);                             // Set title text position

    while (popup.isOpen()) {                                // Main event loop for popup
        sf::Event event;                                     // Declare an SFML event
        while (popup.pollEvent(event)) {                    // Poll all window events
            if (event.type == sf::Event::Closed) return ""; // If window is closed, return empty string
            if (event.type == sf::Event::MouseButtonPressed) { // If mouse was clicked
                sf::Vector2i mousePos = sf::Mouse::getPosition(popup); // Get mouse position relative to popup
                for (size_t i = 0; i < players.size(); ++i) {          // Loop through players
                    sf::FloatRect box(100, 80 + i * 50, 400, 40);      // Create clickable box for each player
                    if (box.contains((float)mousePos.x, (float)mousePos.y)) { // If mouse is inside box
                        popup.close();                    // Close popup window
                        return players[i]->name();        // Return the selected player's name
                    }
                }
            }
        }
        popup.clear(sf::Color(50, 50, 50));                // Clear popup with background color
        popup.draw(title);                                 // Draw title text

        for (size_t i = 0; i < players.size(); ++i) {      // Loop to draw player boxes
            sf::RectangleShape box(sf::Vector2f(400, 40)); // Create a rectangle for each player
            box.setFillColor(sf::Color(100, 100, 200));    // Set box color
            box.setPosition(100, 80 + i * 50);             // Set box position
            popup.draw(box);                               // Draw the box

            sf::Text name(players[i]->name(), font, 28);   // Create text for player name
            name.setFillColor(sf::Color::White);           // Set text color
            name.setPosition(120, 85 + i * 50);            // Set text position
            popup.draw(name);                              // Draw the player name
        }
        popup.display();                                   // Display the updated frame
    }
    return "";                                             // If popup closed without selection, return empty string
}


    // Displays a yes/no popup with a given question and returns true for "Yes" or false for "No"
bool popupYesNo(sf::RenderWindow& parentWindow, sf::Font& font, const std::string& question) { // Function to ask a Yes/No question in a popup
    sf::RenderWindow popup(sf::VideoMode(500, 200), "Need a target?", sf::Style::Titlebar | sf::Style::Close); // Create popup window
    sf::Text questionText(question, font, 28);              // Create text with the question
    questionText.setFillColor(sf::Color::White);            // Set question text color to white
    questionText.setPosition(50, 50);                       // Set position of question text

    sf::RectangleShape yesButton(sf::Vector2f(100, 50));    // Create green "Yes" button
    yesButton.setFillColor(sf::Color::Green);               // Set fill color to green
    yesButton.setPosition(70, 120);                         // Set position of "Yes" button

    sf::Text yesText("Yes", font, 24);                      // Create text for "Yes"
    yesText.setFillColor(sf::Color::Black);                 // Set text color to black
    yesText.setPosition(95, 135);                           // Set position of "Yes" text

    sf::RectangleShape noButton(sf::Vector2f(100, 50));     // Create red "No" button
    noButton.setFillColor(sf::Color::Red);                  // Set fill color to red
    noButton.setPosition(230, 120);                         // Set position of "No" button

    sf::Text noText("No", font, 24);                        // Create text for "No"
    noText.setFillColor(sf::Color::Black);                  // Set text color to black
    noText.setPosition(265, 135);                           // Set position of "No" text

    while (popup.isOpen()) {                                // Event loop for popup
        sf::Event event;                                     // Declare event
        while (popup.pollEvent(event)) {                    // Poll events
            if (event.type == sf::Event::Closed) return false; // If closed, return false
            if (event.type == sf::Event::MouseButtonPressed) { // On mouse click
                sf::Vector2i mousePos = sf::Mouse::getPosition(popup); // Get mouse position
                if (yesButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) { // Check if clicked on "Yes"
                    popup.close();                          // Close popup
                    return true;                            // Return true for "Yes"
                }
                if (noButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {  // Check if clicked on "No"
                    popup.close();                          // Close popup
                    return false;                           // Return false for "No"
                }
            }
        }
        popup.clear(sf::Color(50, 50, 50));                 // Clear popup with background color
        popup.draw(questionText);                           // Draw question
        popup.draw(yesButton);                              // Draw "Yes" button
        popup.draw(yesText);                                // Draw "Yes" text
        popup.draw(noButton);                               // Draw "No" button
        popup.draw(noText);                                 // Draw "No" text
        popup.display();                                    // Display updated frame
    }
    return false;                                           // Default return if window closes
}

// Runs the opening screen with a start button, returning true if the game should start
bool runOpeningScreen() {                                   // Function to display the opening screen
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game"); // Create game window

    sf::Texture backgroundTexture;                          // Declare texture for background
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) { // Load background image
        std::cerr << "Failed to load opening background image" << std::endl; // Error message
        return false;                                       // Return false on failure
    }
    sf::Sprite background(backgroundTexture);               // Create sprite from texture
    background.setScale(                                   // Scale background to fit window
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Font font;                                          // Declare font
    if (!font.loadFromFile("resources/font.ttf")) {         // Load font file
        std::cerr << "Failed to load font.ttf" << std::endl; // Error message
        return false;                                       // Return false on failure
    }

    sf::Vector2f buttonSize(320, 70);                       // Define size for start button
    sf::RectangleShape startButton(buttonSize);             // Create start button
    startButton.setFillColor(sf::Color::Black);             // Set fill color to black
    startButton.setOutlineColor(sf::Color(255, 215, 0));    // Set outline color to gold
    startButton.setOutlineThickness(4);                     // Set outline thickness
    startButton.setPosition(                               // Position the start button
        (window.getSize().x - buttonSize.x) / 2,
        window.getSize().y - 140
    );

    sf::Text startText("Let the Games Begin", font, 34);    // Create start button text
    startText.setFillColor(sf::Color(255, 215, 0));         // Set text color to gold
    sf::FloatRect textBounds = startText.getLocalBounds(); // Get bounds for centering text
    startText.setPosition(                                 // Center the text on the button
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {                               // Main loop for window
        sf::Event event;                                     // Declare event
        while (window.pollEvent(event)) {                   // Poll events
            if (event.type == sf::Event::Closed) {          // If window closed
                window.close();                             // Close window
                return false;                               // Return false
            }

            if (event.type == sf::Event::MouseButtonPressed && // If mouse click
                event.mouseButton.button == sf::Mouse::Left) { // Left click
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window); // Get mouse position
                if (startButton.getGlobalBounds().contains(mousePos)) { // Check if start clicked
                    window.close();                         // Close window
                    return true;                            // Return true to start game
                }
            }
        }

        // Hover effect
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window); // Get current mouse position
        if (startButton.getGlobalBounds().contains(mousePos)) { // If mouse over button
            startButton.setOutlineColor(sf::Color::White);      // Highlight button outline
            startText.setFillColor(sf::Color::White);           // Highlight text color
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0)); // Reset to gold
            startText.setFillColor(sf::Color(255, 215, 0));      // Reset to gold
        }

        window.clear();                                  // Clear window for redraw
        window.draw(background);                         // Draw background
        window.draw(startButton);                        // Draw start button
        window.draw(startText);                          // Draw start text
        window.display();                                // Display the frame
    }

    return true;                                         // Default return true if window closes naturally
}

   // Runs the screen for adding players, allowing name input and returning true if players are added successfully
bool runAddPlayersScreen(Game& game) {                                   // Function to run the player-adding screen
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Add Players");    // Create window with given resolution and title

    sf::Texture backgroundTexture;                                       // Declare background texture
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) { // Try loading background image
        std::cerr << "Failed to load opening background image" << std::endl; // Log error if failed
        return false;                                                    // Return failure
    }
    sf::Sprite background(backgroundTexture);                            // Create sprite with background texture
    background.setScale(                                                 // Scale the background to fit the window
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Font font;                                                       // Declare font
    if (!font.loadFromFile("resources/font.ttf")) {                      // Try loading font
        std::cerr << "Failed to load font\n";                            // Log error if failed
        return false;                                                    // Return failure
    }

    std::vector<std::string> playerNames;                                // Store names of players
    std::string currentInput;                                            // Current input string from user

    sf::Text title("Add Players", font, 64);                             // Create title text
    title.setFillColor(sf::Color::White);                                // Set title color
    title.setPosition(640 - title.getLocalBounds().width / 2, 30);       // Center the title horizontally

    sf::Text instruction("(type name + press ENTER)", font, 36);         // Instruction text
    instruction.setFillColor(sf::Color::White);                          // Set instruction color
    instruction.setPosition(640 - instruction.getLocalBounds().width / 2, 110); // Center instruction

    sf::Text inputText("", font, 30);                                    // Text for current input
    inputText.setFillColor(sf::Color::Black);                            // Set input text color
    sf::RectangleShape inputBox(sf::Vector2f(500, 50));                  // Create input box
    inputBox.setFillColor(sf::Color::White);                             // Set input box color
    inputBox.setPosition(390, 170);                                      // Set input box position
    inputText.setPosition(400, 180);                                     // Set input text position

    sf::Text playersLabel("Players:", font, 40);                         // Label for player list
    playersLabel.setFillColor(sf::Color::Yellow);                        // Set label color
    playersLabel.setPosition(390, 250);                                  // Set label position

    // Use consistent button size with opening screen
    sf::Vector2f buttonSize(310, 65);                                    // Size for the "Start" button
    sf::RectangleShape startButton(buttonSize);                          // Create "Start" button
    startButton.setFillColor(sf::Color::Black);                          // Set fill color
    startButton.setOutlineColor(sf::Color(255, 215, 0));                 // Set gold outline
    startButton.setOutlineThickness(4);                                  // Set outline thickness
    startButton.setPosition((1280 - buttonSize.x) / 2, 600);             // Center button horizontally

    sf::Text startText("Gather the Players", font, 34);                  // Button text
    startText.setFillColor(sf::Color(255, 215, 0));                      // Set text color
    sf::FloatRect textBounds = startText.getLocalBounds();              // Get bounds for centering
    startText.setPosition(                                             // Center text in button
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {                                            // Main window loop
        sf::Event event;                                                 // Declare event
        while (window.pollEvent(event)) {                                // Poll for events
            if (event.type == sf::Event::Closed) {                       // If window closed
                window.close();                                          // Close it
                return false;                                            // Return false
            }

            if (event.type == sf::Event::TextEntered) {                  // Handle text input
                if (event.text.unicode == '\b' && !currentInput.empty()) { // Handle backspace
                    currentInput.pop_back();                             // Remove last character
                } else if (event.text.unicode == '\r') {                 // Handle Enter key
                    if (!currentInput.empty() && playerNames.size() < 6 &&
                        std::find(playerNames.begin(), playerNames.end(), currentInput) == playerNames.end()) {
                        playerNames.push_back(currentInput);             // Add name if valid
                    }
                    currentInput.clear();                                // Clear input
                } else if (event.text.unicode < 128) {                   // Add typed character
                    currentInput += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window); // Get mouse position
                if (startButton.getGlobalBounds().contains(mousePos)) {  // If start button clicked
                    if (playerNames.size() >= 2 && playerNames.size() <= 6) {
                        for (const auto& name : playerNames) {
                            game.add_player(new Player(game, name, "")); // Add each player to game
                        }
                        window.close();                                   // Close the window
                    }
                }
                float listStartY = 320;                                   // Starting Y position for player list
                for (size_t i = 0; i < playerNames.size(); ++i) {
                    sf::FloatRect removeBox(700, listStartY + i * 45, 30, 30); // Area for remove button
                    if (removeBox.contains(mousePos)) {
                        playerNames.erase(playerNames.begin() + i);      // Remove player on click
                        break;
                    }
                }
            }
        }

        // Hover effect
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window); // Get mouse position
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setOutlineColor(sf::Color::White);               // Highlight outline
            startText.setFillColor(sf::Color::White);                    // Highlight text
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0));         // Reset outline
            startText.setFillColor(sf::Color(255, 215, 0));              // Reset text
        }

        inputText.setString(currentInput);                               // Update input text

        window.clear();                                                  // Clear window
        window.draw(background);                                         // Draw background
        window.draw(title);                                              // Draw title
        window.draw(instruction);                                        // Draw instructions
        window.draw(inputBox);                                           // Draw input box
        window.draw(inputText);                                          // Draw input text
        window.draw(playersLabel);                                       // Draw label

        float listStartY = 320;                                          // Y position for player list
        for (size_t i = 0; i < playerNames.size(); ++i) {
            sf::Text playerText("- " + playerNames[i], font, 32);        // Player name text
            playerText.setFillColor(sf::Color::White);                   // Set color
            playerText.setPosition(390, listStartY + i * 45);            // Set position
            window.draw(playerText);                                     // Draw name

            sf::RectangleShape removeBox(sf::Vector2f(30, 30));          // Create red "X" button
            removeBox.setFillColor(sf::Color::Red);                      // Set color
            removeBox.setPosition(700, listStartY + i * 45);             // Set position
            window.draw(removeBox);                                      // Draw box

            sf::Text xText("X", font, 22);                               // Create "X" label
            xText.setFillColor(sf::Color::White);                        // Set text color
            xText.setPosition(710, listStartY + i * 45);                 // Set position
            window.draw(xText);                                          // Draw text
        }

        window.draw(startButton);                                        // Draw "Start" button
        window.draw(startText);                                          // Draw button text
        window.display();                                                // Display all
    }

    return true;                                                         // Return success if finished
}


   // Runs the screen for assigning roles to players, allowing role selection and returning true on completion
bool runAssignRolesScreen(Game& game) {                                    // Function to run the role assignment screen
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Assign Roles");     // Create main window for the role assignment

    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {                        // Load font for text rendering
        std::cerr << "Failed to load font.ttf" << std::endl;
        return false;                                                      // Exit if font fails
    }

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/opening_screen.png")) { // Load background image
        std::cerr << "Failed to load background image" << std::endl;
        return false;
    }

    sf::Sprite background(backgroundTexture);                              // Create background sprite
    background.setScale(                                                   // Scale to match window size
        (float)window.getSize().x / backgroundTexture.getSize().x,
        (float)window.getSize().y / backgroundTexture.getSize().y
    );

    sf::Text title("Assigning Roles", font, 60);                           // Title of the screen
    title.setFillColor(sf::Color::White);
    title.setPosition(640 - title.getLocalBounds().width / 2, 40);         // Center title horizontally

    std::vector<Player*>& players = const_cast<std::vector<Player*>&>(game.get_all_players()); // Access players list
    size_t currentPlayerIndex = 0;                                         // Track index of current player being assigned

    std::random_device rd;                                                // Used for random generation (not actually used here)
    std::mt19937 gen(rd());

    std::vector<std::string> roleNames = {                                // List of available roles
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };

    sf::Vector2f buttonSize(330, 70);                                     // Dimensions for the "Continue" button
    sf::RectangleShape startButton(buttonSize);                           // Create the button
    startButton.setFillColor(sf::Color::Black);
    startButton.setOutlineColor(sf::Color(255, 215, 0));                  // Gold outline
    startButton.setOutlineThickness(4);
    startButton.setPosition((1280 - buttonSize.x) / 2, 600);              // Center button

    sf::Text startText("The Deception Begins", font, 34);                 // Button label
    startText.setFillColor(sf::Color(255, 215, 0));                       // Gold color
    sf::FloatRect textBounds = startText.getLocalBounds();
    startText.setPosition(                                               // Center text in button
        startButton.getPosition().x + (buttonSize.x - textBounds.width) / 2,
        startButton.getPosition().y + (buttonSize.y - textBounds.height) / 2 - 10
    );

    while (window.isOpen()) {                                            // Main event loop
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {                        // Handle window close
                window.close();
                return false;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (currentPlayerIndex < players.size()) {                // Assign role if players remain
                    std::vector<std::string> roleNames = {               // Available role names
                        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
                    };

                    std::uniform_int_distribution<> dis(0, roleNames.size() - 1); //random choice number
                    std::string role = roleNames[dis(gen)]; // random role choose

                    std::string name = players[currentPlayerIndex]->name();
                    Player* newPlayer = nullptr;

                    // Create new role-specific player object
                    if (role == "Governor") newPlayer = new Governor(game, name);
                    else if (role == "Spy") newPlayer = new Spy(game, name);
                    else if (role == "Baron") newPlayer = new Baron(game, name);
                    else if (role == "General") newPlayer = new General(game, name);
                    else if (role == "Judge") newPlayer = new Judge(game, name);
                    else if (role == "Merchant") newPlayer = new Merchant(game, name);

                    delete players[currentPlayerIndex];                  // Delete old generic Player object
                    players[currentPlayerIndex] = newPlayer;             // Replace with role-specific instance

                    currentPlayerIndex++;                                // Move to next player
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window); // Get mouse position
                if (startButton.getGlobalBounds().contains(mousePos) &&
                    currentPlayerIndex >= players.size()) {
                    window.close();                                      // If all roles assigned and button clicked, close window
                    return true;
                }
            }
        }

        // Hover effect for start button
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
        if (startButton.getGlobalBounds().contains(mousePos)) {
            startButton.setOutlineColor(sf::Color::White);
            startText.setFillColor(sf::Color::White);
        } else {
            startButton.setOutlineColor(sf::Color(255, 215, 0));
            startText.setFillColor(sf::Color(255, 215, 0));
        }

        window.clear();                                                  // Clear window
        window.draw(background);                                         // Draw background
        window.draw(title);                                              // Draw title

        float yOffset = 160;                                             // Vertical offset for player list
        for (size_t i = 0; i < players.size(); ++i) {
            std::string displayText = players[i]->name();               // Start with player name
            if (i < currentPlayerIndex) {
                displayText += " - " + players[i]->role();              // Show assigned role
            } else if (i == currentPlayerIndex) {
                displayText += " (Press ENTER)";                        // Indicate waiting for assignment
            }

            sf::Text playerText(displayText, font, 36);                 // Create text object
            playerText.setFillColor(i < currentPlayerIndex ? sf::Color(255, 215, 0) : sf::Color::White);
            playerText.setPosition(160, yOffset);                       // Set position
            window.draw(playerText);                                    // Draw text
            yOffset += 55;
        }

        if (currentPlayerIndex >= players.size()) {
            window.draw(startButton);                                   // Show start button if all roles assigned
            window.draw(startText);
        }

        window.display();                                               // Display everything
    }

    return true;                                                         // Success
}


   // Runs the main game loop, handling user interactions and displaying the game state
void runGameLoopScreen(Game& game) {
    // Create the main game window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Coup Game - GUI");

    // Load background image
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

    // Load font for text rendering
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf")) {
        std::cerr << "Failed to load font.ttf" << std::endl;
        return;
    }

    // Create game renderer and initialize with players and current turn
    GameRenderer renderer(font, game);
    renderer.setPlayers(game.get_all_players());
    renderer.setTurn(game.turn());

    bool gameEnded = false;         // Flag to mark if game has ended
    bool showWinnerPopup = false;  // Flag to display the winner message
    std::string winnerName;        // Holds the winner’s name (if found)

    // Main event loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle mouse click for user actions
            if (!gameEnded && event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f clickPos(event.mouseButton.x, event.mouseButton.y);
                renderer.handleClick(clickPos);  // Delegate click handling to the renderer
            }

            // If the game is over and user clicks anywhere, close the window
            if (showWinnerPopup && event.type == sf::Event::MouseButtonPressed) {
                window.close();
            }
        }

        // Try to check for winner if the game hasn’t ended yet
        if (!gameEnded) {
            try {
                winnerName = game.winner();  // Will throw an exception if there's no winner yet
                gameEnded = true;
                showWinnerPopup = true;
            } catch (const std::exception&) {
                // No winner yet – continue silently
            }
        }

        // Rendering phase
        window.clear();
        window.draw(background);    // Draw background
        renderer.draw(window);      // Draw game state

        // If the game has ended, draw the winner popup
        if (showWinnerPopup) {
            sf::RectangleShape popup(sf::Vector2f(500, 200));
            popup.setFillColor(sf::Color(0, 0, 0, 230));         // Semi-transparent black
            popup.setOutlineColor(sf::Color(255, 215, 0));       // Gold border
            popup.setOutlineThickness(4);
            popup.setPosition(390, 260);                         // Centered popup box

            sf::Text winText(winnerName + " wins the game!", font, 36);
            winText.setFillColor(sf::Color(255, 215, 0));        // Gold text
            sf::FloatRect textBounds = winText.getLocalBounds();
            winText.setPosition(640 - textBounds.width / 2, 300); // Centered

            sf::Text exitText("Click anywhere to exit...", font, 24);
            exitText.setFillColor(sf::Color::White);
            sf::FloatRect exitBounds = exitText.getLocalBounds();
            exitText.setPosition(640 - exitBounds.width / 2, 370); // Centered under winner text

            window.draw(popup);
            window.draw(winText);
            window.draw(exitText);
        }

        window.display();  // Show everything
    }
}
}