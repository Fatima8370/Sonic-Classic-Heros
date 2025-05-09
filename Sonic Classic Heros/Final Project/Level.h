#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Obstacles.h"
#include "BuildLevel.h"

using namespace std;    
using namespace sf; 


// Level class to manage the game level, including loading, rendering, and updating the level state

// grid will be a 2D array of enemies, blocks, and collectibles representing the level layout

// will be separateed into different arrays for enemies, collectibles, and blocks

// factories will be used to create enemies , block and collectibles
// will be used to create the level layout

class Level {
private:
    int timer;
    int levelWidth;
    int levelHeight;
    char** grid;
    float gravity;
    float friction;

public:
    Level(int width, int height);
    ~Level();

    void loadLevel();
    void renderLevel();
    void updateLevel();
    void updateTimer();
    bool checkWin();
    bool checkLose();


    int getWidth() const;
    int getHeight() const;
    char getCell(int x, int y) const;
};


Level::Level(int width, int height)
    : levelWidth(width), levelHeight(height), timer(0), gravity(0.98f), friction(0.1f) {
    grid = new char* [levelHeight];
    for (int i = 0; i < levelHeight; ++i) {
        grid[i] = new char[levelWidth];
        for (int j = 0; j < levelWidth; ++j) {
            grid[i][j] = ' ';  // Default empty cell
        }
    }
}

Level::~Level() {
    for (int i = 0; i < levelHeight; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
}

void Level::loadLevel() {
    // Hardcoded or file-based level loading logic
    std::cout << "Level loaded.\n";
}

void Level::renderLevel() {
    for (int i = 0; i < levelHeight; ++i) {
        for (int j = 0; j < levelWidth; ++j) {
            std::cout << grid[i][j];
        }
        std::cout << std::endl;
    }
}

void Level::updateLevel() {
    // Apply gravity or friction if needed
    std::cout << "Level updated.\n";
}

void Level::updateTimer() {
    ++timer;
}

bool Level::checkWin() {
    // Placeholder win condition
    return false;
}

bool Level::checkLose() {
    // Placeholder lose condition
    return false;
}

int Level::getWidth() const {
    return levelWidth;
}

int Level::getHeight() const {
    return levelHeight;
}

char Level::getCell(int x, int y) const {
    if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight) {
        return grid[y][x];
    }
    return ' ';
}