#pragma once
// BuildLevel.h
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
#include "Character.h"
#include "EntityFactory.h"

using namespace std;
using namespace sf;

class BuildLevel {
private:

    EntityFactory* entityFactory;

    // Counts
    int enemyCount;
    int collectibleCount;
    int obstacleCount;

    // Level dimensions
    int height;
    int width;
    int currentLevel;

    // Level data (loaded from files)
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
            height = 14;
            width = 250;
        }
        else if (levelNum == 3) {
            height = 14;
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

public:
    BuildLevel() :
        entityFactory(nullptr),
        enemyCount(0),
        collectibleCount(0),
        obstacleCount(0),
        height(0),
        width(0),
        currentLevel(0),
        enemyLayout(nullptr),
        collectibleLayout(nullptr),
        obstacleLayout(nullptr),
        levelGrid(nullptr)
    {
        // Create the EntityFactory
        entityFactory = new EntityFactory();
    }

    ~BuildLevel() {
        cleanup();
    }

    void cleanup() {
        // Clean up EntityFactory
        if (entityFactory) {
            delete entityFactory;
            entityFactory = nullptr;
        }

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

        // Create a new EntityFactory
        entityFactory = new EntityFactory();

        // Load level data
        loadLevelData(levelNum);

        // Read level data
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                char ch;
                file.get(ch);

                if (file.eof()) { // if error while reading files
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

        // Initialize the entity factory
        entityFactory->initialize(levelNum, height, width);

        // Create entities using the factory
        entityFactory->createEntities(enemyLayout, collectibleLayout, obstacleLayout);

        return true;
    }

    void update(RenderWindow& window, float offsetX, float deltaTime, Player* player) {
        // Update all entities using the entity factory

        cout << "In buildLevel Update\n";
        if (entityFactory) {
            entityFactory->updateEntities(window, offsetX, deltaTime, player);
        }
    }

    bool isLevelComplete() {
        
        Collectibles** collectibles = entityFactory->getCollectibles();
        int collectibleCount = entityFactory->getCollectibleCount();

        bool allRingsCollected = true;
        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i] && !collectibles[i]->getIsCollected()) {
                allRingsCollected = false;
                break;
            }
        }

        // You would also check if player reached the end of the level
        // This is just a placeholder
        return false;
    }

    void nextLevel() {
        loadLevel(currentLevel + 1);
    }

    int getCurrentLevel() const {
        return currentLevel;
    }

    // Getters for entity arrays and counts
    int getEnemyCount() const { return entityFactory ? entityFactory->getEnemyCount() : 0; }
    int getCollectibleCount() const { return entityFactory ? entityFactory->getCollectibleCount() : 0; }
    int getObstacleCount() const { return entityFactory ? entityFactory->getObstacleCount() : 0; }

    Enemies** getEnemies() { return entityFactory ? entityFactory->getEnemies() : nullptr; }
    Collectibles** getCollectibles() { return entityFactory ? entityFactory->getCollectibles() : nullptr; }
    Obstacles** getObstacles() { return entityFactory ? entityFactory->getObstacles() : nullptr; }

    // Getters for layouts
    char** getEnemyLayout() { return enemyLayout; }
    char** getObstacleLayout() { return obstacleLayout; }
    char** getCollectiblesLayout() { return collectibleLayout; }
    char** getLevelGrid() { return levelGrid; }

    // Getter for entity factory
    EntityFactory* getEntityFactory() { return entityFactory; }

    // Getters for level dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Method to set PlayerFactory in EntityFactory
    void setPlayerFactory(PlayerFactory* playerFactory) {
        if (entityFactory) {
            entityFactory->setPlayerFactory(playerFactory);
        }
    }
};