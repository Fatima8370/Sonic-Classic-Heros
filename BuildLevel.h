#pragma once
#include <iostream>

#include "Level.h"

using namespace std;

class LevelGenerator {
public:
    static char** generateLevel(int levelNumber) {
        // Set dimensions based on the level number
        int width, height;
        if (levelNumber == 1) {
            width = 200;
            height = 14;
        }
        else if (levelNumber == 2) {
            width = 250;
            height = 14;
        }
        else if (levelNumber == 3) {
            width = 300;
            height = 14;
        }
        else if (levelNumber == 4) {  // Boss level
            width = 30;
            height = 14;
        }

        // Dynamically allocate the grid (2D array)
        char** grid = new char* [height];
        for (int i = 0; i < height; ++i) {
            grid[i] = new char[width];
            for (int j = 0; j < width; ++j) {
                grid[i][j] = ' ';  // Empty space
            }
        }

        // For non-boss levels (level 1, 2, 3)
        if (levelNumber == 4) {  // Boss level
            // Platforms in the middle of the screen and spikes below them
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (j > 10 && j < 20) {
                        grid[i][j] = 'p';  // Platforms in the middle
                    }
                    else if (i == height - 1) {
                        grid[i][j] = 's';  // Spikes below the platform
                    }
                }
            }
        }
        else {
            // Randomly place obstacles for non-boss levels
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    int randVal = rand() % 100;

                    if (randVal < 3) {
                        grid[i][j] = 'b';  // Bottomless pit (3% chance)
                    }
                    else if (randVal < 10) {
                        grid[i][j] = 's';  // Spikes (10% chance)
                    }
                    else if (randVal < 20) {
                        grid[i][j] = 'w';  // Wall (10% chance)
                    }
                    else if (randVal < 30) {
                        grid[i][j] = 'p';  // Platform (10% chance)
                    }
                    else if (randVal < 35) {
                        grid[i][j] = 't';  // Trigger (5% chance)
                    }
                }
            }
        }

        return grid;
    }

    // Function to delete the grid after use
    static void deleteGrid(char** grid, int height) {
        for (int i = 0; i < height; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
    }
};
