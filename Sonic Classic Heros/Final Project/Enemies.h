#pragma once

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "Animations.h"

using namespace sf;
using namespace std;

//===========================================================================
// BASE ENEMIES CLASS
//===========================================================================
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
    Animation* currentAnimation;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    Enemies() : area(500.0f), active(true), died(false) {
        initialX = 1200.0f;
        initialY = 600.0f;
        position[0] = initialX;
        position[1] = initialY;

        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);

        sound.loadFromFile("Data/sounds/punch.wav");

        dying.setBuffer(sound);
        dying.setPlayingOffset(seconds(0.0f));
        dying.setVolume(100);

        currentAnimation = &enemyR;
    }

    virtual ~Enemies() = default;

    //===========================================================================
    // OVERRIDE METHODS
    //===========================================================================
    void draw(RenderWindow& window, float directionFaced, float offset) override {}

    void update(char** grid, const int cell_size) override {}

    void render(RenderWindow& window, float offset) override {
        if (!died) {
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }
        }
    }

    //===========================================================================
    // ENEMY BEHAVIOR METHODS
    //===========================================================================
    bool detectArea(float playerX, float playerY) const {
        return (position[0] + area >= playerX && position[0] - area <= playerX);
    }

    virtual void die(const Hitbox& playerHitbox) {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            dying.play();
            died = true;
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
    virtual void update(float playerX, float playerY, float deltaTime) {
        if (speed < 0) {
            currentAnimation = &enemyL;
        }
        else {
            currentAnimation = &enemyR;
        }

        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        hitbox.updateHitbox(position[0], position[1]);
    }

    //===========================================================================
    // GETTERS & SETTERS
    //===========================================================================
    void setPosition(float newX, float newY) {
        position[0] = newX;
        position[1] = newY;
        initialX = newX;
        initialY = newY;

        hitbox.updateHitbox(position[0], position[1]);
    }

    const Hitbox& getHitbox() const {
        return hitbox;
    }

    float getX() const { return position[0]; }
    float getY() const { return position[1]; }
    bool isDead() const { return died; }
};

//===========================================================================
// FOLLOWING TYPE ENEMIES
//===========================================================================
class FollowingType : public Enemies {
public:
    FollowingType() {
        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (detectArea(playerX, playerY) && !died) {
            followPlayer(playerX);
        }
        else {
            position[0] = initialX;
            position[1] = initialY;
        }

        if (speed < 0) {
            currentAnimation = &enemyL;
        }
        else {
            currentAnimation = &enemyR;
        }

        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        hitbox.updateHitbox(position[0], position[1]);
    }

    void followPlayer(float playerX) {
        if (playerX > position[0]) {
            position[0] += abs(speed);
            speed = abs(speed);
        }
        else {
            position[0] -= abs(speed);
            speed = -abs(speed);
        }
    }
};

//===========================================================================
// BAT BRAIN ENEMY
//===========================================================================
class BatBrain : public FollowingType {
public:
    BatBrain() {
        left.loadFromFile("Data/Enemy/batbrainL.png");
        right.loadFromFile("Data/Enemy/batbrainR.png");

        enemyL = Animation(left, 551, 64, 9, 0.05f);
        enemyR = Animation(right, 551, 64, 9, 0.05f);

        hitbox = Hitbox(position[0], position[1], 64.0f, 64.0f);

        currentAnimation = &enemyR;
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "BatBrain ded " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
        }
    }
};

//===========================================================================
// MOTOBUG ENEMY
//===========================================================================
class Motobug : public FollowingType {
public:
    Motobug() {
        left.loadFromFile("Data/Enemy/motobugL.png");
        right.loadFromFile("Data/Enemy/motobugR.png");

        enemyL = Animation(left, 444, 64, 5, 0.05f);
        enemyR = Animation(right, 444, 64, 5, 0.05f);

        hitbox = Hitbox(position[0], position[1], 91.0f, 64.0f);

        currentAnimation = &enemyR;
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "Motobug ded " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
        }
    }
};

//===========================================================================
// SHOOTING TYPE ENEMIES
//===========================================================================
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

    Hitbox bulletHitbox;

public:
    ShootingType()
        : isShooting(false), cooldownTime(0), shootingCooldown(5.0f) {

        bulletTexture.loadFromFile("Data/bullet.png");
        bulletSprite.setTexture(bulletTexture);
        bulletSprite.setScale(1.0f, 1.0f);
        bulletActive = false;

        bulletHitbox = Hitbox(0, 0, 16.0f, 16.0f);

        position[1] -= 200;

        hitbox = Hitbox(position[0], position[1], 50.0f, 50.0f);
    }

    //===========================================================================
    // SHOOTING ENEMY METHODS
    //===========================================================================
    Hitbox& getBulletHitbox() {
        return bulletHitbox;
    }

    void render(RenderWindow& window, float offset) override {
        if (!died) {
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }

            drawHitbox(window, offset);
        }

        if (bulletActive) {
            bulletSprite.setPosition(bulletX - offset, bulletY);
            window.draw(bulletSprite);

            drawBulletHitbox(window, offset);
        }
    }

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
            float dx = playerX - position[0];
            if (abs(dx) > 200.0f) {
                if (dx > 0) {
                    position[0] += abs(speed);
                    speed = abs(speed);
                }
                else {
                    position[0] -= abs(speed);
                    speed = -abs(speed);
                }
            }
            else {
                cooldownTime += deltaTime;
                if (cooldownTime >= shootingCooldown) {
                    shootProjectile(playerX, playerY);
                    cooldownTime = 0;
                }
            }

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

        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }

        hitbox.updateHitbox(position[0], position[1]);

        updateProjectiles();
    }

    void updateProjectiles() {
        if (bulletActive) {
            bulletX += bulletDX;
            bulletY += bulletDY;
            bulletSprite.setPosition(bulletX, bulletY);

            bulletHitbox.updateHitbox(bulletX, bulletY);

            if (bulletX < position[0] - area || bulletX > position[0] + area || bulletY < 0 || bulletY > 1000)
                bulletActive = false;
        }
    }

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

            bulletHitbox.updateHitbox(bulletX, bulletY);
        }
    }
};

//===========================================================================
// BEEBOT ENEMY
//===========================================================================
class BeeBot : public ShootingType {
public:
    BeeBot() {
        left.loadFromFile("Data/Enemy/beebotL.png");
        right.loadFromFile("Data/Enemy/beebotR.png");

        enemyL = Animation(left, 95, 64, 1, 0.05f);
        enemyR = Animation(right, 95, 64, 1, 0.05f);

        currentAnimation = &enemyR;

        position[1] -= 150;

        hitbox = Hitbox(position[0], position[1], 96.0f, 64.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "BeeBot ded " << position[1] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
        }
    }
};

//===========================================================================
// CRABMEAT ENEMY
//===========================================================================
class CrabMeat : public ShootingType {
public:
    CrabMeat() {
        left.loadFromFile("Data/Enemy/crabmeat.png");
        right.loadFromFile("Data/Enemy/crabmeat.png");

        enemyL = Animation(left, 1020, 64, 10, 0.1f);
        enemyR = Animation(right, 1020, 64, 10, 0.1f);

        currentAnimation = &enemyR;

        hitbox = Hitbox(position[0], position[1], 102.0f, 64.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            cout << "CrabMeat ded at position " << position[0] << ' ' << position[1] << "\n";

            dying.play();
            died = true;
        }
    }
};

//===========================================================================
// EGGSTINGER BOSS ENEMY
//===========================================================================
class EggStinger : public ShootingType {
private:
    int hp;

public:
    EggStinger() {
        initialX = 600; initialY = 600;
        left.loadFromFile("Data/Enemy/eggstingerL.png");
        right.loadFromFile("Data/Enemy/eggstingerR.png");
        enemyL = Animation(left, 128, 128, 1, 0.1f);
        enemyR = Animation(right, 128, 128, 1, 0.1f);
        currentAnimation = &enemyR;
        position[0] = initialX;
        position[1] = 150.0f;
        active = true;
        hp = 20;
        speed = 100.0f;
        hitbox = Hitbox(position[0], position[1], 128.0f, 128.0f);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (!died) {
            position[0] += speed * 5 * deltaTime;

            if (position[0] < initialX - 600) {
                speed = abs(speed);
                currentAnimation = &enemyR;
            }
            else if (position[0] > initialX + 600) {
                speed = -abs(speed);
                currentAnimation = &enemyL;
            }

            if (speed < 0) {
                currentAnimation = &enemyL;
            }
            else {
                currentAnimation = &enemyR;
            }

            if (currentAnimation) {
                currentAnimation->update(deltaTime);
            }

            hitbox.updateHitbox(position[0], position[1]);
        }

        updateProjectiles();
    }

    void render(RenderWindow& window, float offset) override {
        if (!died) {
            if (currentAnimation) {
                currentAnimation->setPosition(position[0] - offset, position[1]);
                currentAnimation->draw(window);
            }

            drawHitbox(window, offset);
        }

        if (bulletActive) {
            bulletSprite.setPosition(bulletX - offset, bulletY);
            window.draw(bulletSprite);

            drawBulletHitbox(window, offset);
        }
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox) && !died) {
            hp--;
            cout << "EggStinger hit: " << (20 - hp) << "/20\n";

            if (hp <= 0) {
                cout << "EggStinger Neutralized\n";
                dying.play();
                died = true;
            }
        }
    }
};

//===========================================================================
// ENEMY FACTORY
//===========================================================================
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
    }

    //===========================================================================
    // ENEMY CREATION METHODS
    //===========================================================================
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

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
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
                    if (enemies[i]->getX() == 0 && enemies[i]->getY() == 0) {
                        delete enemies[i];
                        enemies[i] = nullptr;
                    }
                }
                else {
                    player->takeDamage(enemies[i]->getHitbox());
                }

                ShootingType* shootingEnemy = dynamic_cast<ShootingType*>(enemies[i]);
                if (shootingEnemy) {
                    if (shootingEnemy->checkBulletCollision(playerHitbox)) {
                        cout << "Player hit by bullet!" << endl;
                        player->takeDamage(shootingEnemy->getBulletHitbox());
                    }
                }
            }
        }
    }
};