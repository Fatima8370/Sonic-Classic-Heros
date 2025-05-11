#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Hitbox.h"
#include "Animations.h"
#include "GameEntity.h"


using namespace sf;
using namespace std;

// Base class for all characters in the game

class Player : public GameEntity {

protected:
    static int hp;
    static int lives;

    bool isMoving;

    float velocity[2];
    float speed;
    float jumpStrength;
    bool onGround;
    bool isActive;
    bool isJumping;

    Player* leader;
    float followDistance;
    float followLimit;
    float jumpLimit;
    float maxFollowLimit;
    float lastDirection;
    float lastPosition[2];

    float gravity;
    float terminalVelocity;
    int hitboxFactorX;
    int hitboxFactorY;
    int playerHeight;
    int playerWidth;
    Hitbox hitbox;

    bool isUsingSpecialAbility;
    float specialAbilityTimer;
    float specialAbilityLimit;

    //damage related/
    bool isInvincible;
    float invincibilityLimit;
    float invincibilityTimer;

    int maxWidth;

    // Animations
    Animation idleLeftAnim;
    Animation idleRightAnim;
    Animation walkLeftAnim;
    Animation walkRightAnim;
    Animation jumpLeftAnim;
    Animation jumpRightAnim;
    Animation specialLeftAnim;
    Animation specialRightAnim;
    Animation* currentAnimation; // pointer to current animation

    // Textures for different states
    Texture idleLeftTexture;
    Texture idleRightTexture;
    Texture walkLeftTexture;
    Texture walkRightTexture;
    Texture jumpLeftTexture;
    Texture jumpRightTexture;
    Texture specialLeftTexture;
    Texture specialRightTexture;

    // Sound
    SoundBuffer jumpSoundBuffer;
    SoundBuffer damageSoundBuffer;
    SoundBuffer specialSoundBuffer;
    Sound jumpSound;
    Sound damageSound;
    Sound specialSound;

public:
    Player() : GameEntity('p'),
        isInvincible(false),
        invincibilityLimit(3.0f),
        invincibilityTimer(0.0f),
        isMoving(false),
        speed(0.0f),
        jumpStrength(-25.0f),
        onGround(true),
        isJumping(false),
        isActive(false),
        leader(nullptr),
        followDistance(30.0f),
        followLimit(5.0f),
        jumpLimit(5.0f),
        maxFollowLimit(100.0f),
        lastDirection(1.0f),
        gravity(2.0f),
        terminalVelocity(20.0f),
        hitboxFactorX(static_cast<int>(8)),
        hitboxFactorY(static_cast<int>(5)),
        isUsingSpecialAbility(false),
        specialAbilityTimer(0.0f),
        specialAbilityLimit(0.0f)
    {
        for (int i = 0; i < 2; i++) {
            lastPosition[i] = position[i];
            velocity[i] = 0.0f;
        }
    }

    virtual ~Player() {
        leader = nullptr;
    }

    //position getter setter
    bool getIsMoving() const { return isMoving; }

    //leader related getter setters
    void setLeader(Player* lead) { if (lead != this) this->leader = lead; }
    Player* getLeader() { return leader; }
    void setActive(bool ac) { isActive = ac; }
    bool getIsActive() const { return isActive; }
    float getLastDirection() const { return lastDirection; }
    void setFollowDistance(float distance) { followDistance = distance; }

    // player state getters
    bool getIsInvincible() const { return isInvincible; }
    bool getIsUsingSpecialAbility() const { return isUsingSpecialAbility; }
    float getSpecialAbilityTimer() const { return specialAbilityTimer; }
    float getSpecialAbilityLimit() const { return specialAbilityLimit; }
    bool getIsOnGround() const { return onGround; }
    void setOnGround(bool ground) { onGround = ground; }
    bool getIsJumping() const { return isJumping; }

    // Lives and HP setters/getters
    static void setLives(int newLives) { lives = newLives; }
    static int getLives() { return lives; }
    static int getHP() { return hp; }
    static void setHP(int newHP) { hp = newHP; }

    // Character stats getters/setters
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getJumpStrength() const { return jumpStrength; }
    void setJumpStrength(float newJumpStrength) { jumpStrength = newJumpStrength; }
    float getGravity() const { return gravity; }
    void setGravity(float newGravity) { gravity = newGravity; }
    float getTerminalVelocity() const { return terminalVelocity; }
    void setTerminalVelocity(float newTerminalVelocity) { terminalVelocity = newTerminalVelocity; }

    // Following behavior getters/setters
    float* getLastPosition() { return lastPosition; }
    float getFollowLimit() const { return followLimit; }
    void setFollowLimit(float newFollowLimit) { followLimit = newFollowLimit; }
    float getJumpLimit() const { return jumpLimit; }
    void setJumpLimit(float newJumpLimit) { jumpLimit = newJumpLimit; }
    float getMaxFollowLimit() const { return maxFollowLimit; }
    void setMaxFollowLimit(float newMaxFollowLimit) { maxFollowLimit = newMaxFollowLimit; }

    float getSpecialAbilityLimt() const { return specialAbilityLimit; }
    void setSpecialAbilityLimit(float additionalTime) { specialAbilityLimit += additionalTime; }
    float getInvincibilityLimit() const { return invincibilityLimit; }
    void setInvincibilityLimit(int additionalTime) { invincibilityLimit += additionalTime; }

    // Hitbox dimension getters
    int getPlayerHeight() const { return playerHeight; }
    int getPlayerWidth() const { return playerWidth; }

    //velocity, directions, States getter and setter
    float getVelocityX() const { return velocity[0]; }
    float getVelocityY() const { return velocity[1]; }
    void setVelocityX(float v) { velocity[0] = v; }
    void setVelocityY(float v) { velocity[1] = v; }

    //hitbox
    Hitbox& getHitbox() { return hitbox; }
    void updateHitbox() {
        hitbox.updateHitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY);
    }

    //overriden polymorphic functions
    float* getPosition() override {
        return GameEntity::position;
    }

    void setPosition(float x, float y) override {
        GameEntity::position[0] = x;
        GameEntity::position[1] = y;

        lastPosition[0] = x;
        lastPosition[1] = y;
    }

    // Update animation state based on player state
    void updateAnimation(float deltaTime) {
        // Select the appropriate animation based on state and direction
        Animation* newAnimation = nullptr;

        if (isUsingSpecialAbility) {
            newAnimation = (lastDirection >= 0) ? &specialRightAnim : &specialLeftAnim;
        }
        else if (isJumping || !onGround) {
            newAnimation = (lastDirection >= 0) ? &jumpRightAnim : &jumpLeftAnim;
        }
        else if (isMoving) {
            newAnimation = (lastDirection >= 0) ? &walkRightAnim : &walkLeftAnim;
        }
        if (velocity[0] == 0) {
            newAnimation = (lastDirection >= 0) ? &idleRightAnim : &idleLeftAnim;
        }

        // Only change animation if needed
        if (newAnimation != currentAnimation) {
            currentAnimation = newAnimation;
        }

        // Update the current animation
        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }
    }

    virtual void draw(RenderWindow& window, float direction, float offset) override {
        if (currentAnimation) {
            // Update position for the animation sprite
            currentAnimation->setPosition(position[0] - offset, position[1]);

            // Draw the current animation
            currentAnimation->draw(window);
        }

        // Draw hitbox for debugging
        drawHitbox(window, offset);
    }

    virtual void update(char** grid, const int cell_size) override {
        applyPhysics(grid, cell_size);
        if (onGround) {
            isJumping = false;
        }

        if (isInvincible) {
            invincibilityTimer -= 0.016f;

            if (invincibilityTimer <= 0) {
                isInvincible = false;
                cout << "Player is no longer invincible." << endl;
            }
        }

        // Update animations with a fixed delta time
        updateAnimation(0.016f); // 60 FPS equivalent
        updateHitbox();
    }

    //polymorphic functions in player class;
    virtual void activateSpecialAbility() = 0;

    void render(RenderWindow& window, float offset) override {}

    virtual void applySpecialAbilityEffect(Event ev) {}

    virtual void move(float directionX, float gridWidth) {
        isMoving = (directionX != 0);

        for (int i = 0; i < 2; i++) {
            lastPosition[i] = position[i];
        }

		if (position[0] < 0) { // fixed screen size
            position[0] = 128;
        }
        else if (position[0] > gridWidth*64 ) {
            position[0] = 64 * (gridWidth);
        }

        velocity[0] = speed * directionX;
        position[0] += velocity[0];

        updateHitbox();

        

        if (directionX != 0) { lastDirection = directionX; }
    }

    virtual void jump() {
        if (onGround) {
            velocity[1] = jumpStrength;
            isJumping = true;
            onGround = false;

            // Play jump sound if available
            if (jumpSound.getBuffer() != nullptr) {
                jumpSound.play();
            }
        }
    }

    void applyPhysics(char** grid, const int cell_size) {

        applyGravity(grid, cell_size);

        if (velocity[1] < 0) {
            if (hitbox.checkTopCollision(grid, cell_size, 'w')) {
                float collisionPoint = hitbox.getTopCollisionPoint(grid, cell_size, 'W');

                if (collisionPoint > 0) {
                    position[1] = collisionPoint;
                    velocity[1] = 0.0f;
                }
            }
        }
        updateHitbox();
    }

    bool checkBottomCollision(char** grid, const int cell_size, char collisionSymbol, float& outCollisionY) {
        float offsetY = position[1] + velocity[1];
        int bottomLeftY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomLeftX = static_cast<int>((position[0] + hitboxFactorX) / cell_size);
        int bottomMiddleY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomMiddleX = static_cast<int>((position[0] + hitboxFactorX + playerWidth / 2) / cell_size);
        int bottomRightY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomRightX = static_cast<int>((position[0] + hitboxFactorX + playerWidth) / cell_size);

        const int maxHeight = 14;
        
        bool collision = false;

        if (bottomLeftY >= 0 && bottomLeftY < maxHeight && bottomLeftX >= 0 && bottomLeftX < maxWidth) {
            if (grid[bottomLeftY][bottomLeftX] == collisionSymbol) {
                collision = true;
            }
        }

        if (!collision && bottomMiddleY >= 0 && bottomMiddleY < maxHeight &&
            bottomMiddleX >= 0 && bottomMiddleX < maxWidth) {
            if (grid[bottomMiddleY][bottomMiddleX] == collisionSymbol) {
                collision = true;
            }
        }

        if (!collision && bottomRightY >= 0 && bottomRightY < maxHeight &&
            bottomRightX >= 0 && bottomRightX < maxWidth) {
            if (grid[bottomRightY][bottomRightX] == collisionSymbol) {
                collision = true;
            }
        }

        if (collision) {
            outCollisionY = (bottomLeftY * cell_size) - playerHeight - hitboxFactorY;
        }

        return collision;
    }

    void applyGravity(char** grid, const int cell_size) {
        // v = v0 + g
        if (!onGround) {
            velocity[1] += gravity;
            if (velocity[1] >= terminalVelocity) {
                velocity[1] = terminalVelocity;
            }
        }
        else {
            velocity[1] = 0.0f;
        }

        float offsetY = position[1] + velocity[1];
        float collisionY = 0.0f;

        bool collisionWall = checkBottomCollision(grid, cell_size, 'W', collisionY);
        bool collisionPlatform = checkBottomCollision(grid, cell_size, 'P', collisionY);
        bool collisionLevelTrigger = checkBottomCollision(grid, cell_size, 'T', collisionY);


        if (collisionWall || collisionPlatform ) {
            position[1] = collisionY;
            velocity[1] = 0.0f;
            onGround = true;
        }
        else {
            position[1] = offsetY;
            onGround = false;
        }

        updateHitbox();
    }

    virtual void followLeader(float gridWidth) {
        if (!isActive && leader != nullptr) {
            /*calculate leaderPosition and determined required X coordinates
            * check where follower is wrt to required X coordinates
            * if followLimit is exceeded that means either too far left/ far right
            then we determine whether the follower should move left/right
            -followLimit <= followerPosition <= followLinit*/

            float* leaderPosition = leader->getPosition();
            float requiredX = leaderPosition[0] - followDistance;
            float distanceToX = requiredX - position[0];

            float directionX = 0.0f;

            if (distanceToX > followLimit) {
                directionX = 1.0f;
            }
            else if (distanceToX < -followLimit) {
                directionX = -1.0f;
            }

            if (directionX != 0) {
                //follower lags a bit behind so speed is going to be just a bit slower
                move(directionX * 0.8, gridWidth);
            }

            if (position[0] < leaderPosition[0] - maxFollowLimit) {
                position[0] = leaderPosition[0] - followDistance;
            }

            //jump when leader is jumping 
            if (leader->getIsJumping() && leaderPosition[1] < position[1] - jumpLimit) {
                jump();
            }

            if (position[0] < 0) { // fixed screen size
                position[0] = 128;
            }
            else if (position[0] >= gridWidth * 64) {
                position[0] = 64 * (gridWidth - 1);
            }

        }
    }

    void takeDamage(const Hitbox& other) {
        if (!isInvincible && (hitbox.checkCollision(other))) {
            isInvincible = true;
            invincibilityTimer = invincibilityLimit;
            hp -= 10;

            cout << "Player took damage! HP: " << hp << endl;
            cout << "Player is now invincible for " << invincibilityLimit << " seconds." << endl;

            // Play damage sound if available
            if (damageSound.getBuffer() != nullptr) {
                damageSound.play();
            }
        }

        if (hp <= 0) {
            lives--;
            hp = 100;
        }

        if (lives <= 0) {
            //lose condition;
            cout << "Game over!" << endl;
            exit(0);
            /// EXIT SCREEN OR DISPLAY GAME OVER
        }
    }

    //////////////////////////////////////////
    /////////////// My additions /////////////
    //////////////////////////////////////////

    void setInvincible(bool invincible) { isInvincible = invincible; }

    void stopFalling() {
        velocity[1] = 0.0f;
        isJumping = false;
    }

    bool isAttacking() {
        // CHARACTER MUST BE KNUCKLES
        return isUsingSpecialAbility;
    }

    void stopJumping() {
        isJumping = false;
    }

    void stopMoving() {
        isMoving = false;
    }

    float getX() { return GameEntity::position[0]; }
    float getY() { return GameEntity::position[1]; }

    void drawHitbox(RenderWindow& window, float offset) {
        RectangleShape hitboxRect;
        hitboxRect.setPosition(hitbox.getX() - offset, hitbox.getY());
        hitboxRect.setSize(Vector2f(hitbox.getWidth(), hitbox.getHeight()));
        hitboxRect.setFillColor(Color::Transparent);
        hitboxRect.setOutlineColor(Color::Red);
        hitboxRect.setOutlineThickness(1.0f);
        window.draw(hitboxRect);
    }


    virtual void Boosts() {  }

    void Extra() {
        hp += 50;
    }

	void setGridWidth(int width) {
		maxWidth = width;
	}

};

int Player::hp = 100, Player::lives = 3;

// Tails class implementation
class Tails : public Player {
public:
    Tails() {
        idleRightTexture.loadFromFile("Data/Player/tailsR.png");
        idleLeftTexture.loadFromFile("Data/Player/tailsL.png");
        walkRightTexture.loadFromFile("Data/Player/Tails_Walk.png");
        walkLeftTexture.loadFromFile("Data/Player/Tails_WalkL.png");
        jumpRightTexture.loadFromFile("Data/Player/Tails_jump.png");
        jumpLeftTexture.loadFromFile("Data/Player/Tails_jumpL.png");
        specialRightTexture.loadFromFile("Data/Player/tails_SAR.png");
        specialLeftTexture.loadFromFile("Data/Player/tails_SAL.png");

        // Configure the sound effects
        jumpSoundBuffer.loadFromFile("Data/Sound/tails_jump.wav");
        jumpSound.setBuffer(jumpSoundBuffer);

        specialSoundBuffer.loadFromFile("Data/Sound/tails_fly.wav");
        specialSound.setBuffer(specialSoundBuffer);

        idleLeftAnim = Animation(idleLeftTexture, 46, 68, 1, 0.1f);
        idleRightAnim = Animation(idleRightTexture, 46, 68, 1, 0.1f);

        walkLeftAnim = Animation(walkLeftTexture, 855, 68, 10, 0.1f);
        walkRightAnim = Animation(walkRightTexture, 855, 68, 10, 0.1f);

        jumpLeftAnim = Animation(jumpLeftTexture, 180, 62, 2, 0.1f);
        jumpRightAnim = Animation(jumpRightTexture, 180, 62, 2, 0.1f);

        specialLeftAnim = Animation(specialLeftTexture, 381, 68, 4, 0.1f);
        specialRightAnim = Animation(specialRightTexture, 381, 68, 4, 0.1f);

        currentAnimation = &idleRightAnim;

        speed = 10.0f;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 7.0f;
        specialAbilityTimer = 0.0f;

        playerWidth = 46;
        playerHeight = 68;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    void activateSpecialAbility() override {
        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;
            specialAbilityTimer = specialAbilityLimit;
            cout << "Tails flying" << endl;

            // Play special ability sound
            specialSound.play();
        }
    }

    void applySpecialAbilityEffect(Event ev) override {
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::X) {
            if (isUsingSpecialAbility) {
                velocity[1] = -10.0f;
            }
        }
    }

	void Boosts() override {
		if (isUsingSpecialAbility) {
			 specialAbilityTimer += 4.0f;
			cout << "Flight Time Increased by 4.0s \n" << endl;
		}
	}

    void update(char** grid, const int cell_size) override {
        if (isUsingSpecialAbility) {
            if (hitbox.checkTopCollision(grid, cell_size, 'W')) {
                float collisionPoint = hitbox.getTopCollisionPoint(grid, cell_size, 'W');
                if (collisionPoint > 0) {
                    position[1] = collisionPoint;
                    velocity[1] = 0.0f;
                }
            }

            specialAbilityTimer -= 0.016f;
            velocity[1] += gravity * 0.2f;

            if (velocity[1] > 5.0f) {
                velocity[1] = 5.0f;
            }

            float offsetY = position[1] + velocity[1];

            float collisionY = 0.0f;
            bool collision = checkBottomCollision(grid, cell_size, 'W', collisionY);

            if (collision && velocity[1] > 0) {
                position[1] = collisionY;
                velocity[1] = 0.0f;
                onGround = true;
                isUsingSpecialAbility = false;
                cout << "Tails landed on ground and stopped flying." << endl;
            }
            else {
                position[1] = offsetY;
                onGround = false;
            }

            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;
                cout << "Tails flying ended" << endl;
            }

            updateHitbox();
            updateAnimation(0.016f);
        }
        else {
            Player::update(grid, cell_size);
        }
    }
};

// Sonic class implementation
class Sonic : public Player {
private:
    float normalSpeed;

public:
    Sonic() {
        idleRightTexture.loadFromFile("Data/Player/sonic.png");
        idleLeftTexture.loadFromFile("Data/Player/sonicL.png");
        walkRightTexture.loadFromFile("Data/Player/Sonic_WalkR.png");
        walkLeftTexture.loadFromFile("Data/Player/Sonic_WalkL.png");
        jumpRightTexture.loadFromFile("Data/Player/Sonic_JumpR.png");
        jumpLeftTexture.loadFromFile("Data/Player/Sonic_JumpL.png");
        specialRightTexture.loadFromFile("Data/Player/Sonic_SAR.png");
        specialLeftTexture.loadFromFile("Data/Player/Sonic_SAL.png");

        // Configure the sound effects
        jumpSoundBuffer.loadFromFile("Data/Sound/sonic_jump.wav");
        jumpSound.setBuffer(jumpSoundBuffer);

        specialSoundBuffer.loadFromFile("Data/Sound/spin_dash.wav");
        specialSound.setBuffer(specialSoundBuffer);

        idleLeftAnim = Animation(idleLeftTexture, 50, 70, 1, 0.1f);
        idleRightAnim = Animation(idleRightTexture, 50, 70, 1, 0.1f);

        walkLeftAnim = Animation(walkLeftTexture, 756, 70, 10, 0.1f);
        walkRightAnim = Animation(walkRightTexture, 756, 70, 10, 0.1f);

        jumpLeftAnim = Animation(jumpLeftTexture, 520, 56, 8, 0.1f);
        jumpRightAnim = Animation(jumpRightTexture, 520, 56, 8, 0.1f);

        specialLeftAnim = Animation(specialLeftTexture, 560, 70, 8, 0.1f);
        specialRightAnim = Animation(specialRightTexture, 560, 70, 8, 0.1f);

        currentAnimation = &idleRightAnim;

        normalSpeed = 14.0f;
        speed = normalSpeed;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 5.0f;
        specialAbilityTimer = 0.0f;

        playerWidth = 50;
        playerHeight = 70;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    void activateSpecialAbility() override {
        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;
            specialAbilityTimer = specialAbilityLimit;
            speed = normalSpeed + 7.0f;
            cout << "Sonic Zooming" << endl;

            // Play special ability sound
            specialSound.play();
        }
    }

    void update(char** grid, const int cell_size) override {
        Player::update(grid, cell_size);

        if (isUsingSpecialAbility) {
            specialAbilityTimer -= 0.016f;
            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;
                speed = normalSpeed;
                cout << "Sonic's speedies ended" << endl;
            }
        }
    }

    void Boosts() override {

        if (isUsingSpecialAbility) {
            speed = normalSpeed + 11.0f;  // overall change
            cout << "Speed increased +4 \n" << endl;
        }
    }


};

// Knuckles class implementation
class Knuckles : public Player {
private:
    bool isPunching;
    float punchingDuration;
    float punchingTimer;

public:
    Knuckles() {
        idleRightTexture.loadFromFile("Data/Player/knucklesR.png");
        idleLeftTexture.loadFromFile("Data/Player/knucklesL.png");
        walkRightTexture.loadFromFile("Data/Player/Knuckles_WalkR.png");
        walkLeftTexture.loadFromFile("Data/Player/Knuckles_WalkL.png");
        jumpRightTexture.loadFromFile("Data/Player/k_jump_right.png");
        jumpLeftTexture.loadFromFile("Data/Player/k_jump_left.png");
        specialRightTexture.loadFromFile("Data/Player/Knuckles_SAR.png");
        specialLeftTexture.loadFromFile("Data/Player/Knuckles_SAL.png");

        // Configure the sound effects
        jumpSoundBuffer.loadFromFile("Data/Sound/knuckles_jump.wav");
        jumpSound.setBuffer(jumpSoundBuffer);

        specialSoundBuffer.loadFromFile("Data/Sound/knuckles_punch.wav");
        specialSound.setBuffer(specialSoundBuffer);

        idleLeftAnim = Animation(idleLeftTexture, 48, 72, 1, 0.1f);
        idleRightAnim = Animation(idleRightTexture, 48, 72, 1, 0.1f);

        walkLeftAnim = Animation(walkLeftTexture, 910, 72, 12, 0.1f);
        walkRightAnim = Animation(walkRightTexture, 910, 72, 12, 0.1f);

        jumpLeftAnim = Animation(jumpLeftTexture, 1108, 72, 16, 0.1f);
        jumpRightAnim = Animation(jumpRightTexture, 1108, 72, 16, 0.1f);

        specialLeftAnim = Animation(specialLeftTexture, 220, 70, 2, 0.1f);
        specialRightAnim = Animation(specialRightTexture, 220, 70, 2, 0.1f);

        currentAnimation = &idleRightAnim;

        speed = 12.0f;
        isPunching = false;
        punchingDuration = 5.0f;
        punchingTimer = 0.0f;

        playerWidth = 48;
        playerHeight = 72;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    void activateSpecialAbility() override {
        if (!isPunching) {
            isPunching = true;
            isUsingSpecialAbility = true;
            punchingTimer = punchingDuration;
            cout << "Knuckles is now a bodmosh!" << endl;

            // Play special ability sound
            specialSound.play();
        }
    }

    void update(char** grid, const int cell_size) override {
        Player::update(grid, cell_size);

        if (isPunching) {
            punchingTimer -= 0.016f;
            if (punchingTimer <= 0) {
                isPunching = false;
                isUsingSpecialAbility = false;
                cout << "Knuckles' punching ended" << endl;
            }
        }
    }

    void Boosts() override{

        if (isUsingSpecialAbility) {

            invincibilityTimer += 15.0f;
			cout << "Knuckles is now invincible for 15 seconds!" << endl;
        }

    }


};



class PlayerFactory {
private:
    const int NUM_PLAYERS = 3;
    Player* players[3];
    int activePlayerIndex;

    float gridWidth;

public:

    PlayerFactory(float w) : activePlayerIndex(0), gridWidth(w) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i] = nullptr;
        }

        createAllPlayers();
    }

    ~PlayerFactory() {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            cout << "PlayerFactory destructor called" << endl;
            delete players[i];
            players[i] = nullptr;
        }
    }

    Player* createSonic() { return new Sonic; }
    Player* createTails() { return new Tails; }
    Player* createKnuckles() { return new Knuckles; }


    void createAllPlayers() {
        cout << "Creating characters:" << endl;
        players[0] = createSonic();
        players[1] = createTails();
        players[2] = createKnuckles();

        players[activePlayerIndex]->setActive(true);
		players[activePlayerIndex]->setGridWidth(gridWidth);

        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i]->setPosition(100.0f + i * 50.0f, 100.0f);
        }

        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->setLeader(players[activePlayerIndex]);
                players[i]->setFollowDistance(120.0f + i * 60.0f);
            }
        }
    }
    Player** getAllPlayers() {
        return players;
    }

    int getActivePlayerIndex() const {
        return activePlayerIndex;
    }
    Player* getActivePlayer() const {
        return players[activePlayerIndex];
    }

    Player* getPlayer(int index) const {
        if (index >= 0 && index < NUM_PLAYERS) {
            return players[index];
        }
        return nullptr;
    }

    void switchActivePlayer() {
        players[activePlayerIndex]->setActive(false);

        activePlayerIndex++;
        activePlayerIndex %= NUM_PLAYERS;

        players[activePlayerIndex]->setActive(true);


        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->setLeader(players[activePlayerIndex]);

            }
        }

        for (int j = 0; j < NUM_PLAYERS; j++) {
            if (j != activePlayerIndex) {
                players[j]->setLeader(players[activePlayerIndex]);
                players[j]->setFollowDistance(120.0f + j * 60.0f);
            }
        }

        cout << "Switched to player " << activePlayerIndex + 1 << endl;

    }
    void playerSwitchHandling(Event& event) {

        Player* active = players[activePlayerIndex];
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z) {
            switchActivePlayer();
        }
    }


    void draw(RenderWindow& window, float direction, float offset )  const {

        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->draw(window, players[i]->getLastDirection(), offset);
            }
        }
        players[activePlayerIndex]->draw(window, players[activePlayerIndex]->getLastDirection(), offset);
    }

    void update(char** grid, const int cell_size) const {
        //cout << "\n--- Frame update ---" << endl;

        // First, handle active player movement
        Player* activePlayer = players[activePlayerIndex];

       
        activePlayer->update(grid, cell_size);

        // Update followers
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->followLeader(gridWidth);
                players[i]->update(grid, cell_size);
            }
        }
        //cout << "--- End frame update ---\n" << endl;

        if (activePlayer->getY() >= 832) { // Bottomless Pit
            
            cout << "GAME OVER\n";
           

            exit(0);
        }


    }

};