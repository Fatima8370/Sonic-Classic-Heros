#pragma once

#include "Level.h"
#include "BuildLevel.h"
#include "Character.h"
#include "Hitbox.h"

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

// Base class for all obstacles
class Obstacles {
protected:
    char type;
    Sprite sprite;
    Texture texture;
    float x, y;

public:
    virtual void setPosition(float xPos, float yPos) {
        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
    }

    virtual void render(RenderWindow& window, float offset) {
        sprite.setPosition(x - offset, y);  // Apply offset for scrolling
        window.draw(sprite);
    }

    virtual void checkCollision(Hitbox* hitbox) = 0;

    virtual ~Obstacles() {}
};

// Wall class
class Wall : public Obstacles {
    bool breakable;

public:
    Wall(bool isBreakable = false) {
        type = 'w';
        breakable = isBreakable;
        if (breakable)
            texture.loadFromFile("Data/breakable_wall.png");
        else
            texture.loadFromFile("Data/brick2.png");

        sprite.setTexture(texture);
    }

    void checkCollision(Hitbox* hitbox) override {
        // Wall-specific collision logic can go here
    }
};

// Spikes class
class Spikes : public Obstacles {
public:
    Spikes() {
        type = 's';
        texture.loadFromFile("Data/spike.png");
        sprite.setTexture(texture);
    }

    void checkCollision(Hitbox* hitbox) override {
        // Damage player if they collide with spikes (add the logic for it)
    }
};

// Platform class (Now correctly using 64x64 dimensions)
class Platform : public Obstacles{
private:
    float x, y;   // Position of the platform (top-left corner)
    int width, height; // Size of the platform (width and height)

public:
    Platform() {

        type = 'p';
        texture.loadFromFile("Data/brick2.png");
        sprite.setTexture(texture);

    }



    // Check if hitbox collides with the platform (lands on top of it)
    void checkCollision(Hitbox* hitbox) override{
    //    // Hitbox's boundaries
    //    float charLeft = hitbox->getX();
    //    float charRight = charLeft + hitbox->getWidth();
    //    float charTop = hitbox->getY();
    //    float charBottom = charTop + hitbox->getHeight();

    //    // Platform's boundaries (the position and size)
    //    float platformLeft = this->getX();
    //    float platformRight = platformLeft + this->getWidth();
    //    float platformTop = this->getY();
    //    float platformBottom = platformTop + this->getHeight();

    //    // Check if the hitbox is falling onto the platform from above
    //    if (charRight > platformLeft && charLeft < platformRight &&
    //        charBottom <= platformTop && charBottom + 5 > platformTop) {
    //        // The hitbox has landed on the platform
    //        hitbox->setPosition(charLeft, platformTop - hitbox->getHeight());  // Place hitbox on top of the platform
    //        hitbox->stopFalling();  // Stop the falling motion
    //    }

    //    // Check if the hitbox has fallen into a bottomless pit ('b')
    //    int charX = static_cast<int>(charLeft / 32);  // Assuming each grid cell is 32px wide
    //    int charY = static_cast<int>(charBottom / 32);  // Assuming each grid cell is 32px tall

    //    // Ensure we are within bounds of the grid before accessing
    //    if (charX >= 0 && charX < gridWidth && charY < gridHeight && grid[charY][charX] == 'b') {
    //        // The hitbox has fallen into a bottomless pit
    //        hitbox->die();  // Call a method to handle player death
    //    }
    }
};


// LevelTrigger class (128x45 as per specifications)
class LevelTrigger : public Obstacles {
private:
    float width, height;
    bool triggered;
    float fallSpeed;

public:
    LevelTrigger() : triggered(false), fallSpeed(2.0f) {
        type = 't';  // Assign the type of obstacle
        width = 128;
        height = 45;

        // Load the texture
        if (!texture.loadFromFile("Data/level_triggers.png")) {
            std::cerr << "Error loading level trigger texture!" << std::endl;
        }
        sprite.setTexture(texture);
    }

    void checkCollision(Hitbox* hitbox) override {
        // Detect if the hitbox interacts with the level trigger
        float charLeft = hitbox->getX();
        float charRight = charLeft + hitbox->getWidth();
        float charTop = hitbox->getY();
        float charBottom = charTop + hitbox->getHeight();

        float triggerLeft = x;
        float triggerRight = triggerLeft + width;
        float triggerTop = y;
        float triggerBottom = triggerTop + height;

        if (charRight > triggerLeft && charLeft < triggerRight &&
            charBottom > triggerTop && charTop < triggerBottom) {
            if (!triggered) {
                triggered = true;
                std::cout << "Level Trigger Activated!" << std::endl;
            }
        }
    }

    void update() {
        if (triggered) {
            // Move the trigger off-screen
            sprite.move(0, fallSpeed);
        }
    }

    void render(RenderWindow& window, float offset) override {
        sprite.setPosition(x - offset, y);
        window.draw(sprite);
    }
};


class ObstacleFactory {
private:
    static const int MAX_OBSTACLES = 100;
    Obstacles* obstacles[MAX_OBSTACLES];
    int count;

public:
    ObstacleFactory() {
        count = 0;
        for (int i = 0; i < MAX_OBSTACLES; ++i)
            obstacles[i] = nullptr;
    }

    ~ObstacleFactory() {
        for (int i = 0; i < count; ++i)
            delete obstacles[i];
    }

    // Factory + spawn combined
    void spawnObstacle(char type, float x, float y, bool isBreakable = false) {
        if (count >= MAX_OBSTACLES) return;

        Obstacles* obstacle = nullptr;

        switch (type) {
        case 'w': obstacle = new Wall(isBreakable); break;
        case 's': obstacle = new Spikes(); break;
        case 'p': obstacle = new Platform(); break;
        case 't': obstacle = new LevelTrigger(); break;
        }

        if (obstacle) {
            obstacle->setPosition(x, y);
            obstacles[count++] = obstacle;
        }
    }

    void checkAllCollisions(Hitbox* hitbox) {
        for (int i = 0; i < count; ++i) {
            if (obstacles[i])
                obstacles[i]->checkCollision(hitbox);
        }
    }

    void renderAll(sf::RenderWindow& window, float offset) {
        for (int i = 0; i < count; ++i) {
            if (obstacles[i])
                obstacles[i]->render(window, offset);
        }
    }

    void updateAll() {
        for (int i = 0; i < count; ++i) {
            if (LevelTrigger* t = dynamic_cast<LevelTrigger*>(obstacles[i])) {
                t->update();
            }
        }
    }

    Obstacles** getAllObstacles() {
        return obstacles;
    }

    int getCount() const {
        return count;
    }
};

