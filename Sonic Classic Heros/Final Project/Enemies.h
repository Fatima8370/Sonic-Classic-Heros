#pragma once

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "Animations.h"

using namespace sf;
using namespace std;

class Enemies : public GameEntity {

protected:
    int hp;
    bool active;
    float initialX, initialY;
    float speed = 3.0f;
    const float area;
    bool died;

    bool attack;

    Texture left;
    Texture right;

    SoundBuffer sound;
    Sound dying;


    Animation enemyL;
    Animation enemyR;
    Animation* currentAnimation; // Pointer to current animation

public:
    Enemies() : area(500.0f), active(true), died(false) {
        initialX = 1200.0f;
        initialY = 600.0f;
        position[0] = initialX;
        position[1] = initialY;

        // Initialize the hitbox
        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);

        sound.loadFromFile("Data/sounds/punch.wav");

        dying.setBuffer(sound);
        dying.setPlayingOffset(seconds(0.0f));
        dying.setVolume(100);

        // Set default animation to right (will be updated in update())
        currentAnimation = &enemyR;
    }

    virtual ~Enemies() = default;

    void draw(RenderWindow& window, float directionFaced, float offset) override {}

    void update(char** grid, const int cell_size) override {}

    bool detectArea(float playerX, float playerY) const {
        return (position[0] + area >= playerX && position[0] - area <= playerX);
    }

    // Updated to use hitbox collision
    virtual void die(const Hitbox& playerHitbox) {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            dying.play();
            died = true;
        }
    }

    virtual void render(RenderWindow& window, float offset) override {
        if (!died) {
            // Update current animation position
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }

            //drawHitbox(window, offset);
        }
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
// UPDATE & GAME LOGIC METHODS
//===========================================================================



    // Modified to handle all enemy types in one common method
    virtual void update(float playerX, float playerY, float deltaTime) {
        // Update animation based on direction
        if (speed < 0) {
            currentAnimation = &enemyL;
        }
        else {
            currentAnimation = &enemyR;
        }

        // Update animation frames
        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        // Update hitbox position to match enemy position
        hitbox.updateHitbox(position[0], position[1]);
    }

    void setPosition(float newX, float newY) {
        position[0] = newX;
        position[1] = newY;
        initialX = newX;
        initialY = newY;

        // Update hitbox position
        hitbox.updateHitbox(position[0], position[1]);
    }

    // Getter for the hitbox object
    const Hitbox& getHitbox() const {
        return hitbox;
    }

    float getX() const { return position[0]; }
    float getY() const { return position[1]; }
    bool isDead() const { return died; }
};

class FollowingType : public Enemies {
public:
    FollowingType() {
        // Customize hitbox size for following type enemies
        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (detectArea(playerX, playerY) && !died) {

            followPlayer(playerX);
            // Note: die() will be called in the EnemyFactory::updateEnemies method
        }
        else {
            // Return to initial position but stay visible
            position[0] = initialX;
            position[1] = initialY;
        }

        // Update animation based on direction
        if (speed < 0) {
            currentAnimation = &enemyL;
        }
        else {
            currentAnimation = &enemyR;
        }

        // Update animation frames
        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        // Update hitbox position
        hitbox.updateHitbox(position[0], position[1]);
    }

    void followPlayer(float playerX) {
        if (playerX > position[0]) {
            position[0] += abs(speed);
            speed = abs(speed); // Positive speed means moving right
        }
        else {
            position[0] -= abs(speed);
            speed = -abs(speed); // Negative speed means moving left
        }
    }
};

class BatBrain : public FollowingType {
public:
    BatBrain() {
        left.loadFromFile("Data/Enemy/batbrainL.png");
        right.loadFromFile("Data/Enemy/batbrainR.png");

        enemyL = Animation(left, 551, 64, 9, 0.05f);
        enemyR = Animation(right, 551, 64, 9, 0.05f);

        // Customize hitbox for BatBrain
        hitbox = Hitbox(position[0], position[1], 64.0f, 64.0f);

        // Set default animation
        currentAnimation = &enemyR;
    }

    // Override the die method to use hitbox
    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "BatBrain ded " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
            position[0] = 0; position[1] = 0;
        }
    }
};

class Motobug : public FollowingType {
public:
    Motobug() {
        left.loadFromFile("Data/Enemy/motobugL.png");
        right.loadFromFile("Data/Enemy/motobugR.png");

        enemyL = Animation(left, 444, 64, 5, 0.05f);
        enemyR = Animation(right, 444, 64, 5, 0.05f);

        // Customize hitbox for Motobug (typically wider and shorter)
        hitbox = Hitbox(position[0], position[1], 91.0f, 64.0f);

        // Set default animation
        currentAnimation = &enemyR;
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "Motobug ded " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
            position[0] = 0; position[1] = 0;
        }
    }
};

class ShootingType : public Enemies {
protected:
    float shootingCooldown;
    float cooldownTime;
    bool isShooting;

    Texture bulletTexture;
    Sprite bulletSprite;
    bool bulletActive;
    float bulletX, bulletY;
    float bulletDX, bulletDY;

    // Add hitbox for bullet
    Hitbox bulletHitbox;

public:
    ShootingType()
        : isShooting(false), cooldownTime(0), shootingCooldown(5.0f) {

        bulletTexture.loadFromFile("Data/bullet.png");
        bulletSprite.setTexture(bulletTexture);
        bulletSprite.setScale(1.0f, 1.0f);
        bulletActive = false;

        // Initialize bullet hitbox (small size for projectile)
        bulletHitbox = Hitbox(0, 0, 16.0f, 16.0f);

        position[1] -= 200;

        // Customize hitbox for shooting type enemies
        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);
    }

    Hitbox& getBulletHitbox() {
        return bulletHitbox;
    }

    void render(RenderWindow& window, float offset) override {
        if (!died) {
            // Draw current animation
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }

            // Optional: Draw hitbox for debugging
            drawHitbox(window, offset);
        }

        if (bulletActive) {
            bulletSprite.setPosition(bulletX - offset, bulletY);
            window.draw(bulletSprite);

            // Optional: Draw bullet hitbox for debugging
            drawBulletHitbox(window, offset);
        }
    }

    // Optional: Helper method to visualize bullet hitbox for debugging
    void drawBulletHitbox(RenderWindow& window, float offset) {
        RectangleShape bulletHitboxRect;
        bulletHitboxRect.setPosition(bulletHitbox.getX() - offset, bulletHitbox.getY());
        bulletHitboxRect.setSize(Vector2f(bulletHitbox.getWidth(), bulletHitbox.getHeight()));
        bulletHitboxRect.setFillColor(Color::Transparent);
        bulletHitboxRect.setOutlineColor(Color::Green);
        bulletHitboxRect.setOutlineThickness(1.0f);
        window.draw(bulletHitboxRect);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (detectArea(playerX, playerY) && !died) {
            // Movement
            float dx = playerX - position[0];
            if (abs(dx) > 200.0f) {
                if (dx > 0) {
                    position[0] += abs(speed);
                    speed = abs(speed); // Moving right
                }
                else {
                    position[0] -= abs(speed);
                    speed = -abs(speed); // Moving left
                }
            }
            else {
                cooldownTime += deltaTime;
                if (cooldownTime >= shootingCooldown) {
                    shootProjectile(playerX, playerY);
                    cooldownTime = 0; // Reset cooldown after shooting
                }
            }

            // Update animation based on direction
            if (speed < 0) {
                currentAnimation = &enemyL;
            }
            else {
                currentAnimation = &enemyR;
            }
        }
        else {
            cooldownTime = 0;
        }

        // Update animation frames
        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        // Update enemy hitbox
        hitbox.updateHitbox(position[0], position[1]);

        updateProjectiles();
    }

    void updateProjectiles() {
        if (bulletActive) {
            bulletX += bulletDX;
            bulletY += bulletDY;
            bulletSprite.setPosition(bulletX, bulletY);

            // Update bullet hitbox
            bulletHitbox.updateHitbox(bulletX, bulletY);

            if (bulletX < position[0] - area || bulletX > position[0] + area || bulletY < 0 || bulletY > 1000)
                bulletActive = false;
        }
    }

    // Method to check if bullet hits player
    bool checkBulletCollision(const Hitbox& playerHitbox) {
        if (bulletActive) {
            return bulletHitbox.checkCollision(playerHitbox);
        }
        return false;
    }

    void shootProjectile(float playerX, float playerY) {
        if (!bulletActive) {
            cout << "Shooting projectile\n";

            bulletX = position[0];
            bulletY = position[1];

            float dx = playerX - position[0];
            float dy = playerY - position[1];
            float length = sqrt(dx * dx + dy * dy);

            bulletDX = 6.0f * dx / length;
            bulletDY = 8.0f * dy / length;

            bulletSprite.setPosition(position[0], position[1]);
            bulletActive = true;

            // Reset bullet hitbox position
            bulletHitbox.updateHitbox(bulletX, bulletY);
        }
    }
};

class BeeBot : public ShootingType {
public:
    BeeBot() {
        left.loadFromFile("Data/Enemy/beebotL.png");
        right.loadFromFile("Data/Enemy/beebotR.png");

        enemyL = Animation(left, 95, 64, 1, 0.05f);
        enemyR = Animation(right, 95, 64, 1, 0.05f);

        // Set default animation
        currentAnimation = &enemyR;

        position[1] -= 150;

        hitbox = Hitbox(position[0], position[1], 96.0f, 64.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "BeeBot ded " << position[1] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
            position[0] = 0; position[1] = 0;
        }
    }
};

class CrabMeat : public ShootingType {
public:
    CrabMeat() {
        left.loadFromFile("Data/Enemy/crabmeat.png");
        right.loadFromFile("Data/Enemy/crabmeat.png");

        enemyL = Animation(left, 1020, 64, 10, 0.1f);
        enemyR = Animation(right, 1020, 64, 10, 0.1f);

        // Set default animation
        currentAnimation = &enemyR;

        // Customize hitbox for CrabMeat 
        hitbox = Hitbox(position[0], position[1], 102.0f, 64.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "CrabMeat ded at position " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
            position[0] = 0; position[1] = 0;
        }
    }
};

class EggStinger : public ShootingType {
private:
    float flightTimer;
    float attackInterval;
    bool descending;
    bool ascending;
    float targetY;
    float hoverHeight;
    float descendSpeed;
    float groundY;

public:
    EggStinger()
        : flightTimer(0), attackInterval(8.0f), descending(false), ascending(false),
        hoverHeight(150.0f), descendSpeed(5.0f), groundY(800.0f) {

        initialX = 600; initialY = 600;

        left.loadFromFile("Data/Enemy/eggstingerL.png");
        right.loadFromFile("Data/Enemy/eggstingerR.png");

        enemyL = Animation(left, 64, 64, 1, 0.1f);
        enemyR = Animation(right, 64, 64, 1, 0.1f);

        // Set default animation
        currentAnimation = &enemyR;

        position[1] = hoverHeight;
        active = true;

        // Customize hitbox for EggStinger boss (larger hitbox)
        hitbox = Hitbox(position[0], position[1], 128.0f, 128.0f);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (!died) {
            flightTimer += deltaTime;

            // Hover left-to-right
            position[0] += speed * 2 * deltaTime;

            // Change direction when reaching limits
            if (position[0] < initialX - 600) {
                speed = abs(speed); // Change direction to right
            }
            else if (position[0] > initialX + 600) {
                speed = -abs(speed); // Change direction to left
            }

            // Update animation based on direction
            if (speed < 0) {
                currentAnimation = &enemyL;
            }
            else {
                currentAnimation = &enemyR;
            }

            if (!descending && !ascending && flightTimer >= attackInterval) {
                // Start dive
                descending = true;
                flightTimer = 0;
                targetY = groundY;
            }

            if (descending) {
                position[1] += descendSpeed;
                if (position[1] >= targetY) {
                    position[1] = targetY;
                    descending = false;
                    ascending = true;
                    // Optional: destroy block here
                }
            }
            else if (ascending) {
                position[1] -= descendSpeed;
                if (position[1] <= hoverHeight) {
                    position[1] = hoverHeight;
                    ascending = false;
                }
            }

            // Update animation frames
            if (currentAnimation) {
                currentAnimation->update(deltaTime);
            }

            // Update hitbox position
            hitbox.updateHitbox(position[0], position[1]);
        }

        updateProjectiles();
    }

    void render(RenderWindow& window, float offset) override {
        if (!died) {
            // Draw current animation
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }

            // Optional: Draw hitbox for debugging
            drawHitbox(window, offset);
        }

        if (bulletActive) {
            bulletSprite.setPosition(bulletX - offset, bulletY);
            window.draw(bulletSprite);

            // Optional: Draw bullet hitbox for debugging
            drawBulletHitbox(window, offset);
        }
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "EggStinger Neutralized\n";
            dying.play();
            died = true;
        }
    }
};

// Refactored Factory Classes

class EnemyFactory {
private:
    int gridHeight, gridWidth;
    int levelNum;

public:
    EnemyFactory(int height, int width, int lvl)
        : gridHeight(height), gridWidth(width), levelNum(lvl)
    {
        cout << "In Enemy Factory\n";
    }

    ~EnemyFactory() {
        // No need to manage arrays in the factory
    }

    // Create a single enemy
    Enemies* createEnemy(const char& type, float spawnX, float spawnY) {
        Enemies* enemy = nullptr;

        if (type == '2') enemy = new BatBrain();
        else if (type == '1') enemy = new Motobug();
        else if (type == '3') enemy = new BeeBot();
        else if (type == '4') enemy = new CrabMeat();
        else enemy = new EggStinger();

        if (enemy) {
            enemy->setPosition(spawnX, spawnY);
        }

        return enemy;
    }

    // Update an array of enemies
    void updateEnemies(RenderWindow& window, Enemies** enemies, int enemyCount, float offsetX, float deltaTime, Player* player) {

        const Hitbox playerHitbox = player->getHitbox();
        bool attack = player->getIsJumping();

        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i] != nullptr) {
                enemies[i]->update(playerHitbox.getX(), playerHitbox.getY(), deltaTime);
                enemies[i]->render(window, offsetX);

                if (attack) {
                    enemies[i]->die(playerHitbox);
                }

                if (enemies[i]->isDead()) {
                    // We should not delete enemies immediately after setting died to true
                    // The sound needs time to play
                    if (enemies[i]->getX() == 0 && enemies[i]->getY() == 0) {
                        delete enemies[i];
                        enemies[i] = nullptr;
                    }
                }
                else {
                    player->takeDamage(enemies[i]->getHitbox());
                }

                // Check if shooting enemy's bullets hit player
                ShootingType* shootingEnemy = dynamic_cast<ShootingType*>(enemies[i]);
                if (shootingEnemy) {
                    if (shootingEnemy->checkBulletCollision(playerHitbox)) {
                        // Player is hit by bullet, handle damage here
                        cout << "Player hit by bullet!" << endl;
                        player->takeDamage(shootingEnemy->getBulletHitbox());
                    }
                }
            }
        }
    }
};