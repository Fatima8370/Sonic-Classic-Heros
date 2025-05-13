#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "BuildLevel.h"
#include "Character.h"

using namespace sf;
using namespace std;

class Level {
private:
    //===========================================================================
    // LEVEL PROPERTIES & STATE
    //===========================================================================
    int timer;
    int scoreCounter;
    bool isPaused;
    bool isComplete;
    bool isGameOver;

    int levelWidth;
    int levelHeight;
    float gravity;
    float friction;
    float timeLimit;

    int ringCount;
    int totalRingCount;
    int levelNum;

    //===========================================================================
    // ENTITY MANAGEMENT
    //===========================================================================
    BuildLevel* buildLevel;
    PlayerFactory* playerFactory;

    //===========================================================================
    // ASSETS & UI ELEMENTS
    //===========================================================================
    Texture backgroundTexture;
    Sprite  backgroundSprite;
    Font    font;

    Text scoreText;
    Text timerText;
    Text livesText;
    Text ringsText;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    Level(int lvl = 0)
        : timer(0), scoreCounter(0), isPaused(false), isComplete(false), isGameOver(false),
        levelWidth(0), levelHeight(0), gravity(0.98f), friction(0.1f), timeLimit(180.0f),
        ringCount(0), totalRingCount(0), buildLevel(nullptr), playerFactory(nullptr), levelNum(lvl)
    {
        buildLevel = new BuildLevel();
        font.loadFromFile("OOP-Final/SyneMono-Regular.ttf");
        setupUIElements();

        // If a level was specified, load it immediately
        if (lvl > 0) {
            loadLevel(lvl);
        }
    }

    ~Level() {
        cleanup();
    }

    // Clean up resources properly
    void cleanup() {
        if (buildLevel) {
            delete buildLevel;
            buildLevel = nullptr;
        }

        // Don't delete playerFactory as it's managed externally
        playerFactory = nullptr;
    }

    //===========================================================================
    // UI SETUP
    //===========================================================================
    void setupUIElements() {
        scoreText.setFont(font); scoreText.setCharacterSize(20); scoreText.setFillColor(Color::White); scoreText.setPosition(20, 20);
        timerText.setFont(font); timerText.setCharacterSize(20); timerText.setFillColor(Color::White); timerText.setPosition(20, 50);
        livesText.setFont(font); livesText.setCharacterSize(20); livesText.setFillColor(Color::White); livesText.setPosition(20, 80);
        ringsText.setFont(font); ringsText.setCharacterSize(20); ringsText.setFillColor(Color::Yellow); ringsText.setPosition(20, 110);
    }

    //===========================================================================
    // LEVEL LOADING & SETUP
    //===========================================================================
    bool loadBackground(const string& filename) {
        if (!backgroundTexture.loadFromFile(filename)) {
            cerr << "Failed to load background: " << filename << endl;
            return false;
        }
        backgroundSprite.setTexture(backgroundTexture);
        float sx = 1280.0f / backgroundTexture.getSize().x;
        float sy = 896.0f / backgroundTexture.getSize().y;
        backgroundSprite.setScale(sx, sy);
        return true;
    }

    bool loadLevel(int lvl) {
        // Reset level state
        levelNum = lvl;
        timer = 0;
        scoreCounter = 0;
        isPaused = false;
        isComplete = false;
        isGameOver = false;
        ringCount = 0;

        // Configure level parameters based on level number
        switch (lvl) {
        case 1:
            levelWidth = 200;
            levelHeight = 14;
            gravity = 2.0f;
            friction = 0.1f;
            timeLimit = 180.0f;
            loadBackground("Data/Backgrounds/labyrinth_bg.png");
            break;
        case 2:
            levelWidth = 250;
            levelHeight = 16;
            gravity = 2.0f;
            friction = 0.05f;
            timeLimit = 210.0f;
            loadBackground("Data/Backgrounds/icecap_bg.png");
            break;
        case 3:
            levelWidth = 300;
            levelHeight = 18;
            gravity = 1.0f;
            friction = 0.1f;
            timeLimit = 240.0f;
            loadBackground("Data/Backgrounds/deathegg_bg.png");
            break;
        case 4:
            levelWidth = 30;
            levelHeight = 14;
            gravity = 2.0f;
            friction = 0.1f;
            timeLimit = 120.0f;
            loadBackground("Data/Backgrounds/boss_bg.png");
            break;
        default:
            cerr << "Invalid level: " << lvl << endl;
            return false;
        }

        // Ensure we have a clean BuildLevel instance
        if (buildLevel) {
            delete buildLevel;
        }
        buildLevel = new BuildLevel();

        // Load the level data
        if (!buildLevel->loadLevel(lvl)) {
            cerr << "BuildLevel failed to load level " << lvl << endl;
            return false;
        }

        // Count total rings for UI and win condition
        totalRingCount = countTotalRings();

        // Set up player properties if player factory exists
        if (playerFactory) {
            buildLevel->setPlayerFactory(playerFactory);
            setupPlayers();
        }

        cout << "Level " << lvl << " loaded with " << totalRingCount << " rings." << endl;
        return true;
    }

    int countTotalRings() const {
        if (!buildLevel) return 0;

        int count = 0;
        Collectibles** cols = buildLevel->getCollectibles();
        int cCount = buildLevel->getCollectibleCount();

        for (int i = 0; i < cCount; ++i) {
            if (cols[i] && cols[i]->getSymbol() == 'r') {
                ++count;
            }
        }

        return count;
    }

    void setupPlayers() {
        if (!playerFactory) return;

        // Set gravity and level width for all players
        for (int i = 0; i < 3; ++i) {
            Player* p = playerFactory->getPlayer(i);
            if (p) {
                p->setGravity(gravity);
                p->setGridWidth(levelWidth);

                // Reset player position to starting point
                if (i == playerFactory->getActivePlayerIndex()) {
                    p->setPosition(300, 100);
                }
            }
        }
    }

    //===========================================================================
    // GAME LOOP METHODS
    //===========================================================================
    void update(RenderWindow& window, float dt) {
        if (isPaused || isComplete || isGameOver) return;

        // Update game timer
        timer += dt;

        // Get active player
        Player* active = playerFactory ? playerFactory->getActivePlayer() : nullptr;
        if (!active) return;

        // Calculate camera offset based on player position
        float offsetX = calculateOffset(active->getX());

        // Update all entities through BuildLevel
        if (buildLevel) {
            buildLevel->update(window, offsetX, dt, active);
        }

        // Update game state
        updateRingCount();
        updateScore();
        updateUIText();
        checkGameConditions();
    }

    float calculateOffset(float px) const {
        // Calculate camera offset based on player position
        float max = levelWidth * 64 - 1280; // Maximum offset (level width minus screen width)

        if (px < 640) return 0; // Player is in the left portion of the screen
        if (px > levelWidth * 64 - 640) return max; // Player is in the right portion

        return px - 640; // Center player on screen
    }

    void render(RenderWindow& window) {
        if (!buildLevel) return;

        // Draw background
        window.draw(backgroundSprite);

        // Calculate camera offset
        float offsetX = 0;
        if (playerFactory && playerFactory->getActivePlayer()) {
            offsetX = calculateOffset(playerFactory->getActivePlayer()->getX());
        }

        // Draw obstacles (should be drawn first as they're in the background)
        Obstacles** obstacles = buildLevel->getObstacles();
        int obstacleCount = buildLevel->getObstacleCount();
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i]) {
                obstacles[i]->draw(window, 1, offsetX);
            }
        }

        // Draw collectibles
        Collectibles** collectibles = buildLevel->getCollectibles();
        int collectibleCount = buildLevel->getCollectibleCount();
        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i] && !collectibles[i]->getIsCollected()) {
                collectibles[i]->draw(window, 1, offsetX);
            }
        }

        // Draw enemies
        Enemies** enemies = buildLevel->getEnemies();
        int enemyCount = buildLevel->getEnemyCount();
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] && enemies[i]->isDead()) {
                enemies[i]->draw(window, 1,offsetX);
            }
        }

        // Draw player
        if (playerFactory) {
            playerFactory->draw(window, 1.0f, offsetX);
        }

        // Draw UI elements (stays fixed on screen)
        window.draw(scoreText);
        window.draw(timerText);
        window.draw(livesText);
        window.draw(ringsText);
    }

    //===========================================================================
    // GAME STATE MANAGEMENT
    //===========================================================================
    void pause() { isPaused = true; }
    void resume() { isPaused = false; }
    void togglePause() { isPaused = !isPaused; }

    void setPlayerFactory(PlayerFactory* f) {
        playerFactory = f;
        if (buildLevel) {
            buildLevel->setPlayerFactory(f);
        }

        // Update player properties if we already have a level loaded
        if (playerFactory && buildLevel && buildLevel->getCurrentLevel() > 0) {
            setupPlayers();
        }
    }

    void updateRingCount() {
        if (!buildLevel) return;

        int count = 0;
        Collectibles** cols = buildLevel->getCollectibles();
        int colCount = buildLevel->getCollectibleCount();

        for (int i = 0; i < colCount; ++i) {
            if (cols[i] && cols[i]->getSymbol() == 'r' && cols[i]->getIsCollected()) {
                ++count;
            }
        }

        ringCount = count;
    }

    void updateScore() {
        // Base score from rings
        int base = ringCount * 10;

        // Bonus points for remaining time
        int rem = static_cast<int>(timeLimit - timer);

        // Final score calculation
        scoreCounter = base + (rem > 0 ? rem * 2 : 0);
    }

    void updateUIText() {
        // Update score display
        scoreText.setString("Score: " + to_string(scoreCounter));

        // Update timer display (MM:SS format)
        int secs = static_cast<int>(timeLimit - timer);
        if (secs < 0) secs = 0;
        timerText.setString("Time: " + to_string(secs / 60) + ":" +
            (secs % 60 < 10 ? "0" : "") + to_string(secs % 60));

        // Update lives display
        livesText.setString("Lives: " + to_string(Player::getLives()));

        // Update ring count display
        ringsText.setString("Rings: " + to_string(ringCount) + "/" + to_string(totalRingCount));
    }

    void checkGameConditions() {
        // Check win condition
        if (checkWin()) {
            isComplete = true;
            cout << "Level complete!" << endl;
        }

        // Check lose condition
        if (checkLose()) {
            isGameOver = true;
            cout << "Game over!" << endl;
        }
    }

    bool checkWin() const {
        // Win requires collecting all rings and reaching the end of the level
        if (ringCount < totalRingCount || !playerFactory) return false;

        Player* p = playerFactory->getActivePlayer();
        // Player needs to be near the end of the level (within 5 tiles of the right edge)
        return p && (p->getX() >= (levelWidth - 5) * 64);
    }

    bool checkLose() const {
        // Lose if out of lives or time
        if (Player::getLives() <= 0 || timer >= timeLimit) return true;

        if (playerFactory) {
            Player* p = playerFactory->getActivePlayer();
            if (p && p->getY() > levelHeight * 64) { // Player fell off the level
                // Decrease life and respawn if lives remain
                Player::setLives(Player::getLives() - 1);
                if (Player::getLives() > 0) {
                    p->setPosition(300, 100);
                    return false;
                }
                return true; // Game over if no lives remain
            }
        }
        return false;
    }

    // Reset the player's position (e.g., after falling or taking damage)
    void respawnPlayer() {
        if (!playerFactory) return;

        Player* active = playerFactory->getActivePlayer();
        if (active) {
            active->setPosition(300, 100); // Default spawn position
            active->setVelocityX(0);
            active->setVelocityY(0);
        }
    }

    // Progress to next level
    bool nextLevel() {
        int nextLvl = levelNum + 1;
        // Check if next level exists (typically 1-4)
        if (nextLvl > 4) return false;

        return loadLevel(nextLvl);
    }

    //===========================================================================
    // GETTERS & SETTERS
    //===========================================================================
    int getScore()         const { return scoreCounter; }
    int getTimer()         const { return timer; }
    int getRingCount()     const { return ringCount; }
    int getTotalRingCount()const { return totalRingCount; }
    bool getIsComplete()   const { return isComplete; }
    bool getIsGameOver()   const { return isGameOver; }
    bool getIsPaused()     const { return isPaused; }
    BuildLevel* getBuildLevel() { return buildLevel; }
    int getCurrentLevel()  const { return buildLevel ? buildLevel->getCurrentLevel() : 0; }

    float getGravity()     const { return gravity; }
    float getFriction()    const { return friction; }
    int getLevelWidth()    const { return levelWidth; }
    int getLevelHeight()   const { return levelHeight; }

    void setGravity(float g) {
        gravity = g;
        // Update all players with new gravity
        if (playerFactory) {
            for (int i = 0; i < 3; ++i) {
                Player* p = playerFactory->getPlayer(i);
                if (p) p->setGravity(gravity);
            }
        }
    }

   
};