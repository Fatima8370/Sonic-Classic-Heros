#pragma once    

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Character.h"

using namespace sf;
using namespace std;



class Obstacles : public GameEntity {
protected:
    //===========================================================================
    // OBSTACLE PROPERTIES
    //===========================================================================
    Sprite obstacleSprite;
    Texture obstacleTexture;
    bool isColliding;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    Obstacles(float x = 0.0f, float y = 0.0f, char s = ' ') 
        : GameEntity(x, y, s), isColliding(false) {
        hitbox = Hitbox(position[0], position[1], 64, 64);
    }

    virtual ~Obstacles() {}

    //===========================================================================
    // RENDERING & DRAWING METHODS
    //===========================================================================
    // GameEntity abstract method implementations

     void draw(RenderWindow& window, float directionFaced, float offsetX) override {
        obstacleSprite.setPosition(position[0] - offsetX, position[1]);
        window.draw(obstacleSprite);
    }

    // Important: This render method is required by GameEntity
    virtual void render(RenderWindow& window, float offset) override {
        obstacleSprite.setPosition(position[0] - offset, position[1]);
        window.draw(obstacleSprite);
         drawHitbox(window, offset);
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

    //===========================================================================
    // UPDATE & POSITION METHODS
    //===========================================================================
    void update(char** grid, const int cell_size) override {
        updateHitbox();
    }

    void updateHitbox() {
        hitbox.updateHitbox(position[0], position[1]);
    }

    void setPosition(float x, float y) override {
        GameEntity::setPosition(x, y);
        obstacleSprite.setPosition(x, y);
        updateHitbox();
    }

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
    bool isCollidingWithPlayer() const {
        return isColliding;
    }

    // Pure virtual method to be implemented by derived classes
    virtual void checkCollision(Player* player) = 0;
};

//===========================================================================
// WALL CLASS IMPLEMENTATION
//===========================================================================
class Wall : public Obstacles {
private:
    //===========================================================================
    // WALL-SPECIFIC PROPERTIES
    //===========================================================================
    bool isBreakable;
    bool isDestroyed;

public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Wall(bool status = false, float x = 0.0f, float y = 0.0f) :
        Obstacles(x, y, status ? 'B' : 'W'),
        isBreakable(status),
        isDestroyed(false)
    {
        if (isBreakable) {
            obstacleTexture.loadFromFile("Data/breakable_wall.png");
        }
        else {
            obstacleTexture.loadFromFile("Data/brick2.png");
        }
        obstacleSprite.setTexture(obstacleTexture);
    }

    //===========================================================================
    // WALL STATE METHODS
    //===========================================================================
    bool isWallDestroyed() const {
        return isDestroyed;
    }

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
    virtual void checkCollision(Player* player)  {
        if (isDestroyed) {
            return;
        }

        isColliding = false;
        if (player->getHitbox().checkCollision(hitbox)) {
            if (isBreakable && player->getIsActive() && player->getPlayerIndex() == 2) {
                if (player->getIsUsingSpecialAbility()) {
                    isDestroyed = true;
                    cout << "Wall broken by Knuckles!" << endl;
                    return;
                }
            }
            resolveCollisionWithPlayer(player);
        }
    }

    void resolveCollisionWithPlayer(Player* player) {
        bool topCollision = hitbox.checkTopCollision(player->getHitbox());
        bool bottomCollision = hitbox.checkBottomCollision(player->getHitbox());
        bool leftCollision = hitbox.checkLeftCollision(player->getHitbox());
        bool rightCollision = hitbox.checkRightCollision(player->getHitbox());

        // Reactions to each specific type of collision:
        if (topCollision) {
            isColliding = true;
            player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
            player->stopFalling();
            player->stopJumping();
            player->stopMoving();
        }
        else if (leftCollision) {
            isColliding = true;
            player->setPosition(hitbox.getX() - player->getHitbox().getWidth(), player->getY());
            player->setVelocityX(0);
            player->stopMoving();
        }
        else if (rightCollision) {
            isColliding = true;
            player->setPosition(hitbox.getX() + hitbox.getWidth(), player->getY());
            player->setVelocityX(0);
            player->stopMoving();
        }
        else if (bottomCollision) {
            isColliding = true;
            player->setPosition(player->getX(), hitbox.getY() + hitbox.getHeight());
            player->stopJumping();
            player->setVelocityY(0);
        }
    }

    //===========================================================================
    // RENDERING & UPDATE METHODS
    //===========================================================================
    void render(RenderWindow& window, float offset) override {
        if (!isDestroyed) {
            Obstacles::render(window, offset);
        }
    }

    void update(char** grid, const int cell_size = 64) override {
        if (!isDestroyed) {
            Obstacles::update(grid, cell_size);
        }
    }
};

//===========================================================================
// SPIKES CLASS IMPLEMENTATION
//===========================================================================
class Spikes : public Obstacles {
public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Spikes(float x = 0.0f, float y = 0.0f) :
        Obstacles(x, y, 'S')
    {
        hitbox = Hitbox(position[0] + 16, position[1] + 32, 60, 32);
        obstacleTexture.loadFromFile("Data/spike.png");
        obstacleSprite.setTexture(obstacleTexture);
    }

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
    void checkCollision(Player* player) override {
        isColliding = false;

        if (player->getHitbox().checkCollision(hitbox)) {
            if (player->getHitbox().checkBottomCollision(hitbox)) {
                isColliding = true;
                player->takeDamage(hitbox);
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

//===========================================================================
// PLATFORM CLASS IMPLEMENTATION
//===========================================================================
class Platform : public Obstacles {
public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Platform(float x = 0.0f, float y = 0.0f) :
        Obstacles(x, y, 'P')
    {
        hitbox = Hitbox(position[0] + 16, position[1] + 32, 60, 32);

        obstacleTexture.loadFromFile("Data/brick1.png");
        obstacleSprite.setTexture(obstacleTexture);
    }

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
    void checkCollision(Player* player) override {
        isColliding = false;  // Added to fix missing initialization

        if (player->getVelocityY() > 0 &&
            player->getHitbox().checkBottomCollision(hitbox) &&
            hitbox.checkTopCollision(player->getHitbox())) {

            isColliding = true;  // Added to properly set collision flag
            player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
            player->stopFalling();
            player->stopMoving();
            player->stopJumping();
        }
    }
};

//===========================================================================
// LEVEL TRIGGER CLASS IMPLEMENTATION
//===========================================================================
class LevelTrigger : public Obstacles {
private:
    //===========================================================================
    // LEVEL TRIGGER SPECIFIC PROPERTIES
    //===========================================================================
    bool triggered;
    float fallSpeed;

public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    LevelTrigger(float x = 0.0f, float y = 0.0f) :
        Obstacles(x, y, 'T'),
        triggered(false),
        fallSpeed(20.0f)
    {
        obstacleTexture.loadFromFile("Data/ground.png");
        obstacleSprite.setTexture(obstacleTexture);
    }

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
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
                position[1] += fallSpeed;
                updateHitbox();
            }
            else {
                player->setPosition(player->getX(), hitbox.getY() - player->getHitbox().getHeight());
                player->stopFalling();
                player->stopMoving();
            }
        }
    }

    //===========================================================================
    // STATE & UPDATE METHODS
    //===========================================================================
    bool isTriggered() const {
        return triggered;
    }

    void update(char** grid, const int cell_size = 64) override {
        if (triggered) {
            position[1] += fallSpeed;
            updateHitbox();
            obstacleSprite.setPosition(position[0], position[1]);
        }
    }
};

//===========================================================================
// OBSTACLE FACTORY CLASS IMPLEMENTATION
//===========================================================================
class ObstacleFactory {
private:
    //===========================================================================
    // MEMBER VARIABLES
    //===========================================================================
    int gridHeight, gridWidth;
    int levelNum;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    ObstacleFactory(int height, int width, int lvl) : gridHeight(height), gridWidth(width), levelNum(lvl) {}

    ~ObstacleFactory() {
        // No need for cleanup in the factory itself
    }

    //===========================================================================
    // OBSTACLE CREATION METHODS
    //===========================================================================
    Obstacles* createObstacle(const char& type, float spawnX, float spawnY) {
        Obstacles* obstacle = nullptr;

        switch (type) {
        case 'W': obstacle = new Wall(false); break;
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

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
    void updateObstacles(RenderWindow& window, Obstacles** obstacles, int obstacleCount, float offsetX, Player* player) {
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i] != nullptr) {
                Wall* wall = dynamic_cast<Wall*>(obstacles[i]);
                if (wall && wall->isWallDestroyed()) {
                    obstacles[i]->update(nullptr, 64);
                }
                else {
                    obstacles[i]->update(nullptr, 64);
                    obstacles[i]->render(window, offsetX);
                    obstacles[i]->checkCollision(player);
                }
            }
        }
    }
};