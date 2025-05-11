#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Hitbox.h"
#include "Character.h"

// Forward declarations

class GameEntity {
protected:
    float position[2];        // Position [x, y]
    char symbol;              // Character representation in level
    Hitbox hitbox;            // Collision hitbox
    int lvlNum;               // Current level number
    const int cell_size = 64; // Size of grid cells
    bool isActive;            // Is entity active in the game

   

    // Texture and sprite for rendering
    sf::Texture texture;
    sf::Sprite sprite;

public:
    GameEntity(float x = 0.0f, float y = 0.0f, char s = ' ') :
        symbol(s),
        lvlNum(1),
        isActive(true)        
    {
        position[0] = x;
        position[1] = y;
        hitbox = Hitbox(x, y, 64, 64); // Default hitbox size
    }

    virtual ~GameEntity() {}

    // Pure virtual methods that must be implemented by derived classes
    virtual void draw(sf::RenderWindow& window, float directionFaced, float offsetX) = 0;
    virtual void update(char** grid, const int cell_size = 64 ) = 0;
    virtual void render(sf::RenderWindow& window, float offsetX) = 0;

    // Collision detection
    virtual bool checkCollision(const GameEntity& other) const {
        return hitbox.checkCollision(other.hitbox);
    }

    // Handle collision response - should be overridden by derived classes
    virtual void handleCollision(GameEntity* other) {
        // Default implementation does nothing
    }

    // Getters and setters
    char getSymbol() const { return symbol; }
    void setSymbol(char s) { this->symbol = s; }

    int getLevelNum() const { return lvlNum; }
    void setLevelNum(int num) { lvlNum = num; }

    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }

    float getX() const { return position[0]; }
    float getY() const { return position[1]; }

    virtual float* getPosition() { return position; }
    virtual void setPosition(float x, float y) {
        position[0] = x;
        position[1] = y;

        // Update hitbox position
        hitbox.updateHitbox(x, y);

        // Update sprite position if using SFML sprites
        if (sprite.getTexture()) {
            sprite.setPosition(x, y);
        }
    }

    // Set the hitbox size
    void setHitboxSize(float width, float height) {
        hitbox.updateHitbox(width, height);
    }

    // Get the hitbox
    Hitbox& getHitbox() { return hitbox; }
    const Hitbox& getHitbox() const { return hitbox; }

    // Load texture from file
    bool loadTexture(const std::string& filename) {
        if (!texture.loadFromFile(filename)) {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    // Set texture and update sprite
    void setTexture(const sf::Texture& tex) {
        texture = tex;
        sprite.setTexture(texture);
    }

    // Get sprite
    Sprite& getSprite() { return sprite; }
    const Sprite& getSprite() const { return sprite; }
};