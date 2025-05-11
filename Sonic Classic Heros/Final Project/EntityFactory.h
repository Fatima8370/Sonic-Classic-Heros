#pragma once
// EntityFactory.h
#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Enemies.h"
#include "Collectibles.h"
#include "Obstacles.h"
#include "Character.h"

using namespace std;
using namespace sf;

class EntityFactory {
private:
    // Factories
    EnemyFactory* enemyFactory;
    CollectibleFactory* collectibleFactory;
    ObstacleFactory* obstacleFactory;

    PlayerFactory* playerFactory;

    // Entity arrays
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

public:
    EntityFactory() :
        enemyFactory(nullptr),
        collectibleFactory(nullptr),
        obstacleFactory(nullptr),
        playerFactory(nullptr),
        enemies(nullptr),
        collectibles(nullptr),
        obstacles(nullptr),
        enemyCount(0),
        collectibleCount(0),
        obstacleCount(0),
        height(0),
        width(0),
        currentLevel(0)
    {
    }

    ~EntityFactory() {
        cleanup();
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

        playerFactory = nullptr;

        // Clean up entities
        clearEntities();
    }

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

    // Initialize the entity factory for a specific level
    bool initialize(int levelNum, int h, int w) {
        // Store level info
        currentLevel = levelNum;
        height = h;
        width = w;

        // Clean up existing factories
        cleanup();

        // Create new factories
        enemyFactory = new EnemyFactory(height, width, levelNum);
        collectibleFactory = new CollectibleFactory(height, width, levelNum);
        obstacleFactory = new ObstacleFactory(height, width, levelNum);

        cout << "EntityFactory initialized for level " << levelNum
            << " with dimensions " << w << "x" << h << endl;

        return true;
    }

    // Set the player factory (typically created externally)
    void setPlayerFactory(PlayerFactory* factory) {
        playerFactory = factory;
    }

    // Create entities from layout grids
    void createEntities(char** enemyLayout, char** collectibleLayout, char** obstacleLayout) {

        countEntities(enemyLayout, collectibleLayout, obstacleLayout);


        enemies = new Enemies * [enemyCount]();
        collectibles = new Collectibles * [collectibleCount]();
        obstacles = new Obstacles * [obstacleCount]();

        // Create each entity type
        createEnemies(enemyLayout);
        createCollectibles(collectibleLayout);
        createObstacles(obstacleLayout);

        cout << "EntityFactory created " << enemyCount << " enemies, "
            << collectibleCount << " collectibles, and "
            << obstacleCount << " obstacles" << endl;
    }


    void countEntities(char** enemyLayout, char** collectibleLayout, char** obstacleLayout) {
        enemyCount = 0;
        collectibleCount = 0;
        obstacleCount = 0;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (enemyLayout[i][j] != '\0') enemyCount++;
                if (collectibleLayout[i][j] != '\0') collectibleCount++;
                if (obstacleLayout[i][j] != '\0') obstacleCount++;
            }
        }
    }


    void createEnemies(char** enemyLayout) {
        int index = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (enemyLayout[i][j] != '\0') {
                    Enemies* e = enemyFactory->createEnemy(enemyLayout[i][j], j * 64, i * 64);
                    if (e) {
                        enemies[index++] = e;
                    }
                }
            }
        }
    }


    void createCollectibles(char** collectibleLayout) {
        int index = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (collectibleLayout[i][j] != '\0') {
                    Collectibles* c = collectibleFactory->createCollectible(collectibleLayout[i][j], j * 64, i * 64);
                    if (c) {
                        collectibles[index++] = c;
                    }
                }
            }
        }
    }


    void createObstacles(char** obstacleLayout) {
        int index = 0;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (obstacleLayout[i][j] != '\0') {
                    Obstacles* o = obstacleFactory->createObstacle(obstacleLayout[i][j], j * 64, i * 64);
                    if (o) {
                        obstacles[index++] = o;
                    }
                }
            }
        }
    }

    // Update all entities
    void updateEntities(RenderWindow& window, float offsetX, float deltaTime, Player* player) {
        // Update enemies
        if (enemyFactory && enemies) {
            enemyFactory->updateEnemies(window, enemies, enemyCount, offsetX, deltaTime, player);
        }

        // Update collectibles
        if (collectibleFactory && collectibles) {
            collectibleFactory->updateCollectibles(window, collectibles, collectibleCount, offsetX, deltaTime, player);
        }

        // Update obstacles
        if (obstacleFactory && obstacles) {
            obstacleFactory->updateObstacles(window, obstacles, obstacleCount, offsetX, player);
        }
    }

    // Getters
    int getEnemyCount() const { return enemyCount; }
    int getCollectibleCount() const { return collectibleCount; }
    int getObstacleCount() const { return obstacleCount; }

    Enemies** getEnemies() { return enemies; }
    Collectibles** getCollectibles() { return collectibles; }
    Obstacles** getObstacles() { return obstacles; }

    PlayerFactory* getPlayerFactory() { return playerFactory; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCurrentLevel() const { return currentLevel; }
};