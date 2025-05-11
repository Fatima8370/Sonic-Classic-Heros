#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>


#include "Level.h"
#include "Menu.h"
#include "Audio.h"

using namespace std;
using namespace sf;

class GameEngine {
private:
    // Window properties
    const float screen_x = 1280;
    const float screen_y = 896;
    RenderWindow window;

    // GameEngine components
    Menu menu;
    MusicManager gameMusic;
    Leaderboard leaderboard;
    SaveData save;
    ScoreManager sm;

    // GameEngine states
    bool isMenu = true;
    bool isPlaying = false;
    bool isPaused = false;
    bool isExiting = false;
    bool isInOptions = false;
    bool isLeaderboard = false;
    bool isLoadScreen = false;
    bool isLevelSelect = false;
    bool isNameEntry = true;

    // Player info
    string playerName = "Unnamed";

    // Level selection
    static const int LEVEL_COUNT = 4; // 3 regular levels + 1 boss level
    int selectedIndex = 0;
    int currentLevel = 0;

    // UI elements
    Font font;
    Text levelSelect[LEVEL_COUNT];
    Text prompt;
    Text enterName;

public:
    GameEngine() : window(VideoMode(screen_x, screen_y), "Sonic Classic Heroes", Style::Close),
        menu(screen_x, screen_y),
        gameMusic(screen_x, screen_y),
        leaderboard(screen_x, screen_y) {
        // Load font
        font.loadFromFile("OOP-Final/SyneMono-Regular.ttf");

        // Initialize level select menu
        for (int i = 0; i < LEVEL_COUNT - 1; ++i) {
            levelSelect[i].setFont(font);
            levelSelect[i].setString("Level " + std::to_string(i + 1));
            levelSelect[i].setCharacterSize(24);
            levelSelect[i].setPosition((screen_x - 150) / 2, (screen_y / 6) * (i + 1));
        }

        // Boss level
        levelSelect[LEVEL_COUNT - 1].setFont(font);
        levelSelect[LEVEL_COUNT - 1].setString("Boss Level");
        levelSelect[LEVEL_COUNT - 1].setCharacterSize(24);
        levelSelect[LEVEL_COUNT - 1].setPosition((screen_x - 150) / 2, (screen_y / 6) * LEVEL_COUNT);

        // Set up name entry text
        prompt.setString("Enter your name:");
        prompt.setFont(font);
        prompt.setCharacterSize(24);
        prompt.setFillColor(Color::Cyan);
        prompt.setPosition(screen_x / 3, screen_y / 3);

        enterName.setString("");
        enterName.setFont(font);
        enterName.setCharacterSize(24);
        enterName.setStyle(Text::Bold);
        enterName.setPosition(screen_x / 3, screen_y / 3 + 50);

        // Set initial GameEngine state
        isMenu = true;
        isPlaying = false;
        isPaused = false;
        isExiting = false;
        isInOptions = false;
        isLeaderboard = false;
        isLoadScreen = false;
        isLevelSelect = false;
        isNameEntry = false;
    }

    // Main game loop
    void run() {
        Event ev;
        while (window.isOpen()) {
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) {
                    window.close();
                }

                // Handle input based on game state
                if (isNameEntry) {
                    handleNameEntryInput(ev);
                }
                else if (isLevelSelect) {
                    handleLevelSelectInput(ev);
                }
                else if (isLoadScreen) {
                    handleLoadScreenInput(ev);
                }
                else if (isMenu && !isInOptions && !isLeaderboard) {
                    handleMenuInput(ev);
                }
                else if (isLeaderboard) {
                    handleLeaderboardInput(ev);
                }
                else if (isInOptions) {
                    handleOptionsInput(ev);
                }
                else if (isPlaying) {
                    if (isPaused) {
                        handlePauseInput(ev);
                    }
                    else {
                        handleGameInput(ev);
                    }
                }
            }

            updateGame();
            render();
        }
    }

private:
    // Handle input in the main menu
    void handleMenuInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Up) {
                menu.moveup();
            }
            else if (ev.key.code == Keyboard::Down) {
                menu.movedown();
            }
            else if (ev.key.code == Keyboard::Enter) {
                handleMenuSelection(menu.getSelectedOption());
            }
        }
    }

    // Handle input in the leaderboard view
    void handleLeaderboardInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Escape) {
                isLeaderboard = false;
                isMenu = true;
            }
        }
    }

    // Handle input in the options menu
    void handleOptionsInput(Event& ev) {
        gameMusic.handleInput(ev);

        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Escape) {
                isInOptions = false;
                isMenu = true;
            }
        }
    }

    // Handle input during gameplay
    void handleGameInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Escape) {
                isPaused = true;
            }
            // Add character controls here
        }
    }

    // Handle input while game is paused
    void handlePauseInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Enter) {
                isPaused = false;
            }
            else if (ev.key.code == Keyboard::Escape) {
                isMenu = true;
                isPaused = false;
            }
        }
    }

    // Handle name entry input
    void handleNameEntryInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Return || ev.key.code == Keyboard::Enter) {
                if (!playerName.empty()) {
                    save.setPlayerName(playerName);
                    save.saveToFile("saved.txt");
                    isNameEntry = false;
                    isLevelSelect = true;
                }
            }
            else if (ev.key.code == Keyboard::Escape) {
                // Allow going back to menu
                isNameEntry = false;
                isMenu = true;
            }
            else if (ev.key.code == Keyboard::BackSpace) {
                if (!playerName.empty()) {
                    playerName.pop_back();
                    enterName.setString(playerName);
                }
            }
        }
        else if (ev.type == Event::TextEntered) {
            if (ev.text.unicode < 128 && ev.text.unicode != 8 && ev.text.unicode != 13) {
                if (isprint(static_cast<char>(ev.text.unicode)) && playerName.length() < 20) {
                    playerName += static_cast<char>(ev.text.unicode);
                    enterName.setString(playerName);
                }
            }
        }
    }

    // Handle level selection input
    void handleLevelSelectInput(Event& ev) {
        if (ev.type == Event::KeyPressed) {
            if (ev.key.code == Keyboard::Up) {
                if (selectedIndex > 0) selectedIndex--;
            }
            else if (ev.key.code == Keyboard::Down) {
                if (selectedIndex < LEVEL_COUNT - 1) selectedIndex++;
            }
            else if (ev.key.code == Keyboard::Enter) {
                isLevelSelect = false;
                isLoadScreen = true;
                currentLevel = selectedIndex + 1;
            }
        }
    }

    // Handle loading screen input
    void handleLoadScreenInput(Event& ev) {
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Enter) {
            isLoadScreen = false;
            isPlaying = true;
        }
    }

    // Process menu selection
    void handleMenuSelection(int selectedOption) {
        switch (selectedOption) {
        case 0:  // New Game
            startNewGame();
            break;
        case 1:  // Continue
            continueGame();
            break;
        case 2:  // Leaderboard
            isMenu = false;
            isLeaderboard = true;
            break;
        case 3:  // Options
            isMenu = false;
            isInOptions = true;
            break;
        case 4:  // Exit
            exitGame();
            break;
        }
    }

    // Start a new game
    void startNewGame() {
        isMenu = false;
        isNameEntry = true;

        // Reset player name for new entry
        playerName = "";
        enterName.setString(playerName);

        cout << "Starting new game, entering name entry screen" << endl;
    }

    // Continue a previously saved game
    void continueGame() {
        if (save.loadFromFile("saved.txt")) {
            playerName = save.getPlayerName();
            isMenu = false;
            isLevelSelect = true;
        }
        else {
            cout << "No save file found.\n";
        }
    }

    // Load a specific level
    void loadLevel(int level) {
        currentLevel = level;
        isPlaying = true;

        // Add level loading logic here
        switch (level) {
        case 1:
            // Load level 1
            break;
        case 2:
            // Load level 2
            break;
        case 3:
            // Load level 3
            break;
        case 4:
            // Load boss level
            break;
        }
    }

    // Update game state
    void updateGame() {
        if (isPlaying && !isPaused) {
            // Add game logic updates here
        }
    }

    // Render the game
    void render() {
        window.clear();

        if (isNameEntry) {
            renderNameEntry();
        }
        else if (isLevelSelect) {
            renderLevelSelect();
        }
        else if (isLoadScreen) {
            renderLoadingScreen();
        }
        else if (isMenu && !isInOptions && !isLeaderboard) {
            menu.draw(window);
        }
        else if (isInOptions) {
            gameMusic.drawMenu(window);
        }
        else if (isLeaderboard) {
            leaderboard.draw(window);
        }
        else if (isPlaying) {
            if (isPaused) {
                renderPauseScreen();
            }
            else {
                renderGameplay();
            }
        }

        window.display();
    }

    // Render name entry screen
    void renderNameEntry() {
        // Create a title for the name entry screen
        Text nameTitle("ENTER YOUR NAME", font, 36);
        nameTitle.setFillColor(Color::Cyan);
        FloatRect titleBounds = nameTitle.getLocalBounds();
        nameTitle.setPosition((screen_x - titleBounds.width) / 2, screen_y / 4);

        // Position the prompt and input field
        prompt.setString("Type your name and press Enter:");
        FloatRect promptBounds = prompt.getLocalBounds();
        prompt.setPosition((screen_x - promptBounds.width) / 2, screen_y / 2 - 30);

        // Create an underline or box for the text field
        RectangleShape textBox(Vector2f(300, 40));
        textBox.setFillColor(Color(50, 50, 50));
        textBox.setOutlineColor(Color::White);
        textBox.setOutlineThickness(2);
        textBox.setPosition((screen_x - 300) / 2, screen_y / 2 + 20);

        // Position the entered name text
        enterName.setPosition((screen_x - 280) / 2, screen_y / 2 + 25);

        // Add cursor indicator if no text
        if (playerName.empty()) {
            Text cursorText("_", font, 24);
            cursorText.setPosition((screen_x - 280) / 2, screen_y / 2 + 25);
            cursorText.setFillColor(Color::White);
            window.draw(cursorText);
        }

        // Add instruction text
        Text instruction("Press ESC to return to menu", font, 18);
        instruction.setFillColor(Color(200, 200, 200));
        FloatRect instBounds = instruction.getLocalBounds();
        instruction.setPosition((screen_x - instBounds.width) / 2, screen_y * 3 / 4);

        window.draw(nameTitle);
        window.draw(prompt);
        window.draw(textBox);
        window.draw(enterName);
        window.draw(instruction);
    }

    // Render level selection screen
    void renderLevelSelect() {
        // Draw a title for the level select screen
        Text selectTitle("SELECT LEVEL", font, 36);
        selectTitle.setFillColor(Color::Cyan);
        FloatRect titleBounds = selectTitle.getLocalBounds();
        selectTitle.setPosition((screen_x - titleBounds.width) / 2, screen_y / 10);
        window.draw(selectTitle);

        // Draw level options
        for (int i = 0; i < LEVEL_COUNT; ++i) {
            // Highlight the selected level
            levelSelect[i].setFillColor(i == selectedIndex ? Color::Yellow : Color::White);

            // Get text bounds to center the text
            FloatRect textBounds = levelSelect[i].getLocalBounds();
            levelSelect[i].setPosition((screen_x - textBounds.width) / 2, (screen_y / 6) * (i + 2));

            window.draw(levelSelect[i]);
        }
    }

    // Render loading screen
    void renderLoadingScreen() {
        Text title;

        // Display appropriate title based on level
        if (currentLevel == 4) {
            title.setString("BOSS LEVEL");
        }
        else {
            title.setString("LEVEL " + to_string(currentLevel));
        }

        title.setFont(font);
        title.setFillColor(Color::Blue);
        title.setCharacterSize(50);
        title.setStyle(Text::Bold);

        // Center the title horizontally
        FloatRect titleBounds = title.getLocalBounds();
        title.setPosition((screen_x - titleBounds.width) / 2, screen_y / 2);

        Text startPrompt("Press Enter to Start", font, 36);
        startPrompt.setFillColor(Color::White);

        // Center the prompt horizontally
        FloatRect promptBounds = startPrompt.getLocalBounds();
        startPrompt.setPosition((screen_x - promptBounds.width) / 2, screen_y / 2 + 70);

        window.draw(title);
        window.draw(startPrompt);
    }

    // Render pause screen
    void renderPauseScreen() {
        // Add pause screen rendering here
        Text pauseText("PAUSED", font, 50);
        pauseText.setFillColor(Color::White);
        pauseText.setPosition(screen_x / 2 - 100, screen_y / 2);
        window.draw(pauseText);
    }

    // Render gameplay
    void renderGameplay() {
        // Add gameplay rendering here
    }

    // Exit the game
    void exitGame() {
        isExiting = true;
        window.close();
    }
};