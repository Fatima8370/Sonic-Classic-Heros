#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Animations.h"
#include "Hitbox.h"
#include "Character.h"


using namespace std;
using namespace sf;


class Collectibles: public GameEntity {

protected:
    float x, y; // position
    Texture b;
    Sprite a;  // 32 x 32 px

    float boxlen;

    

    Texture life; // 16 x 16 px (for boost/extra life)
    Sprite stick;

    bool isCollected;  // Flag to track if this collectible has been collected

public:
    Collectibles() : isCollected(false), boxlen(32) { x = 0; y = 0; }

    virtual ~Collectibles() {}

    void update(char** grid, const int cell_size) override {};

    float* getPosition()override  { return position; }
    void setPosition(float x, float y) override {}

    void draw(RenderWindow& window, float directionFaced, float offsetX) override {}

    virtual void collect() {
        isCollected = true;
    }

    virtual void update(float deltaTime) {}

	void setCollected(bool collected) {
		isCollected = collected;
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

    virtual void render(RenderWindow& window, float offset) override{

        if (!isCollected) {
            // render the main collectible sprite (base sprite)
            a.setPosition(x - offset, y);
            window.draw(a);

            // If the collectible is a boost/extra life,render the small icon on top
            if (stick.getTexture()) {
                stick.setPosition(x - offset + (boxlen - 16) / 2,
                    y + (boxlen - 16) / 2);
                window.draw(stick);
            }
        }
    }

    Hitbox& getHitbox() {
        return hitbox;
    }

    void setPosition(int xPos, int yPos) {
        x = xPos;
        y = yPos;
    }


    bool getIsCollected() const { return isCollected; }
};

class Rings : public Collectibles {

    Animation collect;
    Texture text;

public:
    Rings(float x , float y) {

		this->x = x; this->y = y;

        text.loadFromFile("Data/rings.png");
         
		collect = Animation(text, 128, 32, 4, 0.1f);

		hitbox = Hitbox(x, y, 32, 32);
    }

    void render(RenderWindow& window, float offset) override {
        if (!isCollected) {
			collect.setPosition(x - offset, y);
            collect.draw(window);
            drawHitbox(window, offset);
        }
    }
};

class Boosts : public Collectibles {
public:
    Boosts(float x, float y) {

        this->x = x; this->y = y;

        b.loadFromFile("Data/Boost_Box.png");
        a.setTexture(b);

        life.loadFromFile("Data/boost.png");
        stick.setTexture(life);

		hitbox = Hitbox(x, y, 32, 32);
    }
};

class ExtraLife : public Collectibles {
public:
    ExtraLife(float x, float y) {

        this->x = x; this->y = y;

        b.loadFromFile("Data/Boost_Box.png");
        
        a.setTexture(b);

        life.loadFromFile("Data/boost.png");
        stick.setTexture(life);

		hitbox = Hitbox(x, y, 32, 32);
    }
};


class CollectibleFactory {
private:
    int gridHeight, gridWidth;
    int levelNum;

public:
    CollectibleFactory(int height, int width, int lvl ): gridHeight(height), gridWidth(width), levelNum(lvl){    }

    ~CollectibleFactory() {   }

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

    // Update an array of collectibles
    void updateCollectibles(RenderWindow& window, Collectibles** collectibles, int collectibleCount,
        float offsetX, float deltaTime, Player* player) {

        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i] != nullptr) {
               

                collectibles[i]->update(deltaTime);

                collectibles[i]->render(window, offsetX);


                if (collectibles[i]->getHitbox().checkCollision(player->getHitbox())) {
                    collectibles[i]->setCollected(true);

                    delete collectibles[i];
                    collectibles[i] = nullptr;
                }
                
               
            }
        }

    }
};



