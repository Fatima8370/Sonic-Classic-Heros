#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class Hitbox {

private:
    float x, y;
    float width, height;

    float leftOffset, rightOffset;
    float topOffset, bottomOffset;

public:
    Hitbox(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f) : x(x), y(y), width(w), height(h) {

        topOffset = 0.0f;
        bottomOffset = height;
        leftOffset = 0.0f;
        rightOffset = width;

    }

    ~Hitbox() {}

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

    // Get actual hitbox dimensions with offsets
    float getLeft() const { return x + leftOffset; }
    float getRight() const { return x + rightOffset; }
    float getTop() const { return y + topOffset; }
    float getBottom() const { return y + bottomOffset; }
    float getCenterX() const { return getLeft() + (getRight() - getLeft()) / 2; }
    float getCenterY() const { return getTop() + (getBottom() - getTop()) / 2; }


    bool checkCollision(const Hitbox& other) const {
        // Calculate actual hitbox coordinates with offsets for this object
        float thisLeft = getLeft();
        float thisRight = getRight();
        float thisTop = getTop();
        float thisBottom = getBottom();

        // Calculate actual hitbox coordinates with offsets for the other object
        float otherLeft = other.getLeft();
        float otherRight = other.getRight();
        float otherTop = other.getTop();
        float otherBottom = other.getBottom();

        // Check if the actual hitboxes overlap
        return (thisLeft < otherRight && thisRight > otherLeft &&
            thisTop < otherBottom && thisBottom > otherTop);
    }

    bool checkAllSidesCollision(const Hitbox& other) const {
        return checkCollision(other);
    }


    bool checkTopCollision(const Hitbox& other) const {

        float thisLeft = getLeft();
        float thisRight = getRight();
        float thisTop = getTop();


        float otherLeft = other.getLeft();
        float otherRight = other.getRight();
        float otherTop = other.getTop();
        float otherBottom = other.getBottom();

        // Create a small hitbox representing just the top edge of this hitbox
        float topEdgeHeight = 1.0f; // Small height for the top edge


        return (thisTop < otherBottom && thisTop + topEdgeHeight > otherTop &&
            thisLeft < otherRight && thisRight > otherLeft);
    }


    bool checkBottomCollision(const Hitbox& other) const {

        float thisLeft = getLeft();
        float thisRight = getRight();
        float thisBottom = getBottom();


        float otherLeft = other.getLeft();
        float otherRight = other.getRight();
        float otherTop = other.getTop();
        float otherBottom = other.getBottom();


        float bottomEdgeHeight = 1.0f; 


        return (thisBottom - bottomEdgeHeight < otherBottom && thisBottom > otherTop &&
            thisLeft < otherRight && thisRight > otherLeft);
    }


    bool checkLeftCollision(const Hitbox& other) const {

        float thisLeft = getLeft();
        float thisTop = getTop();
        float thisBottom = getBottom();


        float otherLeft = other.getLeft();
        float otherRight = other.getRight();
        float otherTop = other.getTop();
        float otherBottom = other.getBottom();


        float leftEdgeWidth = 1.0f; 


        return (thisLeft < otherRight && thisLeft + leftEdgeWidth > otherLeft &&
            thisTop < otherBottom && thisBottom > otherTop);
    }


    bool checkRightCollision(const Hitbox& other) const {

        float thisRight = getRight();
        float thisTop = getTop();
        float thisBottom = getBottom();


        float otherLeft = other.getLeft();
        float otherRight = other.getRight();
        float otherTop = other.getTop();
        float otherBottom = other.getBottom();


        float rightEdgeWidth = 1.0f; 


        return (thisRight - rightEdgeWidth < otherRight && thisRight > otherLeft &&
            thisTop < otherBottom && thisBottom > otherTop);
    }



    float getTopCollisionPoint(char** grid, const int cell_size, const int maxHeight = 14, const int maxWidth = 300) const {
        int topLeftY = static_cast<int>((y + topOffset) / cell_size);

        if (checkTopCollision(grid, cell_size, maxHeight, maxWidth)) {
            return (topLeftY + 1) * cell_size;
        }

        return -1.0f; 
    }

    bool checkTopCollision(char** grid, const int cell_size, const int maxHeight = 14, const int maxWidth = 300) const {

        int topLeftX = static_cast<int>((x + leftOffset) / cell_size);
        int topLeftY = static_cast<int>((y + topOffset) / cell_size);
        int topMiddleX = static_cast<int>((x + width / 2) / cell_size);
        int topMiddleY = static_cast<int>((y + topOffset) / cell_size);
        int topRightX = static_cast<int>((x + rightOffset) / cell_size);
        int topRightY = static_cast<int>((y + topOffset) / cell_size);

        if (topLeftY < 0 || topLeftY >= maxHeight || topLeftX < 0 || topLeftX >= maxWidth ||
            topMiddleY < 0 || topMiddleY >= maxHeight || topMiddleX < 0 || topMiddleX >= maxWidth ||
            topRightY < 0 || topRightY >= maxHeight || topRightX < 0 || topRightX >= maxWidth) {
            return false;
        }

        return (grid[topLeftY][topLeftX] == 'W' || grid[topLeftY][topLeftX] == 'T' || grid[topLeftY][topLeftX] == 'P' ||
            grid[topMiddleY][topMiddleX] == 'W' || grid[topMiddleY][topMiddleX] == 'T' || grid[topMiddleY][topMiddleX] == 'P' ||
            grid[topRightY][topRightX] == 'W' || grid[topRightY][topRightX] == 'T' || grid[topRightY][topRightX] == 'P');
    }









    float getTopCollisionPoint(const Hitbox& other) const {
        if (checkTopCollision(other)) {
            return other.getBottom();
        }
        return -1.0f; 

    }

    float getBottomCollisionPoint(const Hitbox& other) const {
        if (checkBottomCollision(other)) {
            return other.getTop();
        }
        return -1.0f; 
    }

    float getLeftCollisionPoint(const Hitbox& other) const {
        if (checkLeftCollision(other)) {
            return other.getRight();
        }
        return -1.0f;

    }

    float getRightCollisionPoint(const Hitbox& other) const {
        if (checkRightCollision(other)) {
            return other.getLeft();
        }
        return -1.0f; 

    }
};