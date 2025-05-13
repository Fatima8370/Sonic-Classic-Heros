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
    //===========================================================================
    // STATIC PLAYER PROPERTIES
    //===========================================================================
    static int hp;
    static int lives;

    //===========================================================================
    // MOVEMENT & PHYSICS PROPERTIES
    //===========================================================================
    bool isMoving;
    float velocity[2];
    float speed;
    float jumpStrength;
    bool onGround;
    bool isJumping;
    float gravity;
    float terminalVelocity;
    float lastDirection;
    float lastPosition[2];

    //===========================================================================
    // HITBOX & DIMENSIONS
    //===========================================================================
    int hitboxFactorX;
    int hitboxFactorY;
    int playerHeight;
    int playerWidth;
    int maxWidth;  // Grid width for boundary checking

    //===========================================================================
    // FOLLOWER BEHAVIOR PROPERTIES
    //===========================================================================
    Player* leader;
    int playerIndex;
    float followDistance;
    float followLimit;
    float jumpLimit;
    float maxFollowLimit;
    bool isActive;

    //===========================================================================
    // SPECIAL ABILITIES & STATUS EFFECTS
    //===========================================================================
    bool isUsingSpecialAbility;
    float specialAbilityTimer;
    float specialAbilityLimit;
    bool isInvincible;
    float invincibilityLimit;
    float invincibilityTimer;

    //===========================================================================
    // ANIMATIONS & VISUALS
    //===========================================================================
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

    //===========================================================================
    // AUDIO
    //===========================================================================
   

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    Player() : GameEntity('p'),
        playerIndex(-1),
        isInvincible(false),
        invincibilityLimit(3.0f),
        invincibilityTimer(0.0f),
        isMoving(false),
        speed(0.0f),
        jumpStrength(-30.0f),
        onGround(true),
        isJumping(false),
        isActive(false),
        leader(nullptr),
        followDistance(30.0f),
        followLimit(5.0f),
        jumpLimit(5.0f),
        maxFollowLimit(150.0f),
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

    //===========================================================================
    // POSITION & MOVEMENT GETTERS/SETTERS
    //===========================================================================
    bool getIsMoving() const { return isMoving; }
    float getVelocityX() const { return velocity[0]; }
    float getVelocityY() const { return velocity[1]; }
    void setVelocityX(float v) { velocity[0] = v; }
    void setVelocityY(float v) { velocity[1] = v; }
    float* getPosition() override { return GameEntity::position; }
    float getX() { return GameEntity::position[0]; }
    float getY() { return GameEntity::position[1]; }
    float getLastDirection() const { return lastDirection; }
    float* getLastPosition() { return lastPosition; }

    void setPosition(float x, float y) override {
        GameEntity::position[0] = x;
        GameEntity::position[1] = y;
        lastPosition[0] = x;
        lastPosition[1] = y;
    }

    //===========================================================================
    // LEADER/FOLLOWER GETTERS/SETTERS
    //===========================================================================
    void setLeader(Player* lead) { if (lead != this) this->leader = lead; }
    Player* getLeader() { return leader; }
    void setActive(bool ac) { isActive = ac; }
    bool getIsActive() const { return isActive; }
    void setFollowDistance(float distance) { followDistance = distance; }
    float getFollowLimit() const { return followLimit; }
    void setFollowLimit(float newFollowLimit) { followLimit = newFollowLimit; }
    float getJumpLimit() const { return jumpLimit; }
    void setJumpLimit(float newJumpLimit) { jumpLimit = newJumpLimit; }
    float getMaxFollowLimit() const { return maxFollowLimit; }
    void setMaxFollowLimit(float newMaxFollowLimit) { maxFollowLimit = newMaxFollowLimit; }
    void setPlayerIndex(int index) { playerIndex = index; }
    int getPlayerIndex() const { return playerIndex; }

    //===========================================================================
    // CHARACTER STATE GETTERS/SETTERS
    //===========================================================================
    bool getIsInvincible() const { return isInvincible; }
    void setInvincible(bool invincible) { isInvincible = invincible; }
    bool getIsUsingSpecialAbility() const { return isUsingSpecialAbility; }
    float getSpecialAbilityTimer() const { return specialAbilityTimer; }
    float getSpecialAbilityLimit() const { return specialAbilityLimit; }
    float getSpecialAbilityLimt() const { return specialAbilityLimit; }
    void setSpecialAbilityLimit(float additionalTime) { specialAbilityLimit += additionalTime; }
    float getInvincibilityLimit() const { return invincibilityLimit; }
    void setInvincibilityLimit(int additionalTime) { invincibilityLimit += additionalTime; }
    bool getIsOnGround() const { return onGround; }
    void setOnGround(bool ground) { onGround = ground; }
    bool getIsJumping() const { return isJumping; }

    //===========================================================================
    // LIVES & HP STATIC GETTERS/SETTERS
    //===========================================================================
    static void setLives(int newLives) { lives = newLives; }
    static int getLives() { return lives; }
    static int getHP() { return hp; }
    static void setHP(int newHP) { hp = newHP; }

    //===========================================================================
    // PHYSICS & MOVEMENT PARAMETERS GETTERS/SETTERS
    //===========================================================================
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getJumpStrength() const { return jumpStrength; }
    void setJumpStrength(float newJumpStrength) { jumpStrength = newJumpStrength; }
    float getGravity() const { return gravity; }
    void setGravity(float newGravity) { gravity = newGravity; }
    float getTerminalVelocity() const { return terminalVelocity; }
    void setTerminalVelocity(float newTerminalVelocity) { terminalVelocity = newTerminalVelocity; }
    void setGridWidth(int width) { maxWidth = width; }

    //===========================================================================
    // HITBOX GETTERS & METHODS
    //===========================================================================
    int getPlayerHeight() const { return playerHeight; }
    int getPlayerWidth() const { return playerWidth; }
    Hitbox& getHitbox() { return hitbox; }

    void updateHitbox() {
        hitbox.updateHitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY);
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
    // ANIMATION & RENDERING METHODS
    //===========================================================================
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

        if (currentAnimation) {
            currentAnimation->update(deltaTime);
        }
    }

    virtual void draw(RenderWindow& window, float direction, float offset) override {
        if (currentAnimation) {
            currentAnimation->setPosition(position[0] - offset, position[1]);

            currentAnimation->draw(window);
        }

        // Draw hitbox for debugging
        drawHitbox(window, offset);
    }

    void render(RenderWindow& window, float offset) override {}

    //===========================================================================
    // MOVEMENT & PHYSICS METHODS
    //===========================================================================
    virtual void move(float directionX, float gridWidth) {
        isMoving = (directionX != 0);

        for (int i = 0; i < 2; i++) {
            lastPosition[i] = position[i];
        }

        if (position[0] < 0) { // fixed screen size
            position[0] = 128;
        }
        else if (position[0] > gridWidth * 64) {
            position[0] = 64 * (gridWidth);
        }

        velocity[0] = speed * directionX;
        position[0] += velocity[0];

        updateHitbox();

        if (directionX != 0) {
            lastDirection = directionX;
        }
    }

    virtual void jump() {
        if (onGround) {
            velocity[1] = jumpStrength;
            isJumping = true;
            onGround = false;            
        }
    }

    void stopFalling() {
        velocity[1] = 0.0f;
        isJumping = false;
    }

    void stopJumping() {
        isJumping = false;
    }

    void stopMoving() {
        isMoving = false;
    }

    //===========================================================================
    // PHYSICS & COLLISION METHODS
    //===========================================================================
    void applyPhysics(char** grid, const int cell_size) {
        applyGravity(grid, cell_size);
        if (velocity[1] < 0) {
            if (hitbox.checkTopCollision(grid, cell_size)) {
                float collisionPoint = hitbox.getTopCollisionPoint(grid, cell_size);
                if (collisionPoint > 0) {
                    position[1] = collisionPoint;
                    velocity[1] = 0.0f;
                    //cout << "Player: Hit ceiling at position Y = " << collisionPoint << endl;
                }
            }
        }
        updateHitbox();
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

        if (collisionWall || collisionPlatform) {
            position[1] = collisionY;
            velocity[1] = 0.0f;
            onGround = true;

            if (velocity[1] > 5.0f) {
               // cout << "Player: Landed on ground from falling velocity " << velocity[1] << endl;
            }
        }
        else {
            position[1] = offsetY;
            onGround = false;
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

    //===========================================================================
    // UPDATE & GAME LOGIC METHODS
    //===========================================================================
    virtual void update(char** grid, const int cell_size) override {
        applyPhysics(grid, cell_size);
        if (onGround) {
            isJumping = false;
        }

        if (isInvincible) {
            invincibilityTimer -= 0.016f;

            if (invincibilityTimer <= 0) {
                isInvincible = false;
                cout << "Player: Invincibility has worn off." << endl;
            }
        }

        updateAnimation(0.016f);
        updateHitbox();
    }

    virtual void followLeader(float gridWidth) {
        if (!isActive && leader != nullptr) {
            // Calculate leader position and determine required X coordinates
            // Check where follower is with respect to required X coordinates
            // If followLimit is exceeded that means either too far left/right
            // Then we determine whether the follower should move left/right
            // -followLimit <= followerPosition <= followLimit

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
                // Follower lags a bit behind so speed is going to be just a bit slower
                move(directionX * 0.8, gridWidth);
            }
            // Horizontal teleport if too far away
            if (position[0] < leaderPosition[0] - maxFollowLimit) {
                position[0] = leaderPosition[0] - followDistance;
                //cout << "Follower: Teleported horizontally to stay within maxFollowLimit" << endl;
            }

            // NEW: Vertical teleport if fallen too far
            if (position[1] > leaderPosition[1] + 200.0f) {  // 200 pixels below leader
                position[1] = leaderPosition[1];
                velocity[1] = 0.0f;
                onGround = true;
               // cout << "Follower: Teleported vertically to rejoin leader" << endl;
            }


            // Jump when leader is jumping
            if (leader->getIsJumping() && leaderPosition[1] < position[1] - jumpLimit) {
                jump();
               // cout << "Follower: Jumping to follow leader" << endl;
            }
        }
    }

    //===========================================================================
    // SPECIAL ABILITIES & COMBAT METHODS
    //===========================================================================
    virtual void activateSpecialAbility() = 0;

    virtual void applySpecialAbilityEffect(Event ev) {}

    virtual void Boosts() {}

    void takeDamage(const Hitbox& other) {
        if (!isInvincible && (hitbox.checkCollision(other))) {
            isInvincible = true;
            invincibilityTimer = invincibilityLimit;
            hp -= 10;

           // cout << "Player: Took damage! HP: " << hp << endl;
            //cout << "Player: Now invincible for " << invincibilityLimit << " seconds." << endl;

            // Play damage sound if available
            
        }

        if (hp <= 0) {
            lives--;
            hp = 100;
           // cout << "Player: Lost a life! Lives remaining: " << lives << endl;
        }

        if (lives <= 0) {
            // Lose condition
            //cout << "Player: Game over! No lives remaining." << endl;
            exit(0);
            // EXIT SCREEN OR DISPLAY GAME OVER
        }
    }

    bool isAttacking() {
        // CHARACTER MUST BE KNUCKLES
        return isUsingSpecialAbility;
    }

    void Extra() {
        hp += 50;
        cout << "Player: Gained 50 HP! Current HP: " << hp << endl;
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

       

        // Set up animations with proper frame sizes and timing
        idleLeftAnim = Animation(idleLeftTexture, 46, 68, 1, 0.1f);
        idleRightAnim = Animation(idleRightTexture, 46, 68, 1, 0.1f);

        walkLeftAnim = Animation(walkLeftTexture, 855, 68, 10, 0.05f);
        walkRightAnim = Animation(walkRightTexture, 855, 68, 10, 0.05f);

        jumpLeftAnim = Animation(jumpLeftTexture, 180, 62, 2, 0.05f);
        jumpRightAnim = Animation(jumpRightTexture, 180, 62, 2, 0.05f);

        specialLeftAnim = Animation(specialLeftTexture, 381, 68, 4, 0.05f);
        specialRightAnim = Animation(specialRightTexture, 381, 68, 4, 0.05f);

        currentAnimation = &idleRightAnim;

        // Initialize movement and ability parameters
        speed = 10.0f;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 7.0f;
        specialAbilityTimer = 0.0f;

        // Set up hitbox
        playerWidth = 46;
        playerHeight = 68;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    void activateSpecialAbility() override {
        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;
            specialAbilityTimer = specialAbilityLimit;
            cout << "Tails: Flying mode activated! Timer set to " << specialAbilityLimit << " seconds" << endl;

            // Play special ability sound
          
        }
    }

    void applySpecialAbilityEffect(Event ev) override {
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::X) {
            if (isUsingSpecialAbility) {
                velocity[1] = -10.0f;
                cout << "Tails: Propelling upward!" << endl;
            }
        }
    }

    void Boosts() override {
        if (isUsingSpecialAbility) {
            specialAbilityTimer += 4.0f;
            cout << "Tails: Flight Time Boost! Increased by 4.0s (New total: "
                << specialAbilityTimer << "s)" << endl;
        }
    }

    void update(char** grid, const int cell_size) override {
        if (isUsingSpecialAbility) {
            // Check for ceiling collision using the updated method (without character parameter)
            if (hitbox.checkTopCollision(grid, cell_size)) {
                float collisionPoint = hitbox.getTopCollisionPoint(grid, cell_size);
                if (collisionPoint > 0) {
                    position[1] = collisionPoint;
                    velocity[1] = 0.0f;
                    //cout << "Tails: Hit ceiling at position Y = " << collisionPoint << endl;
                }
            }

            // Update flight timer and apply reduced gravity
            specialAbilityTimer -= 0.016f;
            velocity[1] += gravity * 0.2f;

            // Cap falling speed during flight
            if (velocity[1] > 5.0f) {
                velocity[1] = 5.0f;
            }

            float offsetY = position[1] + velocity[1];
            float collisionY = 0.0f;

            // Check for ground/platform collisions
            bool collisionWall = checkBottomCollision(grid, cell_size, 'W', collisionY);
            bool collisionPlatform = checkBottomCollision(grid, cell_size, 'P', collisionY);
            bool collisionLevelTrigger = checkBottomCollision(grid, cell_size, 'T', collisionY);

            // Handle landing
            if ((collisionWall || collisionPlatform) && velocity[1] > 0) {
                position[1] = collisionY;
                velocity[1] = 0.0f;
                onGround = true;
                isUsingSpecialAbility = false;
               // cout << "Tails: Landed on ground at position Y = " << collisionY << ", flight mode ended" << endl;
            }
            else {
                position[1] = offsetY;
                onGround = false;

                if (velocity[1] > 0) {
                    cout << "Tails: Descending at velocity " << velocity[1] << endl;
                }
                else if (velocity[1] < 0) {
                    cout << "Tails: Ascending at velocity " << velocity[1] << endl;
                }
            }

            // End flying if timer runs out
            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;
                cout << "Tails: Flight time limit reached, ending flight mode" << endl;
            }
            else if (specialAbilityTimer < 2.0f) {
                cout << "Tails: Flight time running low! " << specialAbilityTimer << "s remaining" << endl;
            }

            // Update hitbox position and animation
            updateHitbox();
            updateAnimation(0.016f);
        }
        else {
            // Use normal player physics when not flying
            Player::update(grid, cell_size);
        }
    }
};





//===========================================================================
// SONIC CLASS IMPLEMENTATION
//===========================================================================
class Sonic : public Player {
private:
    //===========================================================================
    // SONIC-SPECIFIC PROPERTIES
    //===========================================================================
    float normalSpeed;  // Base speed before special ability

public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Sonic() {
        // Load character textures
        idleRightTexture.loadFromFile("Data/Player/sonic.png");
        idleLeftTexture.loadFromFile("Data/Player/sonicL.png");
        walkRightTexture.loadFromFile("Data/Player/Sonic_WalkR.png");
        walkLeftTexture.loadFromFile("Data/Player/Sonic_WalkL.png");
        jumpRightTexture.loadFromFile("Data/Player/Sonic_JumpR.png");
        jumpLeftTexture.loadFromFile("Data/Player/Sonic_JumpL.png");
        specialRightTexture.loadFromFile("Data/Player/Sonic_SAR.png");
        specialLeftTexture.loadFromFile("Data/Player/Sonic_SAL.png");

       

        // Initialize animations with proper frame sizes and timing
        idleLeftAnim = Animation(idleLeftTexture, 50, 70, 1, 0.05f);
        idleRightAnim = Animation(idleRightTexture, 50, 70, 1, 0.05f);

        walkLeftAnim = Animation(walkLeftTexture, 756, 70, 10, 0.05f);
        walkRightAnim = Animation(walkRightTexture, 756, 70, 10, 0.05f);

        jumpLeftAnim = Animation(jumpLeftTexture, 520, 56, 8, 0.05f);
        jumpRightAnim = Animation(jumpRightTexture, 520, 56, 8, 0.05f);

        specialLeftAnim = Animation(specialLeftTexture, 560, 70, 8, 0.05f);
        specialRightAnim = Animation(specialRightTexture, 560, 70, 8, 0.05f);

        currentAnimation = &idleRightAnim;

        // Set Sonic-specific properties
        normalSpeed = 14.0f;
        speed = normalSpeed;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 5.0f;
        specialAbilityTimer = 0.0f;

        // Set hitbox dimensions
        playerWidth = 50;
        playerHeight = 70;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    //===========================================================================
    // SPECIAL ABILITY METHODS
    //===========================================================================
    void activateSpecialAbility() override {
        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;
            specialAbilityTimer = specialAbilityLimit;
            speed = normalSpeed + 7.0f;
           // cout << "Sonic: Speed boost activated! Speed increased from " << normalSpeed
             //   << " to " << speed << " for " << specialAbilityLimit << " seconds" << endl;

       
        }
    }

    void Boosts() override {
        if (isUsingSpecialAbility) {
            speed = normalSpeed + 11.0f;  // Increased boost
           // cout << "Sonic: Power boost! Speed increased further to " << speed
             //   << " (+" << (speed - normalSpeed) << " from normal)" << endl;
        }
    }

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
    void update(char** grid, const int cell_size) override {
        // Call parent update first
        Player::update(grid, cell_size);

        // Handle speed boost timer
        if (isUsingSpecialAbility) {
            specialAbilityTimer -= 0.016f;

            if (specialAbilityTimer <= 1.0f && specialAbilityTimer > 0.95f) {
                cout << "Sonic: Speed boost ending soon!" << endl;
            }

            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;
                speed = normalSpeed;
                cout << "Sonic: Speed boost ended, returning to normal speed of " << normalSpeed << endl;
            }
        }
    }
};

//===========================================================================
// KNUCKLES CLASS IMPLEMENTATION
//===========================================================================
class Knuckles : public Player {
private:
    //===========================================================================
    // KNUCKLES-SPECIFIC PROPERTIES
    //===========================================================================
    bool isPunching;          // Tracking punching state
    float punchingDuration;   // How long the punch lasts
    float punchingTimer;      // Current time left in punch

public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Knuckles() {
        // Load character textures
        idleRightTexture.loadFromFile("Data/Player/knucklesR.png");
        idleLeftTexture.loadFromFile("Data/Player/knucklesL.png");
        walkRightTexture.loadFromFile("Data/Player/Knuckles_WalkR.png");
        walkLeftTexture.loadFromFile("Data/Player/Knuckles_WalkL.png");
        jumpRightTexture.loadFromFile("Data/Player/k_jump_right.png");
        jumpLeftTexture.loadFromFile("Data/Player/k_jump_left.png");
        specialRightTexture.loadFromFile("Data/Player/Knuckles_SAR.png");
        specialLeftTexture.loadFromFile("Data/Player/Knuckles_SAL.png");

      

        // Initialize animations with proper frame sizes and timing
        idleLeftAnim = Animation(idleLeftTexture, 48, 72, 1, 0.05f);
        idleRightAnim = Animation(idleRightTexture, 48, 72, 1, 0.05f);

        walkLeftAnim = Animation(walkLeftTexture, 910, 72, 12, 0.05f);
        walkRightAnim = Animation(walkRightTexture, 910, 72, 12, 0.05f);

        jumpLeftAnim = Animation(jumpLeftTexture, 1108, 72, 16, 0.05f);
        jumpRightAnim = Animation(jumpRightTexture, 1108, 72, 16, 0.05f);

        specialLeftAnim = Animation(specialLeftTexture, 220, 70, 2, 0.05f);
        specialRightAnim = Animation(specialRightTexture, 220, 70, 2, 0.05f);

        currentAnimation = &idleRightAnim;

        // Set Knuckles-specific properties
        speed = 12.0f;
        isPunching = false;
        punchingDuration = 5.0f;
        punchingTimer = 0.0f;

        // Set hitbox dimensions
        playerWidth = 48;
        playerHeight = 72;
        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY,
            playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }

    //===========================================================================
    // SPECIAL ABILITY METHODS
    //===========================================================================
    void activateSpecialAbility() override {
        if (!isPunching) {
            isPunching = true;
            isUsingSpecialAbility = true;
            punchingTimer = punchingDuration;

            cout << "Knuckles: Powerful punch mode activated for " << punchingDuration;
           
        }
    }

    void Boosts() override {
        if (isUsingSpecialAbility) {
            invincibilityTimer += 15.0f;
            isInvincible = true;
            cout << "Knuckles: Power boost received! Extended invincibility by 15 seconds!" << endl;
            cout << "Knuckles: Total invincibility remaining: " << invincibilityTimer << " seconds" << endl;
        }
    }

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
    void update(char** grid, const int cell_size) override {
        // Call parent update first
        Player::update(grid, cell_size);

        // Handle punching timer
        if (isPunching) {
            punchingTimer -= 0.016f;

            if (punchingTimer <= 1.0f && punchingTimer > 0.95f) {
                cout << "Knuckles: Punch mode ending soon!" << endl;
            }

            if (punchingTimer <= 0) {
                isPunching = false;
                isUsingSpecialAbility = false;
                cout << "Knuckles: Punch mode ended, returning to normal state" << endl;
            }
        }
    }
};

class PlayerFactory {
private:
    //===========================================================================
    // MEMBER VARIABLES
    //===========================================================================
    const int NUM_PLAYERS = 3;
    Player* players[3];
    int activePlayerIndex;
    float gridWidth;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    PlayerFactory(float w) : activePlayerIndex(0), gridWidth(w) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i] = nullptr;
        }

        createAllPlayers();
    }

    ~PlayerFactory() {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            cout << "PlayerFactory: Destroying player " << i << endl;
            delete players[i];
            players[i] = nullptr;
        }
    }

    //===========================================================================
    // PLAYER CREATION METHODS
    //===========================================================================
    Player* createSonic() { return new Sonic; }
    Player* createTails() { return new Tails; }
    Player* createKnuckles() { return new Knuckles; }

    void createAllPlayers() {
        cout << "PlayerFactory: Creating all characters..." << endl;

        // Initialize player instances
        players[0] = createSonic();
        players[1] = createTails();
        players[2] = createKnuckles();

        // Set grid width for ALL players first
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i]->setGridWidth(gridWidth);
        }

        // Set active player
        players[activePlayerIndex]->setActive(true);

        // Set initial positions
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i]->setPlayerIndex(i);
            players[i]->setPosition(300.0f + i * 50.0f, 100.0f);
            cout << "PlayerFactory: Position set for player " << i << endl;
        }

        // Configure follower relationships
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->setLeader(players[activePlayerIndex]);
                players[i]->setFollowDistance(120.0f + i * 60.0f);
            }
        }

       // cout << "PlayerFactory: All players created and initialized" << endl;
    }
    //===========================================================================
    // PLAYER ACCESS METHODS
    //===========================================================================
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

    //===========================================================================
    // PLAYER SWITCHING METHODS
    //===========================================================================
    void switchActivePlayer() {
        // Deactivate current player
        players[activePlayerIndex]->setActive(false);

        // Cycle to next player
        activePlayerIndex++;
        activePlayerIndex %= NUM_PLAYERS;

        // Activate new player
        players[activePlayerIndex]->setActive(true);

        // Update leader-follower relationships
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->setLeader(players[activePlayerIndex]);
                players[i]->setFollowDistance(120.0f + i * 60.0f);
            }
        }

     //   cout << "PlayerFactory: Switched to player " << activePlayerIndex + 1
       //     << " (" << (activePlayerIndex == 0 ? "Sonic" :
         //       activePlayerIndex == 1 ? "Tails" : "Knuckles") << ")" << endl;
    }

    void playerSwitchHandling(Event& event) {
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z) {
            switchActivePlayer();
        }
    }

    //===========================================================================
    // RENDERING METHODS
    //===========================================================================
    void draw(RenderWindow& window, float direction, float offset) const {
        // Draw non-active players first (rendering order)
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->draw(window, players[i]->getLastDirection(), offset);
            }
        }

        // Draw active player on top
        players[activePlayerIndex]->draw(window, players[activePlayerIndex]->getLastDirection(), offset);
    }

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
    void update(char** grid, const int cell_size) const {
        // Get active player reference
        Player* activePlayer = players[activePlayerIndex];

        // Update active player first
        activePlayer->update(grid, cell_size);

        // Update followers
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->followLeader(gridWidth);
                players[i]->update(grid, cell_size);
            }
        }

        // Check for game over condition (falling into pit)
        if (activePlayer->getY() >= 832) {
           // cout << "PlayerFactory: GAME OVER - Player fell into bottomless pit!" << endl;
            // exit(0); // Commented out to prevent immediate termination
        }
    }
};