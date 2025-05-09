#pragma once

#include "Level.h"
#include "BuildLevel.h"
#include "Character.h"
#include "Hitbox.h"
#include "GameEntity.h"

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


/*

Wall (unbreakable) : when player collides with it moving horizontally, it is not able to move 
Wall (breakable) : when player collides, specifically knuckles, wall breaks 
Spikes : player takes damage when collide from top 
Platform : Player can jump from underneath to the top to stand on the platform , top collision? 
LevelTrigger : the bricks fall down after top collision

all used using hitbox
*/

// Base class for all obstacles
class Obstacles {
protected:
    char type;
    Sprite sprite;
    Texture texture;
    float x, y;

    bool isColliding;

public:
    virtual void setPosition(float xPos, float yPos) {
        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
    }

   virtual void render(RenderWindow& window, float offset)  {
        sprite.setPosition(x - offset, y);  // Apply offset for scrolling
        window.draw(sprite);
    }

    virtual void checkCollision(Player* player)  {};

	bool isCollidingWithPlayer() const {
        return isColliding;
	}

    virtual ~Obstacles() {}
};

// Wall class
class Wall : public Obstacles {
    bool breakable;

public:
    Wall(bool isBreakable = false) {
        type = 'W';
        breakable = isBreakable;
        if (breakable)
        {
            type = 'B';
            texture.loadFromFile("Data/breakable_wall.png");
        }
        else
            texture.loadFromFile("Data/brick2.png");

        sprite.setTexture(texture);
    }

	bool isBreakable() const {
		return breakable;
	}

    void checkCollision(Player* player) override {

        Hitbox hitbox = player->getHitbox();

        float charLeft = hitbox.getX();
        float charRight = charLeft + hitbox.getWidth();
        float charTop = hitbox.getY();
        float charBottom = charTop + hitbox.getHeight();

        // Wall boundaries
        float wallLeft = x;
        float wallRight = x + sprite.getLocalBounds().width;
        float wallTop = y;
        float wallBottom = y + sprite.getLocalBounds().height;

        // Check for overlap
        if (charRight > wallLeft && charLeft < wallRight &&
            charBottom > wallTop && charTop < wallBottom) {

			isColliding = true; // Set collision flag

            // Calculate overlap amounts
            float overlapLeft = charRight - wallLeft;
            float overlapRight = wallRight - charLeft;
            float overlapTop = charBottom - wallTop;
            float overlapBottom = wallBottom - charTop;

            // Find smallest overlap to determine push direction
            float minOverlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });

            // Apply collision resolution based on smallest overlap
            if (minOverlap == overlapLeft) {
                // Colliding from left
                player->setPosition(wallLeft - hitbox.getWidth(), charTop);
            }
            else if (minOverlap == overlapRight) {
                // Colliding from right
                player->setPosition(wallRight, charTop);
            }
            else if (minOverlap == overlapTop) {
                // Colliding from top
                player->setPosition(charLeft, wallTop - hitbox.getHeight());
                player->stopFalling(); // Player landed on top
            }
            else if (minOverlap == overlapBottom) {
                // Colliding from bottom
                player->setPosition(charLeft, wallBottom);
                player->stopJumping(); // Player hit their head
            }

            // If wall is breakable and player is attacking, break it
            if (breakable && player->isAttacking()) {
                // Mark wall for deletion or play break animation
                // This depends on how you manage destruction
            }
        }
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

    void checkCollision(Player * player) override {

        Hitbox hitbox = player->getHitbox();


        float charLeft = hitbox.getX();
        float charRight = charLeft + hitbox.getWidth();
        float charTop = hitbox.getY();
        float charBottom = charTop + hitbox.getHeight();

        // Spike boundaries
        float spikeLeft = x;
        float spikeRight = x + 64;
        float spikeTop = y;
        float spikeBottom = y + 64;

        // Check for overlap
        if (charRight > spikeLeft && charLeft < spikeRight &&
            charBottom > spikeTop && charTop < spikeBottom) {
            // Player touched spikes - apply damage
			isColliding = true; // Set collision flag
            player->takeDamage(10); // Assuming you have a takeDamage method
        }
    }
};

// Platform class (Now correctly using 64x64 dimensions)
class Platform : public Obstacles{
private:
    float x, y;   // Position of the platform (top-left corner)
    int width, height; // Size of the platform (width and height)

public:
    Platform() {

        type = 'P';
        texture.loadFromFile("Data/brick1.png");
        sprite.setTexture(texture);

    }



    // Check if hitbox collides with the platform (lands on top of it)
    void checkCollision(Player * player) override{

        Hitbox hitbox = player->getHitbox();


        float charLeft = hitbox.getX();
        float charRight = charLeft + hitbox.getWidth();
        float charTop = hitbox.getY();
        float charBottom = charTop + hitbox.getHeight();

        // Platform boundaries
        float platformLeft = x;
        float platformRight = x + sprite.getLocalBounds().width;
        float platformTop = y;
        float platformBottom = y + sprite.getLocalBounds().height;

        // Check if character is falling (moving downward)
        bool isFalling = player->getVelocityY() > 0;

        // Check for top collision (only when falling)
        if (isFalling &&
            charRight > platformLeft && charLeft < platformRight &&
            charBottom >= platformTop && charBottom <= platformTop + 10) {
            // Character landed on platform
			isColliding = true; // Set collision flag
            player->setPosition(charLeft, platformTop - hitbox.getHeight());
            player->stopFalling();
        }
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
        type = 'T';  // Assign the type of obstacle
        width = 64;
        height = 64;

        // Load the texture
        if (!texture.loadFromFile("Data/ground.png")) {
            std::cerr << "Error loading level trigger texture!" << std::endl;
        }
        sprite.setTexture(texture);
    }

    void checkCollision(Player * player) override {
        // Detect if the hitbox interacts with the level trigger

        Hitbox hitbox = player->getHitbox();

        float charLeft = hitbox.getX();
        float charRight = charLeft + hitbox.getWidth();
        float charTop = hitbox.getY();
        float charBottom = charTop + hitbox.getHeight();

        float triggerLeft = x;
        float triggerRight = triggerLeft + width;
        float triggerTop = y;
        float triggerBottom = triggerTop + height;

        if (charRight > triggerLeft && charLeft < triggerRight &&
            charBottom > triggerTop && charTop < triggerBottom) {

			isColliding = true; // Set collision flag

            if (!triggered) {
                triggered = true;
                std::cout << "Level Trigger Activated!" << std::endl;
                if (triggered) {
                    // Move the trigger off-screen
                    sprite.move(0, fallSpeed);
                }
            }
        }
    }

	bool isTriggered() const {
		return triggered;
	}

    
};


class ObstacleFactory {
private:
    static const int MAX_OBSTACLES = 500;
    Obstacles* obstacles[MAX_OBSTACLES];
    int count;

public:
    ObstacleFactory(int count) {
        this->count = count;
    }

    ~ObstacleFactory() {
        count = 0;
    }

    //Factory + spawn combined
    Obstacles* createObstacle(char type, float x, float y) {

        Obstacles* obstacle = nullptr;

        switch (type) {
        case 'W': obstacle = new Wall(); break;
        case 'S': obstacle = new Spikes(); break;
        case 'P': obstacle = new Platform(); break;
        case 'T': obstacle = new LevelTrigger(); break;
        case 'B': obstacle = new Wall(true); break;

        }
        obstacle->setPosition(x, y);

        return obstacle;
    }

    void checkAllCollisions(Player* player) {

        for (int i = 0; i < count; ++i) {
            if (obstacles[i])
                obstacles[i]->checkCollision(player);
        }
    }

    void renderAll(RenderWindow& window, float offset) {
        for (int i = 0; i < count; ++i) {
            if (obstacles[i])
                obstacles[i]->render(window, offset);
        }
    }

    void update(RenderWindow& window, Player* player, float offset, Obstacles* obstacles[]) {

        for (int i = 0; i < count; ++i) {
            if (obstacles[i]) {
                obstacles[i]->render(window, offset);
                obstacles[i]->checkCollision(player);
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