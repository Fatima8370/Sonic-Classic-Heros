#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameEngine.h"

using namespace std;
using namespace sf;

class Level {


protected:

    Time timer;
    int levelWidth;
    int levelHeight;
    float gravity;
    float friction;

    int lvlNum;

public:
    Level(int width, int height, int i) : levelWidth(width), 
        levelHeight(height), gravity(0.98f), friction(0.1f) 
    { lvlNum = i; }

    ~Level() {
      
    }

    void loadLevel() {
        std::cout << "Level loaded.\n";
    }

    void renderLevel() {
        
    }

    void updateLevel() {
        std::cout << "Level updated.\n";
    }

    void updateTimer() {

    }

    bool checkWin() {
        return false;
    }

    bool checkLose() {
        return false;
    }

    int getWidth() const {
        return levelWidth;
    }

    int getHeight() const {
        return levelHeight;
    }

    
};
