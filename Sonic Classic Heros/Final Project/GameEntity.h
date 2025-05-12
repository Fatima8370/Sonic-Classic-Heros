#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Hitbox.h"
#include "Character.h"

//===========================================================================
// GAME ENTITY CLASS DECLARATION
//===========================================================================
class GameEntity {
protected:
    float position[2];
    char symbol;
    Hitbox hitbox;
    int lvlNum;
    const int cell_size = 64;
    bool isActive;

    sf::Texture texture;
    sf::Sprite sprite;

public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    GameEntity(float x = 0.0f, float y = 0.0f, char s = ' ') :
        symbol(s),
        lvlNum(1),
        isActive(true)
    {
        position[0] = x;
        position[1] = y;
        hitbox = Hitbox(x, y, 64, 64);
    }

    virtual ~GameEntity() {}

    //===========================================================================
    // PURE VIRTUAL METHODS
    //===========================================================================
    virtual void draw(sf::RenderWindow& window, float directionFaced, float offsetX) = 0;
    virtual void update(char** grid, const int cell_size = 64) = 0;
    virtual void render(sf::RenderWindow& window, float offsetX) = 0;

    //===========================================================================
    // COLLISION METHODS
    //===========================================================================
    virtual bool checkCollision(const GameEntity& other) const {
        return hitbox.checkCollision(other.hitbox);
    }

    virtual void handleCollision(GameEntity* other) {
    }

    //===========================================================================
    // GETTERS
    //===========================================================================
    char getSymbol() const { return symbol; }
    int getLevelNum() const { return lvlNum; }
    bool getIsActive() const { return isActive; }
    float getX() const { return position[0]; }
    float getY() const { return position[1]; }
    virtual float* getPosition() { return position; }
    Hitbox& getHitbox() { return hitbox; }
    const Hitbox& getHitbox() const { return hitbox; }
    Sprite& getSprite() { return sprite; }
    const Sprite& getSprite() const { return sprite; }

    //===========================================================================
    // SETTERS
    //===========================================================================
    void setSymbol(char s) { this->symbol = s; }
    void setLevelNum(int num) { lvlNum = num; }
    void setIsActive(bool active) { isActive = active; }

    virtual void setPosition(float x, float y) {
        position[0] = x;
        position[1] = y;
        hitbox.updateHitbox(x, y);
        if (sprite.getTexture()) {
            sprite.setPosition(x, y);
        }
    }

    void setHitboxSize(float width, float height) {
        hitbox.updateHitbox(width, height);
    }

    //===========================================================================
    // TEXTURE METHODS
    //===========================================================================
    bool loadTexture(const std::string& filename) {
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }
};