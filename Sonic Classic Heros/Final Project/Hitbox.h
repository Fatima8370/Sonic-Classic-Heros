#pragma once
#include <iostream>

using namespace std;

class Hitbox {

private:

    float x, y;
    float width, height;

    float leftOffset, rightOffset;
    float topOffset, bottomOffset;


public:

    Hitbox(float x = 0.0f, float y = 0.0f, float w = 22.0f, float h = 32.0f) :
        x(x), y(y), width(w), height(h) {
        topOffset = 0.0f;
        bottomOffset = height;
        leftOffset = 0.0f;
        rightOffset = width;


        cout << "H" << endl;
    }

    ~Hitbox() {
        cout << "~H" << endl;
    }



    void setOffsets(float t, float b, float l, float r) {
        topOffset = t;
        bottomOffset = b;
        leftOffset = l;
        rightOffset = r;
    }



    void updateHitbox(float x, float y) {
        this->x = x;
        this->y = y;
    }



    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }



    //object to object ( player and enemies) 
    bool checkCollision(const Hitbox& other)const {

        return (x <= other.x + other.width && x + width >= other.x &&
            y <= other.y + other.height && y + height >= other.y);
    }



    // for player and game objects ( exlcuding players , enemies ofcourse)
    bool checkGridCollisions(char** grid, char object, const int cell_size = 64) const {
        // checking collision at top left right middle 
        // bottom left right 
        // middle left right

        //lefts
        int topLeftX = static_cast<int>((x + leftOffset) / cell_size);
        int topLeftY = static_cast<int>((y + topOffset) / cell_size);
        int bottomLeftX = static_cast<int>((x + leftOffset) / cell_size);
        int bottomLeftY = static_cast<int>((y + bottomOffset) / cell_size);
        //rights
        int topRightX = static_cast<int>((x + rightOffset) / cell_size);
        int topRightY = static_cast<int>((y + topOffset) / cell_size);
        int bottomRightX = static_cast<int>((x + rightOffset) / cell_size);
        int bottomRightY = static_cast<int>((y + bottomOffset) / cell_size);

        //middles.
        int topMiddleX = static_cast<int>((x + width / 2) / cell_size);
        int topMiddleY = static_cast<int>((y + topOffset) / cell_size);

        int leftMiddleX = static_cast<int>((x + leftOffset) / cell_size);
        int leftMiddleY = static_cast<int> ((y + height / 2) / cell_size);

        int rightMiddleX = static_cast<int>((x + rightOffset) / cell_size);
        int rightMiddleY = static_cast<int> ((y + height / 2) / cell_size);

        int bottomMiddleX = static_cast<int>((x + width / 2) / cell_size);
        int bottomMiddleY = static_cast<int>((y + bottomOffset) / cell_size);

        if (grid[topLeftY][topLeftX] == object || grid[topRightY][topRightX] == object ||

            grid[bottomLeftY][bottomLeftX] == object || grid[bottomRightY][bottomRightX] == object ||

            grid[leftMiddleX][leftMiddleY] == object || grid[rightMiddleX][rightMiddleY] == object ||

            grid[bottomMiddleX][bottomMiddleY] == object || grid[topMiddleY][topMiddleX] == object)
        {
            return true;
        }
        return false;
    }
    

    // for the gravity detection
    bool checkBottomCollision(char** grid, char object, const int cell_size = 64) const {

        int bottomMiddleX = static_cast<int>((x + width / 2) / cell_size);
        int bottomMiddleY = static_cast<int>((y + bottomOffset) / cell_size);

        int bottomLeftX = static_cast<int>((x + leftOffset) / cell_size);
        int bottomLeftY = static_cast<int>((y + bottomOffset) / cell_size);

        int bottomRightX = static_cast<int>((x + rightOffset) / cell_size);
        int bottomRightY = static_cast<int>((y + bottomOffset) / cell_size);

        if (grid[bottomLeftY][bottomLeftX] == object || grid[bottomRightY][bottomRightX] == object || grid[bottomMiddleX][bottomMiddleY] == object) {
            return true;
        }
        return false;
    }

};