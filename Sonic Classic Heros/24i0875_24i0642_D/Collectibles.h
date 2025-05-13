
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Animations.h"
#include "Hitbox.h"
#include "Character.h"


using namespace std;
using namespace sf;


class Collectibles : public GameEntity {
protected:
    //===========================================================================
    // COLLECTIBLE PROPERTIES
    //===========================================================================
    Texture collectibleTexture;
    Sprite collectibleSprite;  // 32 x 32 px

    float boxlen;

    Texture iconTexture; // 16 x 16 px (for boost/extra life)
    Sprite iconSprite;

    bool isCollected;  // Flag to track if this collectible has been collected

    SoundBuffer sound;
    Sound ring;


public:
    //===========================================================================
    // CONSTRUCTOR & DESTRUCTOR
    //===========================================================================
    Collectibles() : GameEntity(), isCollected(false), boxlen(32) {
        position[0] = 0;
        position[1] = 0;

        sound.loadFromFile("Data/sounds/undertale.wav");

        ring.setBuffer(sound);
        ring.setPlayingOffset(seconds(0.0f));
        ring.setVolume(100);

    }

    virtual ~Collectibles() {}

    //===========================================================================
    // GAME ENTITY OVERRIDE METHODS
    //===========================================================================
    void update(char** grid, const int cell_size) override {};

    float* getPosition() override { return position; }

    void setPosition(float x, float y) override {}

    void draw(RenderWindow& window, float directionFaced, float offsetX) override {}

    //===========================================================================
    // COLLECTIBLE METHODS
    //===========================================================================
    virtual void collect() {
        ring.play();
        isCollected = true;
    }

    virtual void update(float deltaTime) {}

    //===========================================================================
    // HITBOX & RENDERING METHODS
    //===========================================================================
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
        if (!isCollected) {
            collectibleSprite.setPosition(position[0] - offset, position[1]);
            window.draw(collectibleSprite);

            if (iconSprite.getTexture()) {
                iconSprite.setPosition(position[0] - offset + (boxlen - 16) / 2,
                    position[1] + (boxlen - 16) / 2);
                window.draw(iconSprite);
            }
        }
    }

    //===========================================================================
    // GETTERS & SETTERS
    //===========================================================================
    Hitbox& getHitbox() {
        return hitbox;
    }

    void setPosition(int xPos, int yPos) {
        position[0] = xPos;
        position[1] = yPos;
    }

    bool getIsCollected() const { return isCollected; }
};

//===========================================================================
// RINGS CLASS IMPLEMENTATION
//===========================================================================
class Rings : public Collectibles {
private:
    //===========================================================================
    // RINGS-SPECIFIC PROPERTIES
    //===========================================================================
    Animation collect;
    Texture text;

    

public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Rings(float x, float y) {
        position[0] = x;
        position[1] = y;

        text.loadFromFile("Data/rings.png");

        collect = Animation(text, 128, 32, 4, 0.05f);

        hitbox = Hitbox(x, y, 32, 32);

       

    }

    //===========================================================================
    // RENDERING METHODS
    //===========================================================================
    void render(RenderWindow& window, float offset) override {
        if (!isCollected) {
            collect.setPosition(position[0] - offset, position[1]);
            collect.draw(window);
            //drawHitbox(window, offset);
            collect.update(0.025f); // Update the animation
        }
    }

};

//===========================================================================
// BOOSTS CLASS IMPLEMENTATION
//===========================================================================
class Boosts : public Collectibles {
public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    Boosts(float x, float y) {
        position[0] = x;
        position[1] = y;

        collectibleTexture.loadFromFile("Data/Boost_Box.png");
        collectibleSprite.setTexture(collectibleTexture);

        iconTexture.loadFromFile("Data/boost.png");
        iconSprite.setTexture(iconTexture);

        hitbox = Hitbox(x, y, 32, 32);
    }
};

//===========================================================================
// EXTRA LIFE CLASS IMPLEMENTATION
//===========================================================================
class ExtraLife : public Collectibles {
public:
    //===========================================================================
    // CONSTRUCTOR
    //===========================================================================
    ExtraLife(float x, float y) {
        position[0] = x;
        position[1] = y;

        collectibleTexture.loadFromFile("Data/Boost_Box.png");
        collectibleSprite.setTexture(collectibleTexture);

        iconTexture.loadFromFile("Data/boost.png");
        iconSprite.setTexture(iconTexture);

        hitbox = Hitbox(x, y, 32, 32);
    }
};

//===========================================================================
// COLLECTIBLE FACTORY CLASS IMPLEMENTATION
//===========================================================================
class CollectibleFactory {
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
    CollectibleFactory(int height, int width, int lvl) : gridHeight(height), gridWidth(width), levelNum(lvl) {}

    ~CollectibleFactory() {}

    //===========================================================================
    // COLLECTIBLE CREATION METHODS
    //===========================================================================
    // Create a single collectible
    Collectibles* createCollectible(const char& type, float spawnX, float spawnY) {
        Collectibles* collectible = nullptr;
        switch (type) {
        case 'r': collectible = new Rings(spawnX, spawnY); break;
        case 'b': collectible = new Boosts(spawnX, spawnY); break;
        case 'l': collectible = new ExtraLife(spawnX, spawnY); break;
        }

        if (collectible) {
            collectible->setPosition(spawnX, spawnY);
        }

        return collectible;
    }

    //===========================================================================
    // UPDATE METHODS
    //===========================================================================
    // Update an array of collectibles
    void updateCollectibles(RenderWindow& window, Collectibles** collectibles, int collectibleCount,
        float offsetX, float deltaTime, Player* player) {

        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i] != nullptr) {
                collectibles[i]->update(deltaTime);
                collectibles[i]->render(window, offsetX);

                if (collectibles[i]->getHitbox().checkCollision(player->getHitbox())) {
                    collectibles[i]->collect();
                    delete collectibles[i];
                    collectibles[i] = nullptr;
                }
            }
        }
    }
};
