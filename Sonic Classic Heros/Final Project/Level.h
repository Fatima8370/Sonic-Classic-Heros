#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Obstacles.h"
#include "BuildLevel.h"


class Level {

    int MAX_OBSTACLES;

    char** lvl;
    int width, height;
    const int cellSize = 64;
    int lvlNum;

    Obstacles* obstacleList [MAX_OBSTACLES];

	ObstacleFactory* obsFactory;
    int obstacleCount;


public:
   


};
