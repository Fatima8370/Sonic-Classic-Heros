//#pragma once
//
//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include "Obstacles.h"
//#include "BuildLevel.h"
//
//#define MAX_OBSTACLES 500
//
//class Level {
//    char** lvl;
//    int width, height;
//    const int cellSize = 64;
//    int lvlNum;
//
//    Obstacles* obstacleList[MAX_OBSTACLES];
//    int obstacleCount;
//
//public:
//    inline Level(int i) : lvlNum(i), obstacleCount(0) {
//        switch (lvlNum) {
//        case 1: width = 200; height = 14; break;
//        case 2: width = 250; height = 14; break;
//        case 3: width = 300; height = 14; break;
//        case 4: width = 20;  height = 14; break;
//        default: width = 0; height = 0; break;
//        }
//
//        lvl = new char* [height];
//        for (int i = 0; i < height; ++i)
//            lvl[i] = new char[width] { '\0' };
//    }
//
//    inline void setCell(int row, int col, char symbol) {
//        if (row >= 0 && row < height && col >= 0 && col < width)
//            lvl[row][col] = symbol;
//    }
//
//    inline void generateObjectsFromGrid() {
//        LevelLayoutBuilder::buildLevel(this, lvlNum);
//
//        obstacleCount = 0;
//        for (int i = 0; i < height; ++i)
//            for (int j = 0; j < width; ++j)
//                spawnObstacle(lvl[i][j], j, i);
//    }
//
//    inline void spawnObstacle(char type, int x, int y) {
//        if (obstacleCount >= MAX_OBSTACLES) return;
//
//        Obstacles* obj = nullptr;
//        switch (type) {
//        case 'w': obj = new Wall(false); break;
//        case 'b': obj = new Wall(true); break;
//        case 'p': obj = new Platform(); break;
//        case 'x': obj = new Spikes(); break;
//        case 'o': obj = new BottomlessPit(); break;
//        default: return;
//        }
//
//        if (obj) {
//            obj->setPosition(x * cellSize, y * cellSize);
//            obstacleList[obstacleCount++] = obj;
//        }
//    }
//
//    inline void renderObstacles(sf::RenderWindow& window, float offset) {
//        for (int i = 0; i < obstacleCount; ++i)
//            obstacleList[i]->render(window, offset);
//    }
//
//    inline int getWidth() const { return width; }
//    inline int getHeight() const { return height; }
//
//    void renderLevel() {
//        // Display characters, enemies, grid etc.
//    }
//
//    ~Level() {
//        for (int i = 0; i < obstacleCount; ++i)
//            delete obstacleList[i];
//        for (int i = 0; i < height; ++i)
//            delete[] lvl[i];
//        delete[] lvl;
//    }
//};
