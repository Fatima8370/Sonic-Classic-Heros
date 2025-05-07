#pragma once
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include "Hitbox.h"
#include "Character.h"
#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;

class Enemies {

protected:
    int hp;
    bool active;
    float x, y;
    float initialX, initialY;
    float flip;
    bool died;
    float speed = 3.0f;
    const float area;

    bool attack;

    Texture b;
    Sprite a;

    SoundBuffer sound;
    Sound dying;

    // Add a hitbox member to the Enemies class
    Hitbox hitbox;


public:
    Enemies() : area(500.0f), flip(-2.0f), active(true), died(false) {
        initialX = 1200.0f;
        initialY = 600.0f;
        x = initialX;
        y = initialY;

        // Initialize the hitbox
        hitbox = Hitbox(x, y, 50.0f, 50.0f);

        sound.loadFromFile("Data/sounds/punch.wav");

        dying.setBuffer(sound);
        dying.setPlayingOffset(seconds(.4));
        //dying.setPitch(2.0f);
        dying.setVolume(100);
    }

    virtual ~Enemies() = default;

    bool detectArea(float playerX, float playerY) const {
        return (x + area >= playerX && x - area <= playerX);
    }

    // Updated to use hitbox collision
    virtual void die(const Hitbox& playerHitbox) {
        if (hitbox.checkCollision(playerHitbox)) {
            dying.play();
            died = true;
        }
    }

    virtual void draw(RenderWindow& window, float offset) {
        if (!died)
        {
            a.setPosition(x - offset, y);
            window.draw(a);

            // Optional: Draw hitbox for debugging
            // drawHitbox(window, offset);
        }
    }

    // Optional: Helper method to visualize hitboxes for debugging
    void drawHitbox(RenderWindow& window, float offset) {
        RectangleShape hitboxRect;
        hitboxRect.setPosition(hitbox.getX() - offset, hitbox.getY());
        hitboxRect.setSize(Vector2f(hitbox.getWidth(), hitbox.getHeight()));
        hitboxRect.setFillColor(Color::Transparent);
        hitboxRect.setOutlineColor(Color::Red);
        hitboxRect.setOutlineThickness(1.0f);
        window.draw(hitboxRect);
    }

    // Modified to handle all enemy types in one common method
    virtual void update(float playerX, float playerY, float deltaTime) {
        // Base class implementation does nothing - will be overridden by specific enemy types

        // Update hitbox position to match enemy position
        hitbox.updateHitbox(x, y);
    }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
        initialX = newX;
        initialY = newY;

        // Update hitbox position
        hitbox.updateHitbox(x, y);
    }

    // Getter for the hitbox object
    const Hitbox& getHitbox() const {
        return hitbox;
    }

    float getX() const { return x; }
    float getY() const { return y; }
    bool isDead() const { return died; }
};

class FollowingType : public Enemies {
public:
    FollowingType() {
        // Customize hitbox size for following type enemies
        hitbox = Hitbox(x, y, 50.0f, 50.0f);
    }

    // IMPORTANT: Override the update method to handle following behavior
    void update(float playerX, float playerY, float deltaTime) override {
        if (detectArea(playerX, playerY) && !died) {
            // Debug print when enemy starts following
            cout << "Following player at " << playerX << ", " << playerY << endl;

            followPlayer(playerX);
            // Note: die() will be called in the EnemyFactory::updateEnemies method
        }
        else {
            // Return to initial position but stay visible
            x = initialX;
            y = initialY;
        }

        // Update hitbox position
        hitbox.updateHitbox(x, y);
    }

    void followPlayer(float playerX) {
        if (playerX > x) {
            x += speed;
            if (flip > 0) flip = -flip;
        }
        else {
            x -= speed;
            if (flip < 0) flip = -flip;
        }

        a.setScale(flip, 2.0f);
        a.setPosition(x, y);
    }
};

class BatBrain : public FollowingType {
public:
    BatBrain() {
        b.loadFromFile("Data/batbrainL1.png");
        a.setTexture(b);
        a.setScale(flip, 2.0f);

        // Customize hitbox for BatBrain
        hitbox = Hitbox(x, y, 40.0f, 30.0f);
        // Adjust offsets to match sprite dimensions
        hitbox.setOffsets(5.0f, 25.0f, 5.0f, 35.0f);
    }

    // Override the die method to use hitbox
    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox)) {
            cout << "BatBrain ded " << x << ' ' << y << "\n";

            x = 0; y = 0;
            a.setPosition(x, y);
            dying.play();
            died = true;
        }
    }
};

class Motobug : public FollowingType {
public:
    Motobug() {
        b.loadFromFile("Data/motobug.png");
        a.setTexture(b);
        a.setScale(flip, 2.0f);

        // Customize hitbox for Motobug (typically wider and shorter)
        hitbox = Hitbox(x, y, 60.0f, 30.0f);
        // Adjust offsets to match sprite dimensions
        hitbox.setOffsets(10.0f, 30.0f, 10.0f, 50.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox)) {
            cout << "Motobug ded " << x << ' ' << y << "\n";

            x = 0; y = 0;
            a.setPosition(x, y);
            dying.play();
            died = true;
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
        bulletSprite.setScale(2.0f, 2.0f);
        bulletActive = false;

        // Initialize bullet hitbox (small size for projectile)
        bulletHitbox = Hitbox(0, 0, 10.0f, 10.0f);

        y -= 200;

        // Customize hitbox for shooting type enemies
        hitbox = Hitbox(x, y, 50.0f, 50.0f);
    }

	Hitbox& getBulletHitbox() {
		return bulletHitbox;
	}

    void draw(RenderWindow& window, float offset) override {
        if (!died)
        {
            a.setPosition(x - offset, y);
            window.draw(a);
        }

        if (bulletActive)
        {
            bulletSprite.setPosition(bulletX - offset, bulletY);
            window.draw(bulletSprite);

            // Optional: Draw bullet hitbox for debugging
            // drawBulletHitbox(window, offset);
        }
    }

    

    void update(float playerX, float playerY, float deltaTime) override {
        if (detectArea(playerX, playerY) && !died) {
            // Movement
            float dx = playerX - x;
            if (abs(dx) > 200.0f) {
                x += (dx > 0 ? speed : -speed);
                flip = (dx > 0) ? -2.0f : 2.0f;
            }
            else {
                cooldownTime += deltaTime;
                if (cooldownTime >= shootingCooldown) {
                    shootProjectile(playerX, playerY);
                    cooldownTime = 0; // Reset cooldown after shooting
                }
            }

            a.setScale(flip, 2.0f);
            a.setPosition(x, y);
        }
        else {
            cooldownTime = 0;
        }

        // Update enemy hitbox
        hitbox.updateHitbox(x, y);

        updateProjectiles();
    }

    void updateProjectiles() {
        if (bulletActive) {
            bulletX += bulletDX;
            bulletY += bulletDY;
            bulletSprite.setPosition(bulletX, bulletY);

            // Update bullet hitbox
            bulletHitbox.updateHitbox(bulletX, bulletY);

            if (bulletX < x - area || bulletX > x + area || bulletY < 0 || bulletY > 1000)
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

            bulletX = x;
            bulletY = y;

            float dx = playerX - x;
            float dy = playerY - y;
            float length = sqrt(dx * dx + dy * dy);

            bulletDX = 6.0f * dx / length;
            bulletDY = 8.0f * dy / length;

            bulletSprite.setPosition(x, y);
            bulletActive = true;

            // Reset bullet hitbox position
            bulletHitbox.updateHitbox(bulletX, bulletY);
        }
    }
};

class BeeBot : public ShootingType {
public:
    BeeBot() {
        b.loadFromFile("Data/beebot.png");
        a.setTexture(b);
        a.setScale((flip / 2), 1.0f);
        y -= 150;

        // Customize hitbox for BeeBot
        hitbox = Hitbox(x, y, 40.0f, 30.0f);
        // Adjust offsets to match sprite dimensions
        hitbox.setOffsets(5.0f, 25.0f, 5.0f, 35.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox)) {
            cout << "BeeBot ded " << x << ' ' << y << "\n";

            x = 0; y = 0;
            a.setPosition(x, y);
            dying.play();
            died = true;
        }
    }
};

class CrabMeat : public ShootingType {
public:
    CrabMeat() {
        b.loadFromFile("Data/crabmeat.png");
        a.setTexture(b);
        a.setScale(flip, 2.0f);

        // Customize hitbox for CrabMeat (wider than tall)
        hitbox = Hitbox(x, y, 60.0f, 40.0f);
        // Adjust offsets to match sprite dimensions
        hitbox.setOffsets(10.0f, 40.0f, 10.0f, 50.0f);
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox)) {
            cout << "CrabMeat ded at position " << x << ' ' << y << "\n";

            x = 0; y = 0;
            a.setPosition(x, y);
            dying.play();
            died = true;
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

        b.loadFromFile("Data/eggstinger.png");
        a.setTexture(b);
        a.setScale(2.0f, 2.0f);  // Boss size
        y = hoverHeight;

        active = true;

        // Customize hitbox for EggStinger boss (larger hitbox)
        hitbox = Hitbox(x, y, 80.0f, 80.0f);
        // Adjust offsets to match sprite dimensions
        hitbox.setOffsets(10.0f, 70.0f, 10.0f, 70.0f);
    }

    void update(float playerX, float playerY, float deltaTime) override {
        if (!died) {
            flightTimer += deltaTime;

            // Hover left-to-right
            x += speed * 2 * flip * deltaTime;
            if (x < initialX - 600) flip = -flip;
            else if (x > initialX + 600) flip = -flip;

            if (!descending && !ascending && flightTimer >= attackInterval) {
                // Start dive
                descending = true;
                flightTimer = 0;
                targetY = groundY;
            }

            if (descending) {
                y += descendSpeed;
                if (y >= targetY) {
                    y = targetY;
                    descending = false;
                    ascending = true;
                    // Optional: destroy block here
                }
            }
            else if (ascending) {
                y -= descendSpeed;
                if (y <= hoverHeight) {
                    y = hoverHeight;
                    ascending = false;
                }
            }

            a.setPosition(x, y);
            a.setScale(flip, 2.0f);

            // Update hitbox position
            hitbox.updateHitbox(x, y);
        }

        updateProjectiles();
    }

    void draw(RenderWindow& window, float offset) override {
        if (!died) {
            a.setPosition(x - offset, y);
            window.draw(a);
        }
    }

    void die(const Hitbox& playerHitbox) override {
        if (hitbox.checkCollision(playerHitbox)) {
            a.setPosition(0, 0);

            cout << "EggStinger Neutralized\n";
            dying.play();
            died = true;
        }
    }
};

class EnemyFactory {
private:
    static const int MAX_ENEMIES = 50;
    Enemies* enemies[MAX_ENEMIES];
    float usedX[MAX_ENEMIES];
    int count;

    char** levelGrid;
    int gridHeight, gridWidth;
    int levelNum;

    // Minimum distance between enemies
    const float MIN_ENEMY_DISTANCE = 150.0f;

    bool isFarFromOthers(float x) {
        for (int i = 0; i < count; ++i) {
            if (abs(x - usedX[i]) < MIN_ENEMY_DISTANCE)
                return false;
        }
        return true;
    }

    Enemies* createEnemy(const string& type, float spawnX, float spawnY) {
        Enemies* enemy = nullptr;

        if (type == "batbrain") enemy = new BatBrain();
        else if (type == "motobug") enemy = new Motobug();
        else if (type == "beebot") enemy = new BeeBot();
        else if (type == "crabmeat") enemy = new CrabMeat();
        else if (type == "eggstinger") enemy = new EggStinger();

        if (enemy) {
            enemy->setPosition(spawnX, spawnY);
            cout << "Created " << type << " at (" << spawnX << ", " << spawnY << ")" << endl;
        }
        return enemy;
    }

public:
    EnemyFactory(char** grid, int height, int width, int lvl)
        : gridHeight(height), gridWidth(width), levelNum(lvl), count(0), levelGrid(grid)
    {
        for (int i = 0; i < MAX_ENEMIES; ++i)
            enemies[i] = nullptr;

        cout << "In Enemy Factory\n";
    }

    Enemies* createWithSpawn(const string& type) {
        if (count >= MAX_ENEMIES) return nullptr;

        cout << "Creating with spawn: " << type << endl;

        int tries = 0;
        const int MAX_TRIES = 100; // Increased attempts

        while (tries < MAX_TRIES) {
            int col = rand() % gridWidth;
            int row;
            float spawnX = static_cast<float>(col * 64);  // tile size
            float spawnY;
            bool validSpawn = false;

            // Different spawn algorithms based on enemy type
            if (type == "batbrain" || type == "beebot") {
                // FLYING ENEMIES: Spawn in open air
                // Pick a random row in the upper 2/3 of the level
                row = rand() % (int)(gridHeight * 0.66);

                // Check for an open space (no wall/spike/pit)
                if (levelGrid[row][col] == ' ') {
                    // For flying enemies, adjust height to be higher off the ground
                    // Find distance to ground
                    int groundRow = row;
                    while (groundRow < gridHeight && levelGrid[groundRow][col] == ' ') {
                        groundRow++;
                    }

                    // If we're at least 2 tiles from ground/ceiling, this is a good spot
                    if (row > 1 && (groundRow - row) >= 2) {
                        spawnY = static_cast<float>(row * 64);
                        validSpawn = true;

                        // Adjust heights for specific flying enemies
                        if (type == "beebot") {
                            spawnY -= 30; // Lift BeeBots higher
                        }
                    }
                }
            }
            else if (type == "motobug" || type == "crabmeat") {
                // GROUND ENEMIES: Find solid ground to spawn on

                // Start from a random position in the lower 2/3 of the level
                row = (int)(gridHeight * 0.33) + rand() % (int)(gridHeight * 0.66);

                // Look for a platform or ground
                if (row + 1 < gridHeight &&
                    (levelGrid[row][col] == ' ' &&
                        (levelGrid[row + 1][col] == 'w' || levelGrid[row + 1][col] == 'p'))) {

                    // Place directly on top of the ground
                    spawnY = static_cast<float>(row * 64);
                    validSpawn = true;

                    // Adjust for specific ground enemies
                    if (type == "motobug") {
                        spawnY += 10; // Place motobugs slightly lower to touch ground
                    }
                }
            }
            else if (type == "eggstinger") {
                // BOSS: Special spawn in center-top
                spawnX = (gridWidth / 2) * 64.0f;
                spawnY = 150.0f;
                validSpawn = true;
            }

            if (validSpawn && isFarFromOthers(spawnX)) {
                Enemies* e = createEnemy(type, spawnX, spawnY);
                if (e) {
                    enemies[count] = e;
                    usedX[count] = spawnX;
                    ++count;
                    return e;
                }
            }

            ++tries;
        }

        cout << "Failed to spawn " << type << " after " << MAX_TRIES << " attempts." << endl;
        return nullptr;
    }

    // More flexible enemy creation for specific patterns or manual placement
    Enemies* createAt(const string& type, float spawnX, float spawnY) {
        if (count >= MAX_ENEMIES) return nullptr;

        if (isFarFromOthers(spawnX)) {
            Enemies* e = createEnemy(type, spawnX, spawnY);
            if (e) {
                enemies[count] = e;
                usedX[count] = spawnX;
                ++count;
                return e;
            }
        }
        return nullptr;
    }

    // Spawn a specific number of each enemy type
    void populateLevel() {
        // Clear any existing enemies
        for (int i = 0; i < count; i++) {
            if (enemies[i]) {
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        count = 0;

        // Spawn enemies based on level number
        int batbrains = 3 + levelNum;
        int motobugs = 4 + levelNum;
        int beebots = (levelNum > 1) ? 2 + levelNum : 0; // No beebots in level 1
        int crabmeats = (levelNum > 2) ? 2 + levelNum : 0; // No crabmeats in levels 1-2

        // Boss only in certain levels
        if (levelNum % 4 == 0) { // Every 4th level has a boss
            createWithSpawn("eggstinger");
        }

        // Spawn regular enemies
        for (int i = 0; i < batbrains; i++) createWithSpawn("batbrain");
        for (int i = 0; i < motobugs; i++) createWithSpawn("motobug");
        for (int i = 0; i < beebots; i++) createWithSpawn("beebot");
        for (int i = 0; i < crabmeats; i++) createWithSpawn("crabmeat");

        cout << "Level " << levelNum << " populated with " << count << " enemies" << endl;
    }

    // Modified updateEnemies method to use player hitbox instead of coordinates
    void updateEnemies(RenderWindow& window, Enemies* enemy[], int count, float offsetX,
        float deltaTime, Player * player)
    {
       const Hitbox playerHitbox = player->getHitbox();

		bool attack = player->getIsJumping();

		


        // Only print debug info occasionally to avoid console spam
        static int frameCounter = 0;
        frameCounter++;

        if (frameCounter % 60 == 0 && count > 0) { // Print debug every 60 frames
            cout << "Updating " << count << " enemies. Player at ("
                << playerHitbox.getX() << ", " << playerHitbox.getY() << ")" << endl;
            frameCounter = 0;
        }

        for (int i = 0; i < count; i++)
        {
            if (enemy[i])
            {
                // Update all enemies using the unified update method
                enemy[i]->update(playerHitbox.getX(), playerHitbox.getY(), deltaTime);

                // Draw with scrolling offset
                enemy[i]->draw(window, offsetX);

                // Handle attack/die logic with hitbox
                if (attack) {
                    enemy[i]->die(playerHitbox);
				}
				else if (enemy[i]->isDead()) {
					// Handle enemy death
					cout << "Enemy " << i << " is dead!" << endl;
					delete enemy[i];
					enemy[i] = nullptr;
					count--;
				}
                else {
                    player->takeDamage(enemy[i]->getHitbox());
                }

                // Check if shooting enemy's bullets hit player
                ShootingType* shootingEnemy = dynamic_cast<ShootingType*>(enemy[i]);
                if (shootingEnemy) {
                    if (shootingEnemy->checkBulletCollision(playerHitbox)) {
                        // Player is hit by bullet, handle damage here
                        cout << "Player hit by bullet!" << endl;
                        player->takeDamage(shootingEnemy->getBulletHitbox());
                        // Add code to damage player
                    }
                }
            }
        }
    }

    // Clear all enemies - useful when changing levels
    void clearEnemies() {
        for (int i = 0; i < count; i++) {
            if (enemies[i]) {
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        count = 0;
        cout << "All enemies cleared" << endl;
    }

    int getCount() const { return count; }
    Enemies** getAllEnemies() { return enemies; }
};