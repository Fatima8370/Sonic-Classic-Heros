#pragma once    

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Character.h"
#include "Hitbox.h"
#include "GameEntity.h"

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

class Obstacles : public GameEntity {
protected:
    char type;
    Sprite sprite;
    Texture texture;
    float x, y;

    bool isColliding;

public:
    Obstacles() : isColliding(false) {
        hitbox = Hitbox(x, y, 64, 64);
    }

    void draw(RenderWindow& window, float directionFaced, float offsetX) override {}

    void update(char** grid, const int cell_size = 64) override {}

    virtual void setPosition(float xPos, float yPos) {
        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
        hitbox.updateHitbox(x, y);
    }

    void drawHitbox(RenderWindow& window, float offset) {
        RectangleShape hitboxRect;
        hitboxRect.setPosition(hitbox.getX() - offset, hitbox.getY());
        hitboxRect.setSize(Vector2f(hitbox.getWidth(), hitbox.getHeight()));
        hitboxRect.setFillColor(Color::Transparent);
        hitboxRect.setOutlineColor(Color::Red);
        hitboxRect.setOutlineThickness(1.0f);
        window.draw(hitboxRect);
    }

    virtual void render(RenderWindow& window, float offset) override {
        sprite.setPosition(x - offset, y);  // Apply offset for scrolling
        window.draw(sprite);
        drawHitbox(window, offset);
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
    Wall(bool isBreakable = false) : breakable(isBreakable) {
        type = isBreakable ? 'B' : 'W';

        if (breakable)
            texture.loadFromFile("Data/breakable_wall.png");
        else
            texture.loadFromFile("Data/brick2.png");

        sprite.setTexture(texture);
    }

    bool isBreakable() const {
        return breakable;
    }

    void checkCollision(Player* player) override {
        isColliding = false;

        if (player->getHitbox().checkCollision(hitbox)) {

            bool topCollision = hitbox.checkTopCollision(player->getHitbox());
            bool bottomCollision = hitbox.checkBottomCollision(player->getHitbox());
            bool leftCollision = hitbox.checkLeftCollision(player->getHitbox());
            bool rightCollision = hitbox.checkRightCollision(player->getHitbox());

            // If wall is breakable and player is attacking, break it
            if (breakable && player->isAttacking()) {
                // Implementation for breaking wall would go here
                // For now, just set a flag that it's being broken
                isColliding = true;
                return;
            }

            // Handle top collision (player landing on top of wall)
            if (topCollision) {
                isColliding = true;
                player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
                player->stopFalling();
				player->stopJumping();
                player->stopMoving();
            }
            // Handle side collisions (player walking into wall)
            else if (leftCollision) {
                isColliding = true;
                player->setPosition(hitbox.getX() - player->getHitbox().getWidth(), player->getY());
                player->setVelocityX(0);
                player->stopMoving();
                player->stopFalling();
                player->stopJumping();
            }
            else if (rightCollision) {
                isColliding = true;
                player->setPosition(hitbox.getX() + hitbox.getWidth(), player->getY());
                player->setVelocityX(0);
                player->stopMoving();
                player->stopFalling();
                player->stopJumping();
            }
            // Handle bottom collision (player jumping into wall from below)
            else if (bottomCollision) {
                isColliding = true;
                player->setPosition(player->getX(), hitbox.getY() + hitbox.getHeight());
                player->stopJumping();
				player->setVelocityY(0);
            }
        }
    }
};

// Spikes class
class Spikes : public Obstacles {
public:
    Spikes() {
        type = 'S';
        texture.loadFromFile("Data/spike.png");
        sprite.setTexture(texture);
    }

    void checkCollision(Player* player) override {
        isColliding = false;

        if (player->getHitbox().checkCollision(hitbox)) {
            // Check if player is colliding with the top of the spikes
            if (player->getHitbox().checkBottomCollision(hitbox)) {
                isColliding = true;
                player->takeDamage(10);
                player->stopFalling();
                player->stopJumping();
            }
            // Check for side collisions with spikes
            else if (hitbox.checkLeftCollision(player->getHitbox()) ||
                hitbox.checkRightCollision(player->getHitbox())) 
            {
                isColliding = true;
				player->setVelocityX(0);
                player->stopFalling();
                player->stopMoving();
				player->stopJumping();
            }
        }
    }
};

// Platform class
class Platform : public Obstacles {
public:
    Platform() {
        type = 'P';
        texture.loadFromFile("Data/brick1.png");
        sprite.setTexture(texture);
    }

    // Check if player collides with the platform (lands on top of it)
    void checkCollision(Player* player) override {


        if (player->getVelocityY() > 0 &&
            player->getHitbox().checkBottomCollision(hitbox) &&
            hitbox.checkTopCollision(player->getHitbox())) {

            player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
            player->stopFalling();
            player->stopMoving();
            player->stopJumping();
        }
    }
};

class LevelTrigger : public Obstacles {
private:
    bool triggered;
    float fallSpeed;

public:
    LevelTrigger() : triggered(false), fallSpeed(128.0f) {
        type = 'T';

        if (!texture.loadFromFile("Data/ground.png")) {
        }
        sprite.setTexture(texture);
    }

    void checkCollision(Player* player) override {
        isColliding = false;

        // Check if player is landing on top of the trigger
        if (player->getVelocityY() > 0 &&
            player->getHitbox().checkBottomCollision(hitbox) &&
            hitbox.checkTopCollision(player->getHitbox())) {

            isColliding = true;
            player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
            player->stopFalling();

            if (!triggered) {
                triggered = true;
                cout << "Level Trigger Activated!" << endl;
                // Move the trigger downward when activated
                y += fallSpeed;
                hitbox.updateHitbox(x, y);
            }
            else {
                player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
                player->stopFalling();
                player->stopMoving();
                player->stopJumping();
            }
        }
    }

    bool isTriggered() const {
        return triggered;
    }

    void update(char** grid, const int cell_size) override {
        if (triggered) {
            // Continue moving the trigger downward after activation
            y += fallSpeed;
            hitbox.updateHitbox(x, y);
            sprite.setPosition(x, y);
        }
    }
};
class ObstacleFactory {
private:
    int gridHeight, gridWidth;
    int levelNum;

public:
    ObstacleFactory(int height, int width, int lvl) : gridHeight(height), gridWidth(width), levelNum(lvl) {    }

    ~ObstacleFactory() {
        // No need to manage arrays in the factory
    }

    // Create a single obstacle
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
        }

        return obstacle;
    }

    // Update an array of obstacles
    void updateObstacles(RenderWindow& window, Obstacles** obstacles, int obstacleCount,
        float offsetX, Player* player) {

        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i] != nullptr) {
                
                // Render obstacles with scrolling offset
                obstacles[i]->render(window, offsetX);
                // Check collision with player
                obstacles[i]->checkCollision(player);
                
            }
        }
    }

};


