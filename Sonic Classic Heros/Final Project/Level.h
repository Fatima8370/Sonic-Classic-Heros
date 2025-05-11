// Level.h
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "BuildLevel.h"
#include "Character.h"

using namespace std;
using namespace sf;

class Level {
private:
    // Level state
    int timer;                // Level timer in seconds
    int scoreCounter;         // Current score
    bool isPaused;            // Is level paused
    bool isComplete;          // Is level completed
    bool isGameOver;          // Is game over

    // Level properties
    int levelWidth;           // Level width in cells
    int levelHeight;          // Level height in cells
    char** grid;              // Level grid representation
    float gravity;            // Level-specific gravity
    float friction;           // Level-specific friction

    // Level mechanics
    float timeLimit;          // Time limit in seconds
    int ringCount;            // Current collected rings
    int totalRingCount;       // Total available rings

    // Entity management
    BuildLevel* buildLevel;   // Manages level entities
    PlayerFactory* playerFactory; // Manages player characters

    // Level assets
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Font font;                // For UI elements

    // UI elements
    Text scoreText;
    Text timerText;
    Text livesText;
    Text ringsText;

	int levelNum;            // Current level number

public:
    Level(float screen_x =0, float screen_y = 0, int lvl = 0) :
        timer(0),
        scoreCounter(0),
        isPaused(false),
        isComplete(false),
        isGameOver(false),
        levelWidth(0),
        levelHeight(0),
        grid(nullptr),
        gravity(0.98f),
        friction(0.1f),
        timeLimit(180.0f),     // 3 minutes default
        ringCount(0),
        totalRingCount(0),
        buildLevel(nullptr),
        playerFactory(nullptr),
		levelNum(lvl) { // Initialize level number
    
        buildLevel = new BuildLevel();

        // Load font for UI elements
        font.loadFromFile("OOP-Final/SyneMono-Regular.ttf");

        // Initialize UI texts
        setupUIElements();
    }

    ~Level() {
        // Clean up grid
        if (grid) {
            for (int i = 0; i < levelHeight; ++i) {
                delete[] grid[i];
            }
            delete[] grid;
            grid = nullptr;
        }

        // Clean up BuildLevel
        if (buildLevel) {
            delete buildLevel;
            buildLevel = nullptr;
        }

        // We don't delete playerFactory as it's owned by GameEngine
        playerFactory = nullptr;
    }

    // Initialize UI elements
    void setupUIElements() {
        // Score text
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(20, 20);

        // Timer text
        timerText.setFont(font);
        timerText.setCharacterSize(20);
        timerText.setFillColor(Color::White);
        timerText.setPosition(20, 50);

        // Lives text
        livesText.setFont(font);
        livesText.setCharacterSize(20);
        livesText.setFillColor(Color::White);
        livesText.setPosition(20, 80);

        // Rings text
        ringsText.setFont(font);
        ringsText.setCharacterSize(20);
        ringsText.setFillColor(Color::Yellow);
        ringsText.setPosition(20, 110);
    }

    // Load a specific level
    bool loadLevel(int levelNum) {
        // Reset level state
        timer = 0;
        scoreCounter = 0;
        isPaused = false;
        isComplete = false;
        isGameOver = false;
        ringCount = 0;

        // Set level parameters based on level number
        switch (levelNum) {
        case 1: // Labyrinth Zone
            levelWidth = 200;
            levelHeight = 14;
            gravity = 2.0f;
            friction = 0.1f;
            timeLimit = 180.0f; // 3 minutes
            loadBackground("Data/Backgrounds/labyrinth_bg.png");
            break;

        case 2: // Ice Cap Zone
            levelWidth = 250;
            levelHeight = 16;
            gravity = 2.0f;
            friction = 0.05f; // More slippery
            timeLimit = 210.0f; // 3.5 minutes
            loadBackground("Data/Backgrounds/icecap_bg.png");
            break;

        case 3: // Death Egg Zone
            levelWidth = 300;
            levelHeight = 18;
            gravity = 1.0f; // Lower gravity
            friction = 0.1f;
            timeLimit = 240.0f; // 4 minutes
            loadBackground("Data/Backgrounds/deathegg_bg.png");
            break;

        case 4: // Boss Level
            levelWidth = 30;
            levelHeight = 14;
            gravity = 2.0f;
            friction = 0.1f;
            timeLimit = 120.0f; // 2 minutes
            loadBackground("Data/Backgrounds/boss_bg.png");
            break;

        default:
            cout << "Invalid level number: " << levelNum << endl;
            return false;
        }

        // Clean up old grid if exists
        if (grid) {
            for (int i = 0; i < levelHeight; ++i) {
                delete[] grid[i];
            }
            delete[] grid;
        }

        // Initialize new grid
        grid = new char* [levelHeight];
        for (int i = 0; i < levelHeight; ++i) {
            grid[i] = new char[levelWidth];
            for (int j = 0; j < levelWidth; ++j) {
                grid[i][j] = ' ';  // Default empty cell
            }
        }

        // Load level through BuildLevel
        bool success = buildLevel->loadLevel(levelNum);

        if (success) {
            // Update level grid from BuildLevel
            char** levelGrid = buildLevel->getLevelGrid();
            if (levelGrid) {
                for (int i = 0; i < levelHeight; ++i) {
                    for (int j = 0; j < levelWidth; ++j) {
                        if (i < buildLevel->getHeight() && j < buildLevel->getWidth()) {
                            grid[i][j] = levelGrid[i][j];
                        }
                    }
                }
            }

            // Count total rings
            totalRingCount = 0;
            Collectibles** collectibles = buildLevel->getCollectibles();
            if (collectibles) {
                for (int i = 0; i < buildLevel->getCollectibleCount(); i++) {
                    if (collectibles[i] && collectibles[i]->getSymbol() == 'r') {
                        totalRingCount++;
                    }
                }
            }

            // Set BuildLevel's PlayerFactory
            if (playerFactory) {
                buildLevel->setPlayerFactory(playerFactory);

                // Apply level physics to all players
                for (int i = 0; i < 3; i++) {
                    Player* player = playerFactory->getPlayer(i);
                    if (player) {
                        player->setGravity(gravity);
                        // Set grid width for boundary checking
                        player->setGridWidth(levelWidth);
                    }
                }
            }

            cout << "Level " << levelNum << " loaded successfully with " << totalRingCount << " rings." << endl;
            return true;
        }
        else {
            cout << "Failed to load level " << levelNum << "." << endl;
            return false;
        }
    }

    // Load background texture
    void loadBackground(const string& filename) {
        if (!backgroundTexture.loadFromFile(filename)) {
            cout << "Failed to load background: " << filename << endl;
            // Load default background if failed
            backgroundTexture.loadFromFile("Data/BG.png");
        }
        backgroundSprite.setTexture(backgroundTexture);
    }

    // Update level logic
    void update(RenderWindow& window,float deltaTime) {
        if (isPaused || isComplete || isGameOver) {
            return;
        }

        // Update timer
        timer += deltaTime;

        // Get active player
        Player* activePlayer = playerFactory ? playerFactory->getActivePlayer() : nullptr;
        if (!activePlayer) return;

        // Calculate camera offset based on player position
        float offsetX = 0;
        if (activePlayer->getX() >= 640 && activePlayer->getX() <= levelWidth * 64 - 640) {
            offsetX = activePlayer->getX() - 640;
        }
        else if (activePlayer->getX() > levelWidth * 64 - 640) {
            offsetX = levelWidth * 64 - 1280;
        }

        // Update entities through BuildLevel
        buildLevel->update(window, offsetX, deltaTime, activePlayer);

        // Update ring count
        updateRingCount();

        // Calculate score
        updateScore();

        // Update UI text
        updateUIText();

        // Check win/lose conditions
        checkGameConditions();
    }

    // Update ring count
    void updateRingCount() {
        int newRingCount = 0;
        Collectibles** collectibles = buildLevel->getCollectibles();
        if (collectibles) {
            for (int i = 0; i < buildLevel->getCollectibleCount(); i++) {
                if (collectibles[i] && collectibles[i]->getIsCollected()) {
                    newRingCount++;
                }
            }
        }

        if (newRingCount != ringCount) {
            ringCount = newRingCount;
        }
    }

    // Update score
    void updateScore() {
        // Base score: 10 points per ring
        int baseScore = ringCount * 10;

        // Time bonus: points for remaining time
        int timeRemaining = timeLimit - timer;
        int timeBonus = (timeRemaining > 0) ? timeRemaining * 2 : 0;

        scoreCounter = baseScore + timeBonus;
    }

    // Update UI text elements
    void updateUIText() {
        scoreText.setString("Score: " + to_string(scoreCounter));

        int minutes = static_cast<int>(timeLimit - timer) / 60;
        int seconds = static_cast<int>(timeLimit - timer) % 60;
        timerText.setString("Time: " + to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + to_string(seconds));

        livesText.setString("Lives: " + to_string(Player::getLives()));
        ringsText.setString("Rings: " + to_string(ringCount) + "/" + to_string(totalRingCount));
    }

    // Check win/lose conditions
    void checkGameConditions() {
        if (checkWin()) {
            isComplete = true;
            cout << "Level complete!" << endl;
        }

        if (checkLose()) {
            isGameOver = true;
            cout << "Game over!" << endl;
        }
    }

    // Win condition check
    bool checkWin() {
        // Main win condition: collected all rings and reached end of level

        // First check: all rings collected
        bool allRingsCollected = (ringCount >= totalRingCount);

        // Second check: reached end of level
        bool reachedEnd = false;

        if (playerFactory) {

            Player* player = playerFactory->getActivePlayer();

            if (player) {
                // Consider level complete if player is near right edge
                float playerX = player->getX();
                float levelEndX = (levelWidth - 5) * 64; // 5 tiles from edge
                reachedEnd = (playerX >= levelEndX);
            }
        }

        return allRingsCollected && reachedEnd;
    }

    // Lose condition check
    bool checkLose() {
        // Lose conditions:
        // 1. Out of lives
        if (Player::getLives() <= 0) {
            return true;
        }

        // 2. Out of time
        if (timer >= timeLimit) {
            return true;
        }

        // 3. Player fell into pit
        if (playerFactory) {
            Player* player = playerFactory->getActivePlayer();
            if (player && player->getY() > levelHeight * 64) {
                // Reduce lives and respawn if still has lives
                Player::setLives(Player::getLives() - 1);
                if (Player::getLives() > 0) {
                    // Respawn player at start position
                    player->setPosition(300.0f, 100.0f);
                    return false;
                }
                return true;
            }
        }

        return false;
    }

    // Render the level
    void render(RenderWindow& window) {
        // Draw background
        window.draw(backgroundSprite);

        // Level rendering is handled by BuildLevel's update method
        // which was called in the update() method

        // Draw UI elements
        window.draw(scoreText);
        window.draw(timerText);
        window.draw(livesText);
        window.draw(ringsText);

       
    }

    // Set PlayerFactory
    void setPlayerFactory(PlayerFactory* factory) {
        playerFactory = factory;

        if (buildLevel) {
            buildLevel->setPlayerFactory(playerFactory);
        }
    }

    // Pause/resume methods
    void pause() {
        isPaused = true;
    }

    void resume() {
        isPaused = false;
    }

    void togglePause() {
        isPaused = !isPaused;
    }

    // Getters
    int getScore() const {
        return scoreCounter;
    }

    int getTimer() const {
        return static_cast<int>(timer);
    }

    int getRingCount() const {
        return ringCount;
    }

    int getTotalRingCount() const {
        return totalRingCount;
    }

    bool getIsComplete() const {
        return isComplete;
    }

    bool getIsGameOver() const {
        return isGameOver;
    }

    bool getIsPaused() const {
        return isPaused;
    }

    BuildLevel* getBuildLevel() {
        return buildLevel;
    }

    // Get current level number
    int getCurrentLevel() const {
        return buildLevel ? buildLevel->getCurrentLevel() : 0;
    }
};