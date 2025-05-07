#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Hitbox.h"
#include "GameEntity.h"


using namespace sf;
using namespace std;



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


    Texture playerTexture;
    Sprite playerSprite;
    float scaleX, scaleY;

public:
    Player() : GameEntity(),
        isInvincible(false),
        invincibilityLimit(3.0f),
        invincibilityTimer(0.0f),

        scaleX(2.5f),
        scaleY(2.5f),

        isMoving(false),


        speed(0.0f),
        jumpStrength(-20.0f),
        onGround(true),
        isJumping(false),
        isActive(false),
        leader(nullptr),

        followDistance(60.0f),
        followLimit(5.0f),
        jumpLimit(5.0f),
        maxFollowLimit(100.0f),


        lastDirection(1.0f),
        gravity(1.0f),
        terminalVelocity(20.0f),


        hitboxFactorX(static_cast<int>(8 * 2.5f)),
        hitboxFactorY(static_cast<int>(5 * 2.5f)),
        playerHeight(static_cast<int>(35 * 2.5f)),
        playerWidth(static_cast<int>(24 * 2.5f)),


        isUsingSpecialAbility(false),
        specialAbilityTimer(0.0f),
        specialAbilityLimit(0.0f)
    {

        for (int i = 0; i < 2; i++) {
            lastPosition[i] = position[i];
            velocity[i] = 0.0f;
        }

        hitbox = Hitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY, playerWidth - 2 * hitboxFactorX, playerHeight - 2 * hitboxFactorY);
    }




    virtual ~Player() {
        leader = nullptr;
    }



    //position getter setter
    float* getLastPosition() { return lastPosition; }

    float* getPosition() override {
        return GameEntity::position;  // Return base class position
    }

    void setPosition(float x, float y) override {
        GameEntity::position[0] = x;  // Update base class position
        GameEntity::position[1] = y;
        lastPosition[0] = x;  // Also update last position
        lastPosition[1] = y;
    }

    float getX() { return GameEntity::position[0]; }
    float getY() {return GameEntity::position[1]; }


    //leader related getter setters
    void setLeader(Player* lead) { if (lead != this) this->leader = lead; }
    Player* getLeader() { return leader; }
    void setActive(bool ac) { isActive = ac; }
    bool getIsActive() const { return isActive; }
    float getLastDirection() const { return lastDirection; }
    void setFollowDistance(float distance) { followDistance = distance; }

    // Player state getters
    bool getIsInvincible() const { return isInvincible; }
    bool getIsUsingSpecialAbility() const { return isUsingSpecialAbility; }
    float getSpecialAbilityTimer() const { return specialAbilityTimer; }
    float getSpecialAbilityLimit() const { return specialAbilityLimit; }

    // Lives and HP setters/getters
    static void setLives(int newLives) { lives = newLives; }

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
    float getFollowLimit() const { return followLimit; }
    void setFollowLimit(float newFollowLimit) { followLimit = newFollowLimit; }
    float getJumpLimit() const { return jumpLimit; }
    void setJumpLimit(float newJumpLimit) { jumpLimit = newJumpLimit; }
    float getMaxFollowLimit() const { return maxFollowLimit; }
    void setMaxFollowLimit(float newMaxFollowLimit) { maxFollowLimit = newMaxFollowLimit; }

    // Hitbox dimension getters
    int getPlayerHeight() const { return playerHeight; }
    int getPlayerWidth() const { return playerWidth; }

    //velocity, directions, States getter and setter
    float getVelocityX() const { return velocity[0]; }
    float getVelocityY() const { return velocity[1]; }
    void setVelocityX(float v) { velocity[0] = v; }
    void setVelocityY(float v) { velocity[1] = v; }

    bool getIsOnGround() const { return onGround; }
    void setOnGround(bool ground) { onGround = ground; }
    bool getIsJumping() const { return isJumping; }

    static int getHP() { return hp; }
    static void setHP(int newHP) { hp = newHP; }

    //htibox
    Hitbox& getHitbox() { return hitbox; }
    void updateHitbox() {
        hitbox.updateHitbox(position[0] + hitboxFactorX, position[1] + hitboxFactorY);
    }


    //all polymorphic functions 
    virtual void activateSpecialAbility() = 0;
    virtual void applySpecialAbilityEffect(Event ev) {}

    virtual void draw(RenderWindow& window, float direction, float offset) override {
        playerSprite.setPosition(position[0]-offset, position[1]);

        if (direction < 0) {
            playerSprite.setScale(-scaleX, scaleY);
        }
        else {
            playerSprite.setScale(scaleX, scaleY);
        }
        window.draw(playerSprite);
    }

    bool getIsMoving() const { return isMoving; }


    virtual void move(float directionX) {

        if (position[0] < 64) position[0] = 64;

        isMoving = (directionX != 0);

        for (int i = 0; i < 2; i++) {
            lastPosition[i] = position[i];
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
        }
    }


    virtual void followLeader() {

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
                move(directionX * 0.8);
            }

            if (position[0] < leaderPosition[0] - maxFollowLimit) {
                position[0] = leaderPosition[0] - followDistance;
            }

            //jump when leader is jumping 
            if (leader->getIsJumping() && leaderPosition[1] < position[1] - jumpLimit) {
                jump();
            }
        }
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
        int bottomLeftY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomLeftX = static_cast<int>((position[0] + hitboxFactorX) / cell_size);
        int bottomMiddleY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomMiddleX = static_cast<int>((position[0] + hitboxFactorX + playerWidth / 2) / cell_size);
        int bottomRightY = static_cast<int>((offsetY + hitboxFactorY + playerHeight) / cell_size);
        int bottomRightX = static_cast<int>((position[0] + hitboxFactorX + playerWidth) / cell_size);
        const int maxHeight = 14;
        const int maxWidth = 110;

        bool collision = false;
        if (bottomLeftY >= 0 && bottomLeftY < maxHeight && bottomLeftX >= 0 && bottomLeftX < maxWidth) {
            if (grid[bottomLeftY][bottomLeftX] == 'w') {
                collision = true;
            }
        }
        if (!collision && bottomMiddleY >= 0 && bottomMiddleY < maxHeight &&
            bottomMiddleX >= 0 && bottomMiddleX < maxWidth) {
            if (grid[bottomMiddleY][bottomMiddleX] == 'w') {
                collision = true;
            }
        }
        if (!collision && bottomRightY >= 0 && bottomRightY < maxHeight &&
            bottomRightX >= 0 && bottomRightX < maxWidth) {
            if (grid[bottomRightY][bottomRightX] == 'w') {
                collision = true;
            }
        }
        if (collision) {
            position[1] = (bottomLeftY * cell_size) - playerHeight - hitboxFactorY;
            velocity[1] = 0.0f;
            onGround = true;
        }
        else {
            position[1] = offsetY;
            onGround = false;
        }
        updateHitbox();
    }



    void update(char** grid, const int cell_size) override {
        applyGravity(grid, cell_size);
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
        updateHitbox();
    }


    void takeDamage(const Hitbox& other) {

        if (!isInvincible && (hitbox.checkCollision(other))) {

            isInvincible = true;
            invincibilityTimer = invincibilityLimit;
            hp -= 10;

        }
        if (hp <= 0) {
            lives--;
            hp = 100;
        }

        if (lives <= 0) {
            //lose condition;
            cout << "Game over!" << endl;
            exit(0);
        }
    }

    static int getLives() { return lives; }

}; int Player::hp = 100, Player::lives = 3;





class Tails : public Player {
private:


public:
    Tails() {
        scaleX = 3.2;
        scaleY = 3.2;

        //followDistance = 60.0f;


        playerHeight = (static_cast<int>(35 * scaleX));
        playerWidth = (static_cast<int>(24 * scaleY));


        speed = 10.0f;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 7.0f;
        specialAbilityTimer = 0.0f;


        playerTexture.loadFromFile("Data/tails.png");
        playerSprite.setTexture(playerTexture);
        playerSprite.setScale(scaleX, scaleY);
    }

    void activateSpecialAbility() override {

        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;
            specialAbilityTimer = specialAbilityLimit;
            cout << "Tails flying" << endl;
        }
    }

    void applySpecialAbilityEffect(Event ev) override {
        if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::X) {
            if (isUsingSpecialAbility) {
                velocity[1] = -8.0f;
            }
        }


    }
    void update(char** grid, const int cell_size) override {

        if (isUsingSpecialAbility) {

            specialAbilityTimer -= 0.016f;

            velocity[1] += gravity * 0.5f;

            //same logic as terminal velocity
            if (velocity[1] > 5.0f) {
                velocity[1] = 5.0f;
            }

            position[1] += velocity[1];

            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;
                cout << "Tails flying ended" << endl;

            }
            updateHitbox();

        }
        else {
            Player::update(grid, cell_size);

        }

    }

};








class Sonic : public Player {
private:
    float normalSpeed;

    Texture stillRightTexture;
    Texture walkRightTexture;
    Texture stillLeftTexture;
    Texture walkLeftTexture;
public:


    Sonic() {
        scaleX = 2.3;
        scaleY = 2.3;
        playerHeight = (static_cast<int>(35 * scaleX));
        playerWidth = (static_cast<int>(24 * scaleY));


        normalSpeed = speed = 14.0f;
        isUsingSpecialAbility = false;
        specialAbilityLimit = 5.0f;
        specialAbilityTimer = 0.0f;

        stillRightTexture.loadFromFile("Sprites/sonic/sonic_right_still.png");
        walkRightTexture.loadFromFile("Sprites/sonic/sonic_walk_right.png");
        stillLeftTexture.loadFromFile("Data/sonic.png");
        walkLeftTexture.loadFromFile("Sprites/sonic/sonic_walk_left.png");

        // Start with the right-facing still texture
        playerTexture = stillRightTexture;
        playerSprite.setTexture(playerTexture);
        playerSprite.setScale(scaleX, scaleY);
    }


    void activateSpecialAbility() override {

        if (!isUsingSpecialAbility) {
            isUsingSpecialAbility = true;

            specialAbilityTimer = specialAbilityLimit;
            speed = normalSpeed + 7.0f;

            cout << "Sonic Zooming" << endl;

        }
    }


    void draw(RenderWindow& window, float direction, float offset) override {
        // Select the appropriate texture based on movement and direction
        if (direction < 0) { // Left direction
            if (isMoving) {
                playerSprite.setTexture(walkLeftTexture);
            }
            else {
                playerSprite.setTexture(stillLeftTexture);
            }
        }
        else { // Right direction
            if (isMoving) {
                playerSprite.setTexture(walkRightTexture);
            }
            else {
                playerSprite.setTexture(stillRightTexture);
            }
        }

        // No need to flip the sprite since we're using direction-specific textures
        playerSprite.setScale(scaleX, scaleY);

        // Set position and draw
        playerSprite.setPosition(position[0]-offset, position[1]);
        window.draw(playerSprite);
    }

    void update(char** grid, const int cell_size) override {



        if (isUsingSpecialAbility) {

            specialAbilityTimer -= 0.016f;

            if (specialAbilityTimer <= 0) {
                isUsingSpecialAbility = false;

                speed = normalSpeed;
                cout << "Sonic's speedies ended" << endl;

            }
            updateHitbox();
        }
        else {
            Player::update(grid, cell_size);
        }
    }



};

class Knuckles : public Player {
private:
    bool isPunching;
    float punchingDuration;
    float punchingTimer;

public:
    Knuckles() {
        scaleX = 2.7;
        scaleY = 2.7;
        playerHeight = (static_cast<int>(35 * scaleX));
        playerWidth = (static_cast<int>(24 * scaleY));


        speed = 12.0f;
        isPunching = false;

        punchingDuration = 5.0f;
        punchingTimer = 0.0f;

        playerTexture.loadFromFile("Sprites/knuckles/knuckles.png");
        playerSprite.setTexture(playerTexture);
        playerSprite.setScale(scaleX, scaleY);
    }


    void activateSpecialAbility() override {
        if (!isPunching) {
            isPunching = true;
            punchingTimer = punchingDuration;
            cout << "Knuckles is now a bodmosh!" << endl;
        }
    }

    void update(char** grid, const int cell_size) override {
        Player::update(grid, cell_size);

        if (isPunching) {
            punchingTimer -= 0.016f;
            if (punchingTimer <= 0) {
                isPunching = false;
                cout << "Knuckles' punching ended" << endl;
            }
        }
    }

};


class PlayerFactory {

private:
    const int NUM_PLAYERS = 3;
    Player* players[3];
    int activePlayerIndex;


public:

    PlayerFactory() : activePlayerIndex(0) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players[i] = nullptr;
        }

        createAllPlayers();
    }

    virtual ~PlayerFactory() {
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


    void draw(RenderWindow& window, float offset) const {

        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->draw(window, players[i]->getLastDirection(), offset);
            }
        }
        players[activePlayerIndex]->draw(window, players[activePlayerIndex]->getLastDirection(),offset);
    }

    void update(char** grid, const int cell_size) const {
        //cout << "\n--- Frame update ---" << endl;

        // First, handle active player movement
        Player* activePlayer = players[activePlayerIndex];

        // Debug active player state
       /* cout << "Active player position: (" << activePlayer->getPosition()[0]
            << ", " << activePlayer->getPosition()[1]
            << "), velocity: (" << activePlayer->getVelocityX()
            << ", " << activePlayer->getVelocityY()
            << "), onGround: " << (activePlayer->getIsOnGround() ? "yes" : "no") << endl;*/

        activePlayer->update(grid, cell_size);

        // Update followers
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (i != activePlayerIndex) {
                players[i]->followLeader();
                players[i]->update(grid, cell_size);
            }
        }
        //cout << "--- End frame update ---\n" << endl;
    }

};