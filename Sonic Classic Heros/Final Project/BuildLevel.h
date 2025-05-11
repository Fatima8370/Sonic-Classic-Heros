#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Obstacles.h"
#include "Enemies.h"
#include "Collectibles.h"

using namespace std;
using namespace sf;

class BuildLevel {
private:

    Level* level;

    // Factories - now only create entities, don't store them
    EnemyFactory* enemyFactory;
    CollectibleFactory* collectibleFactory;
    ObstacleFactory* obstacleFactory;

    // Entity arrays - managed by BuildLevel
    Enemies** enemies;
    Collectibles** collectibles;
    Obstacles** obstacles;

    // Counts
    int enemyCount;
    int collectibleCount;
    int obstacleCount;

    // Level dimensions
    int height;
    int width;
    int currentLevel;

    // Level data (could be loaded from files)
    char** enemyLayout;
    char** collectibleLayout;
    char** obstacleLayout;
    char** levelGrid;

    // Load level data
    void loadLevelData(int levelNum) {
        // Clear previous layouts if they exist
        if (enemyLayout) {
            for (int i = 0; i < height; i++) {
                delete[] enemyLayout[i];
            }
            delete[] enemyLayout;
        }
        if (collectibleLayout) {
            for (int i = 0; i < height; i++) {
                delete[] collectibleLayout[i];
            }
            delete[] collectibleLayout;
        }
        if (obstacleLayout) {
            for (int i = 0; i < height; i++) {
                delete[] obstacleLayout[i];
            }
            delete[] obstacleLayout;
        }
        if (levelGrid) {
            for (int i = 0; i < height; i++) {
                delete[] levelGrid[i];
            }
            delete[] levelGrid;
        }

        // Set dimensions based on level
        if (levelNum == 1) {
            height = 14;
            width = 200;
        }
        else if (levelNum == 2) {
            height = 16;
            width = 250;
        }
        else if (levelNum == 3) {
            height = 18;
            width = 300;
        }
        else {
            height = 14;
            width = 30;
        }

        // Allocate memory for layouts
        enemyLayout = new char* [height];
        collectibleLayout = new char* [height];
        obstacleLayout = new char* [height];
        levelGrid = new char* [height];

        for (int i = 0; i < height; i++) {
            enemyLayout[i] = new char[width]();  // Initialize to '\0'
            collectibleLayout[i] = new char[width]();
            obstacleLayout[i] = new char[width]();
            levelGrid[i] = new char[width]();
        }
    }

    // Count the number of items in each grid
    void countGridItems() {
        // Reset counts
        enemyCount = 0;
        collectibleCount = 0;
        obstacleCount = 0;

        // Count items in each grid
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (enemyLayout[i][j] != '\0') enemyCount++;
                if (collectibleLayout[i][j] != '\0') collectibleCount++;
                if (obstacleLayout[i][j] != '\0') obstacleCount++;
            }
        }
    }

    // Clean up entities
    void clearEntities() {
        // Clean up enemies
        if (enemies) {
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i]) {
                    delete enemies[i];
                }
            }
            delete[] enemies;
            enemies = nullptr;
        }

        // Clean up collectibles
        if (collectibles) {
            for (int i = 0; i < collectibleCount; i++) {
                if (collectibles[i]) {
                    delete collectibles[i];
                }
            }
            delete[] collectibles;
            collectibles = nullptr;
        }

        // Clean up obstacles
        if (obstacles) {
            for (int i = 0; i < obstacleCount; i++) {
                if (obstacles[i]) {
                    delete obstacles[i];
                }
            }
            delete[] obstacles;
            obstacles = nullptr;
        }

        enemyCount = collectibleCount = obstacleCount = 0;
    }

public:
    BuildLevel() :
        enemyFactory(nullptr),
        collectibleFactory(nullptr),
        obstacleFactory(nullptr),
        enemies(nullptr),
        collectibles(nullptr),
        obstacles(nullptr),
        enemyCount(0),
        collectibleCount(0),
        obstacleCount(0),
        height(0),
        width(0),
        currentLevel(0),
        enemyLayout(nullptr),
        collectibleLayout(nullptr),
        obstacleLayout(nullptr),
        levelGrid(nullptr),
        level(nullptr)
    {
    }

    ~BuildLevel() {
        cleanup();
    }



    Level* generateLevel(int width, int height, int levelNum) {
        if (level) {
            delete level;  // Clean up previous level if exists
        }
        level = new Level(width, height, levelNum);
        level->loadLevel();
        return level;
    }

    void cleanup() {
        // Clean up factories
        if (enemyFactory) {
            delete enemyFactory;
            enemyFactory = nullptr;
        }
        if (collectibleFactory) {
            delete collectibleFactory;
            collectibleFactory = nullptr;
        }
        if (obstacleFactory) {
            delete obstacleFactory;
            obstacleFactory = nullptr;
        }

        // Clean up entities
        clearEntities();

        // Clean up layouts
        if (enemyLayout) {
            for (int i = 0; i < height; i++) {
                delete[] enemyLayout[i];
            }
            delete[] enemyLayout;
            enemyLayout = nullptr;
        }

        if (collectibleLayout) {
            for (int i = 0; i < height; i++) {
                delete[] collectibleLayout[i];
            }
            delete[] collectibleLayout;
            collectibleLayout = nullptr;
        }

        if (obstacleLayout) {
            for (int i = 0; i < height; i++) {
                delete[] obstacleLayout[i];
            }
            delete[] obstacleLayout;
            obstacleLayout = nullptr;
        }

        if (levelGrid) {
            for (int i = 0; i < height; i++) {
                delete[] levelGrid[i];
            }
            delete[] levelGrid;
            levelGrid = nullptr;
        }
    }

    bool loadLevel(int levelNum) {

        currentLevel = levelNum;
        string filename = "Data/Level/level" + to_string(levelNum) + ".csv";

        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open level file: " << filename << endl;
            return false;
        }

        // Clear previous data
        cleanup();
        loadLevelData(levelNum);

        // Read level data
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                char ch;
                file.get(ch);

                if (file.eof()) { // id error while reading files
                    return false;
                }

                levelGrid[i][j] = ch;

                // Categorize by type
                if (ch == ' ' || ch == '\n' || ch == '\r') {
                    continue;
                }
                else if (ch >= 'A' && ch <= 'Z') {
                    obstacleLayout[i][j] = ch;
                }
                else if (ch >= 'a' && ch <= 'z') {
                    collectibleLayout[i][j] = ch;
                }
                else if (ch >= '0' && ch <= '9') {
                    enemyLayout[i][j] = ch;
                }
            }

            // Skip newline characters
            char nextChar;
            while (file.get(nextChar) && (nextChar == '\n' || nextChar == '\r')) {}
            if (!file.eof()) {
                file.putback(nextChar);
            }
        }

        file.close();

        // Count items for array allocation
        countGridItems();

        cout << "Level " << levelNum << " loaded with:" << endl;
        cout << " - " << obstacleCount << " obstacles" << endl;
        cout << " - " << enemyCount << " enemies" << endl;
        cout << " - " << collectibleCount << " collectibles" << endl;

        // Create factories
        enemyFactory = new EnemyFactory(height, width, levelNum);
        collectibleFactory = new CollectibleFactory(height, width, levelNum);
        obstacleFactory = new ObstacleFactory(height, width, levelNum);

        // Allocate memory for entity arrays
        enemies = new Enemies * [enemyCount]();
        collectibles = new Collectibles * [collectibleCount]();
        obstacles = new Obstacles * [obstacleCount]();

        // Create entities based on layouts
        int enemyIndex = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (enemyLayout[i][j] != '\0') {
                    Enemies* e = enemyFactory->createEnemy(enemyLayout[i][j], j * 64, i * 64);
                    if (e) {
                        enemies[enemyIndex++] = e;
                    }
                }
            }
        }

        int collectibleIndex = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (collectibleLayout[i][j] != '\0') {
                    Collectibles* c = collectibleFactory->createCollectible(collectibleLayout[i][j], j * 64, i * 64);
                    if (c) {
                        collectibles[collectibleIndex++] = c;
                    }
                }
            }
        }

        int obstacleIndex = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (obstacleLayout[i][j] != '\0') {
                    Obstacles* o = obstacleFactory->createObstacle(obstacleLayout[i][j], j * 64, i * 64);
                    if (o) {
                        obstacles[obstacleIndex++] = o;
                    }
                }
            }
        }

        cout << "Level " << levelNum << " loaded with "
            << enemyIndex << " enemies, "
            << collectibleIndex << " collectibles, and "
            << obstacleIndex << " obstacles." << endl;

        return true;
    }

    void update(RenderWindow& window, float offsetX, float deltaTime, Player* player) {
        // Update all entities using the factory update methods
        if (enemyFactory && enemies) {
            enemyFactory->updateEnemies(window, enemies, enemyCount, offsetX, deltaTime, player);
        }

        if (collectibleFactory && collectibles) {
            collectibleFactory->updateCollectibles(window, collectibles, collectibleCount, offsetX, deltaTime, player);
        }

        if (obstacleFactory && obstacles) {
            obstacleFactory->updateObstacles(window, obstacles, obstacleCount, offsetX, player);
        }
    }

    bool isLevelComplete() {
        // Check conditions for level completion
        // This could be player reaching the end, defeating all enemies, collecting all items, etc.
        return false; // Placeholder
    }

    void nextLevel() {
        loadLevel(currentLevel + 1);
    }

    int getCurrentLevel() const {
        return currentLevel;
    }

    // Getters for entity arrays and counts
    int getEnemyCount() const { return enemyCount; }
    int getCollectibleCount() const { return collectibleCount; }
    int getObstacleCount() const { return obstacleCount; }

    Enemies** getEnemies() { return enemies; }
    Collectibles** getCollectibles() { return collectibles; }
    Obstacles** getObstacles() { return obstacles; }

    char** getEnemyLayout() { return enemyLayout; }
    char** getObstacleLayout() { return obstacleLayout; }
    char** getCollectiblesLayout() { return collectibleLayout; }

	EnemyFactory* getEnemyFactory() { return enemyFactory; }
	CollectibleFactory* getCollectibleFactory() { return collectibleFactory; }
	ObstacleFactory* getObstacleFactory() { return obstacleFactory; }

	int getHeight() const { return height; }
	int getWidth() const { return width; }

	Level* getLevel() { return level; }
};

