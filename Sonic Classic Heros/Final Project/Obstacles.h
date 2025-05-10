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
class Obstacles : public GameEntity {
protected:
    char type;
    Sprite sprite;
    Texture texture;
    float x, y;

    bool isColliding;

public:
    Obstacles() {
		hitbox = Hitbox(x, y, 64, 64);
    }

    void draw(RenderWindow& window, float directionFaced, float offsetX) override { }

    void update(char** grid, const int cell_size = 64) override {}

    virtual void setPosition(float xPos, float yPos) {
        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
    }

   virtual void render(RenderWindow& window, float offset)  override{
        sprite.setPosition(x - offset, y);  // Apply offset for scrolling
        window.draw(sprite);
    }

    virtual void checkCollision(Player* player) = 0;




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

        if (player->getHitbox().checkCollision(hitbox) ) {


            // If wall is breakable and player is attacking, break it
            if (breakable && player->isAttacking()) {
                
                // knuckles implementable


            }

			if (hitbox.checkAllSidesCollision(player->getHitbox())) {
				// Player is colliding with the wall
				isColliding = true; // Set collision flag
				player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
				player->stopFalling();
			}
			else {
				isColliding = false; // Reset collision flag if not colliding
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

        if (player->getHitbox().checkBottomCollision(hitbox)) {

			isColliding = true; // Set collision flag
            player->takeDamage(10); // Assuming you have a takeDamage method
			player->stopFalling(); // Stop player from falling
            player->stopJumping();
        }
        else {
			isColliding = false; // Reset collision flag if not colliding
			player->setVelocityX(0.0f);
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
    void checkCollision(Player* player) override {


        if (player->getHitbox().checkBottomCollision(hitbox) &&
            hitbox.checkTopCollision(player->getHitbox())) {

            isColliding = true; // Set collision flag

            player->setPosition(hitbox.getX(), hitbox.getY() - hitbox.getHeight());
            player->stopFalling();
        }
    }


};


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


        if (player->getHitbox().checkBottomCollision(hitbox) &&
            hitbox.checkTopCollision(player->getHitbox())) {

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
    float usedPositions[MAX_OBSTACLES][2]; // Store x,y coordinates
    int count;
    int gridHeight, gridWidth;
    int levelNum;

public:
    ObstacleFactory(int obstacleCount, int height , int width , int lvl )
        : count(0), gridHeight(height), gridWidth(width), levelNum(lvl)
    {
        for (int i = 0; i < MAX_OBSTACLES; ++i)
            obstacles[i] = nullptr;
        cout << "In Obstacle Factory\n";
    }

    ~ObstacleFactory() {
        clearObstacles();
    }

    Obstacles* createObstacle(const char& type, float spawnX, float spawnY) {
        Obstacles* obstacle = nullptr;

        switch (type) {
        case 'W': obstacle = new Wall(); break;
        case 'S': obstacle = new Spikes(); break;
        case 'P': obstacle = new Platform(); break;
        case 'T': obstacle = new LevelTrigger(); break;
        case 'B': obstacle = new Wall(true); break;
        }

        if (obstacle) {
            obstacle->setPosition(spawnX, spawnY);
            cout << "Created " << type << " at (" << spawnX << ", " << spawnY << ")" << endl;

            // Store the position for the created obstacle
            if (count < MAX_OBSTACLES) {
                obstacles[count] = obstacle;
                usedPositions[count][0] = spawnX;
                usedPositions[count][1] = spawnY;
                count++;
            }
        }

        return obstacle;
    }

    void updateObstacles(RenderWindow& window, Obstacles* obstacle[], int obstacleCount,
        float offsetX, Player* player) {
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacle[i]) {
                // Render the obstacle with scrolling offset
                obstacle[i]->render(window, offsetX);

                // Check collision with player
                obstacle[i]->checkCollision(player);
            }
        }
    }

    void clearObstacles() {
        for (int i = 0; i < count; i++) {
            if (obstacles[i]) {
                delete obstacles[i];
                obstacles[i] = nullptr;
            }
        }
        count = 0;
        cout << "All obstacles cleared" << endl;
    }

    int getCount() const { return count; }
    Obstacles** getAllObstacles() { return obstacles; }
};