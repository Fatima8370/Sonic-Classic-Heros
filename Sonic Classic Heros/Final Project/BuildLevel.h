#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Level.h"
#include "GameEntity.h"
#include "Obstacles.h"
#include "Enemies.h"
#include "Collectibles.h"

class ObstacleFactory;
class EnemyFactory;
class CollectibleFactory;
class Obstacles;
class Enemies;
class Collectibles;
class Player;


using namespace std;
using namespace sf;

class LevelLoader {
private:
    int gridHeight;
    int gridWidth;
    int currentLevel;

    // Level data
    char** levelGrid;
    char** obstacleGrid;
    char** enemyGrid;
    char** collectibleGrid;

    // Factory references
    ObstacleFactory* obstacleFactory;
    EnemyFactory* enemyFactory;
    CollectibleFactory* collectibleFactory;

    // Arrays to store created objects
    Obstacles** obstacles;
    Enemies** enemies;
    Collectibles** collectibles;

    // Counts
    int obstacleCount;
    int enemyCount;
    int collectibleCount;

    void initializeGrids() {
        levelGrid = new char* [gridHeight];
        obstacleGrid = new char* [gridHeight];
        enemyGrid = new char* [gridHeight];
        collectibleGrid = new char* [gridHeight];

        for (int i = 0; i < gridHeight; i++) {
            levelGrid[i] = new char[gridWidth];
            obstacleGrid[i] = new char[gridWidth];
            enemyGrid[i] = new char[gridWidth];
            collectibleGrid[i] = new char[gridWidth];

            // Initialize all cells to empty
            for (int j = 0; j < gridWidth; j++) {
                levelGrid[i][j] = ' ';
                obstacleGrid[i][j] = '\0';
                enemyGrid[i][j] = '\0';
                collectibleGrid[i][j] = '\0';
            }
        }
    }

    // Count items in grid
    void countGridItems() {
        obstacleCount = 0;
        enemyCount = 0;
        collectibleCount = 0;

        for (int i = 0; i < gridHeight; i++) {
            for (int j = 0; j < gridWidth; j++) {
                if (obstacleGrid[i][j] != '\0') obstacleCount++;
                if (enemyGrid[i][j] != '\0') enemyCount++;
                if (collectibleGrid[i][j] != '\0') collectibleCount++;
            }
        }
    }

    // Clear memory for grids
    void clearGrids() {
        if (levelGrid) {
            for (int i = 0; i < gridHeight; i++) {
                delete[] levelGrid[i];
                delete[] obstacleGrid[i];
                delete[] enemyGrid[i];
                delete[] collectibleGrid[i];
            }
            delete[] levelGrid;
            delete[] obstacleGrid;
            delete[] enemyGrid;
            delete[] collectibleGrid;

            levelGrid = nullptr;
            obstacleGrid = nullptr;
            enemyGrid = nullptr;
            collectibleGrid = nullptr;
        }
    }

public:
    // Constructor
    LevelLoader(int height, int width) :
        gridHeight(height),
        gridWidth(width),
        currentLevel(1),
        levelGrid(nullptr),
        obstacleGrid(nullptr),
        enemyGrid(nullptr),
        collectibleGrid(nullptr),
        obstacleFactory(nullptr),
        enemyFactory(nullptr),
        collectibleFactory(nullptr),
        obstacles(nullptr),
        enemies(nullptr),
        collectibles(nullptr),
        obstacleCount(0),
        enemyCount(0),
        collectibleCount(0)
    {
        initializeGrids();
    }

    // Destructor
    ~LevelLoader() {
        clearGrids();

        // Clean up object arrays
        if (obstacles) {
            delete[] obstacles;
            obstacles = nullptr;
        }

        if (enemies) {
            delete[] enemies;
            enemies = nullptr;
        }

        if (collectibles) {
            delete[] collectibles;
            collectibles = nullptr;
        }

        // Clean up factories
        if (obstacleFactory) {
            delete obstacleFactory;
            obstacleFactory = nullptr;
        }

        if (enemyFactory) {
            delete enemyFactory;
            enemyFactory = nullptr;
        }

        if (collectibleFactory) {
            delete collectibleFactory;
            collectibleFactory = nullptr;
        }
    }

    // Load level from file
    bool loadLevel(int levelNum) {
        currentLevel = levelNum;
        string filename = "Data/Level/level" + to_string(levelNum) + ".csv";

        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open level file: " << filename << endl;
            return false;
        }

        // Clear previous data
        clearGrids();
        initializeGrids();

        // Read level data
        for (int i = 0; i < gridHeight; ++i) {
            for (int j = 0; j < gridWidth; ++j) {
                char ch;
                file.get(ch);

                if (file.eof()) {
                    cerr << "Unexpected end of file while reading level." << endl;
                    return false;
                }

                levelGrid[i][j] = ch;

                // Categorize by type
                if (ch == ' ') {
                    continue;
                }
                else if (ch >= 'A' && ch <= 'Z') {
                    obstacleGrid[i][j] = ch;
                }
                else if (ch >= 'a' && ch <= 'z') {
                    collectibleGrid[i][j] = ch;
                }
                else if (ch >= '0' && ch <= '9') {
                    enemyGrid[i][j] = ch;
                }
            }
            file.ignore(1); // Skip newline
        }

        file.close();

        // Count items for array allocation
        countGridItems();

        cout << "Level " << levelNum << " loaded with:" << endl;
        cout << " - " << obstacleCount << " obstacles" << endl;
        cout << " - " << enemyCount << " enemies" << endl;
        cout << " - " << collectibleCount << " collectibles" << endl;

        return true;
    }

    // Create objects from loaded level data
    void createLevelObjects() {
        // Create factories with the proper parameters
        obstacleFactory = new ObstacleFactory(obstacleCount, gridHeight, gridWidth, currentLevel);
        enemyFactory = new EnemyFactory(gridHeight, gridWidth, currentLevel);
        collectibleFactory = new CollectibleFactory(collectibleCount, gridHeight, gridWidth, currentLevel);

        // Allocate arrays
        obstacles = new Obstacles * [obstacleCount];
        enemies = new Enemies * [enemyCount];
        collectibles = new Collectibles * [collectibleCount];

        // Create obstacles
        int obstacleIndex = 0;
        for (int i = 0; i < gridHeight; ++i) {
            for (int j = 0; j < gridWidth; ++j) {
                if (obstacleGrid[i][j] != '\0') {
                    obstacles[obstacleIndex] = obstacleFactory->createObstacle(
                        obstacleGrid[i][j],
                        j * 64, // Use cell size
                        i * 64
                    );
                    obstacleIndex++;
                }
            }
        }

        // Create enemies
        int enemyIndex = 0;
        for (int i = 0; i < gridHeight; ++i) {
            for (int j = 0; j < gridWidth; ++j) {
                if (enemyGrid[i][j] != '\0') {
                    enemies[enemyIndex] = enemyFactory->createEnemy(
                        enemyGrid[i][j],
                        j * 64, // Use cell size
                        i * 64
                    );
                    enemyIndex++;
                }
            }
        }

        // Create collectibles
        int collectibleIndex = 0;
        for (int i = 0; i < gridHeight; ++i) {
            for (int j = 0; j < gridWidth; ++j) {
                if (collectibleGrid[i][j] != '\0') {
                    collectibles[collectibleIndex] = collectibleFactory->createCollectible(
                        collectibleGrid[i][j],
                        j * 64, // Use cell size
                        i * 64
                    );
                    collectibleIndex++;
                }
            }
        }
    }

    // Update and render level
    void update(RenderWindow& window, Player* player, float offsetX, float deltaTime) {
        // Update and render obstacles
        obstacleFactory->updateObstacles(window, obstacles, obstacleCount, offsetX, player);

        // Update and render enemies
        enemyFactory->updateEnemies(window, enemies, enemyCount, offsetX, deltaTime, player);

        // Update and render collectibles
        collectibleFactory->updateCollectibles(window, collectibles, collectibleCount, offsetX, deltaTime, player);
    }

    // Get grid data
    char** getLevelGrid() { return levelGrid; }

    // Get counts
    int getObstacleCount() const { return obstacleCount; }
    int getEnemyCount() const { return enemyCount; }
    int getCollectibleCount() const { return collectibleCount; }

    // Change level
    bool changeLevel(int newLevel) {
        // Clean up existing objects
        if (obstacles) {
            for (int i = 0; i < obstacleCount; i++) {
                if (obstacles[i]) {
                    delete obstacles[i];
                    obstacles[i] = nullptr;
                }
            }
            delete[] obstacles;
            obstacles = nullptr;
        }

        if (enemies) {
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i]) {
                    delete enemies[i];
                    enemies[i] = nullptr;
                }
            }
            delete[] enemies;
            enemies = nullptr;
        }

        if (collectibles) {
            for (int i = 0; i < collectibleCount; i++) {
                if (collectibles[i]) {
                    delete collectibles[i];
                    collectibles[i] = nullptr;
                }
            }
            delete[] collectibles;
            collectibles = nullptr;
        }

        // Clean up factories
        if (obstacleFactory) {
            delete obstacleFactory;
            obstacleFactory = nullptr;
        }

        if (enemyFactory) {
            delete enemyFactory;
            enemyFactory = nullptr;
        }

        if (collectibleFactory) {
            delete collectibleFactory;
            collectibleFactory = nullptr;
        }

        // Load the new level
        return loadLevel(newLevel);
    }

    // Get object arrays
    Obstacles** getObstacles() { return obstacles; }
    Enemies** getEnemies() { return enemies; }
    Collectibles** getCollectibles() { return collectibles; }

    // Clean up all game objects
    void clearAllObjects() {
        if (obstacleFactory) {
            obstacleFactory->clearObstacles();
        }

        if (enemyFactory) {
            enemyFactory->clearEnemies();
        }

        if (collectibleFactory) {
            collectibleFactory->clearCollectibles();
        }
    }
};/*


ObstacleFactory obstacleFactory(MAX_OBSTACLES);
EnemyFactory enemyFactory(height, width, currentLevel);
CollectibleFactory collectibleFactory(MAX_COLLECTIBLES);


LevelLoader levelLoader(height, width);
levelLoader.loadLevel(1); // Load level 1

levelLoader.createLevelObjects(&obstacleFactory, &enemyFactory, &collectibleFactory);

levelLoader.update(window, player, offsetX, deltaTime);

levelLoader.changeLevel(nextLevel);
levelLoader.createLevelObjects(&obstacleFactory, &enemyFactory, &collectibleFactory);





*/