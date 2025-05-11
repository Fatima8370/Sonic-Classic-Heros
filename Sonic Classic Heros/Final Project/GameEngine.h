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

    Level levels[4]; // 3 and 1 boss level

	Clock clock; // For delta time calculation


public:
    GameEngine() : window(VideoMode(screen_x, screen_y), "Sonic Classic Heroes", Style::Close),
        menu(screen_x, screen_y),
        gameMusic(screen_x, screen_y),
        leaderboard(screen_x, screen_y) {

		levels[0] = Level(screen_x, screen_y, 1);
        levels[1] = Level(screen_x, screen_y, 2);
        levels[2] = Level(screen_x, screen_y, 3);
        levels[3] = Level(screen_x, screen_y, 4);

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

			Time deltaTime = clock.restart();

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
            render(window, deltaTime.asSeconds());
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

            case 0: startNewGame(); break;

            case 1: continueGame(); break;

            case 2: 
                isMenu = false;
                isLeaderboard = true;
                break;

            case 3: 
                isMenu = false;
                isInOptions = true;
                break;

            case 4: exitGame(); break;

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
        
    }

    // Render the game
    void render(RenderWindow& window, float deltaTime) {
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
                renderGameplay(window, deltaTime);
            }
        }

        window.display();
    }

    // Calculate approximate text width based on character count and size
    float calculateTextWidth(const string& str, int charSize) {
        return str.length() * charSize * 0.6f;
    }

    // Render name entry screen
    void renderNameEntry() {

        Text nameTitle("ENTER YOUR NAME", font, 36);
        nameTitle.setFillColor(Color::Cyan);


        float nameTitleWidth = calculateTextWidth("ENTER YOUR NAME", 36);
        nameTitle.setPosition((screen_x - nameTitleWidth) / 2, screen_y / 4);


        prompt.setString("Type your name and press Enter:");
        float promptWidth = calculateTextWidth("Type your name and press Enter:", 24);
        prompt.setPosition((screen_x - promptWidth) / 2, screen_y / 2 - 30);


        RectangleShape textBox(Vector2f(300, 40));
        textBox.setFillColor(Color(50, 50, 50));
        textBox.setOutlineColor(Color::White);
        textBox.setOutlineThickness(2);
        textBox.setPosition((screen_x - 300) / 2, screen_y / 2 + 20);


        enterName.setPosition((screen_x - 280) / 2, screen_y / 2 + 25);

        if (playerName.empty()) {
            Text cursorText("_", font, 24);
            cursorText.setPosition((screen_x - 280) / 2, screen_y / 2 + 25);
            cursorText.setFillColor(Color::White);
            window.draw(cursorText);
        }

        // Add instruction text
        Text instruction("Press ESC to return to menu", font, 18);
        instruction.setFillColor(Color(200, 200, 200));
        float instructionWidth = calculateTextWidth("Press ESC to return to menu", 18);
        instruction.setPosition((screen_x - instructionWidth) / 2, screen_y * 3 / 4);

        window.draw(nameTitle);
        window.draw(prompt);
        window.draw(textBox);
        window.draw(enterName);
        window.draw(instruction);
    }

    // Render level selection screen
    void renderLevelSelect() {

        Text selectTitle("SELECT LEVEL", font, 36);
        selectTitle.setFillColor(Color::Cyan);
        float titleWidth = calculateTextWidth("SELECT LEVEL", 36);
        selectTitle.setPosition((screen_x - titleWidth) / 2, screen_y / 10);
        window.draw(selectTitle);


        for (int i = 0; i < LEVEL_COUNT; ++i) {

            levelSelect[i].setFillColor(i == selectedIndex ? Color::Yellow : Color::White);

            string levelText;

            if (i < LEVEL_COUNT - 1) {
                levelText = "Level " + to_string(i + 1);
            }
            else {
                levelText = "Boss Level";
            }

            float textWidth = calculateTextWidth(levelText, 24);
            levelSelect[i].setPosition((screen_x - textWidth) / 2, (screen_y / 6) * (i + 2));

            window.draw(levelSelect[i]);
        }
    }


    void renderLoadingScreen() {
        Text title;


        string titleText;
        if (currentLevel == 4) {
            titleText = "BOSS LEVEL";
        }
        else {
            titleText = "LEVEL " + to_string(currentLevel);
        }

        title.setString(titleText);
        title.setFont(font);
        title.setFillColor(Color::Blue);
        title.setCharacterSize(50);
        title.setStyle(Text::Bold);


        float titleWidth = calculateTextWidth(titleText, 50);
        title.setPosition((screen_x - titleWidth) / 2, screen_y / 2);


        Text startPrompt("Press Enter to Start", font, 36);
        startPrompt.setFillColor(Color::White);


        float promptWidth = calculateTextWidth("Press Enter to Start", 36);
        startPrompt.setPosition((screen_x - promptWidth) / 2, screen_y / 2 + 70);

        window.draw(title);
        window.draw(startPrompt);
    }


    void renderPauseScreen() {

        Text pauseText("PAUSED", font, 50);
        pauseText.setFillColor(Color::White);


        float pauseWidth = calculateTextWidth("PAUSED", 50);
        pauseText.setPosition((screen_x - pauseWidth) / 2, screen_y / 2);

        window.draw(pauseText);
    }


    void renderGameplay(RenderWindow& window, float deltaTime) {
        if (isPlaying && !isPaused) {
            // Add game logic updates here

            levels[selectedIndex].render(window); // Example delta time
            levels[selectedIndex].update(window, deltaTime); // Example delta time


        }
    }


    void exitGame() {
        isExiting = true;
        window.close();
    }
};