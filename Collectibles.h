#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Animations.h"

using namespace std;

class Collectibles {
protected:
    int x, y; // position
    Texture b;
    Sprite a;  // 32 x 32 px

    float boxlen;

    Texture life; // 16 x 16 px (for boost/extra life)
    Sprite stick;

    bool isCollected;  // Flag to track if this collectible has been collected

public:
    Collectibles() : isCollected(false), boxlen(32) { x = 0; y = 0; }

    virtual void collect() {
        isCollected = true;
    }

    virtual void update(float deltaTime) {}

    virtual void draw(sf::RenderWindow& window, float offset) {
        if (!isCollected) {
            // Draw the main collectible sprite (base sprite)
            a.setPosition(x - offset, y);
            window.draw(a);

            // If the collectible is a boost/extra life, draw the small icon on top
            if (stick.getTexture()) {
                stick.setPosition(x - offset + (boxlen - stick.getLocalBounds().width) / 2,
                    y + (boxlen - stick.getLocalBounds().height) / 2);
                window.draw(stick);
            }
        }
    }

    void setPosition(int xPos, int yPos) {
        x = xPos;
        y = yPos;
    }


    bool getIsCollected() const { return isCollected; }
};

class Rings : public Collectibles {
public:
    Rings() {
        if (!b.loadFromFile("Data/ring.png")) {
            cerr << "Error loading texture!" << endl;
        }
        a.setTexture(b);
    }

    void draw(sf::RenderWindow& window, float offset) override {
        if (!isCollected) {
            a.setPosition(x - offset, y);
            window.draw(a);  // Only draw the ring sprite
        }
    }
};

class Boosts : public Collectibles {
public:
    Boosts() {
        if (!b.loadFromFile("Data/Boost_Box.png")) {
            cerr << "Error loading texture!" << endl;
        }
        a.setTexture(b);

        if (!life.loadFromFile("boost.png")) {
            cerr << "Error loading boost icon!" << endl;
        }
        stick.setTexture(life);
    }
};

class ExtraLife : public Collectibles {
public:
    ExtraLife() {
        if (!b.loadFromFile("Data/Boost_Box.png")) {
            cerr << "Error loading texture!" << endl;
        }
        a.setTexture(b);

        if (!life.loadFromFile("boost.png")) {
            cerr << "Error loading extra life icon!" << endl;
        }
        stick.setTexture(life);
    }
};


class CollectibleFactory {
private:
    Collectibles** collectibles;   // Dynamic array of pointers
    int capacity;                  // Max number of collectibles
    int count;                     // Current number of collectibles

public:
    CollectibleFactory(int maxCollectibles = 100) {
        capacity = maxCollectibles;
        count = 0;
        collectibles = new Collectibles * [capacity];
        for (int i = 0; i < capacity; ++i)
            collectibles[i] = nullptr;
    }

    ~CollectibleFactory() {
        for (int i = 0; i < count; ++i) {
            delete collectibles[i];
            collectibles[i] = nullptr;
        }
        delete[] collectibles;
    }

    // Create a Ring
    void createRing(int x, int y) {
        if (count < capacity) {
            Rings* ring = new Rings();
            ring->setPosition(x, y);
            collectibles[count++] = ring;
        }
    }

    // Create a Boost
    void createBoost(int x, int y) {
        if (count < capacity) {
            Boosts* boost = new Boosts();
            boost->setPosition(x, y);
            collectibles[count++] = boost;
        }
    }

    // Create an Extra Life
    void createExtraLife(int x, int y) {
        if (count < capacity) {
            ExtraLife* life = new ExtraLife();
            life->setPosition(x, y);
            collectibles[count++] = life;
        }
    }

    void updateAll(float deltaTime) {
        for (int i = 0; i < count; ++i) {
            collectibles[i]->update(deltaTime);
        }
    }

    void drawAll(sf::RenderWindow& window, float offset) {
        for (int i = 0; i < count; ++i) {
            collectibles[i]->draw(window, offset);
        }
    }

    Collectibles** getAllCollectibles() {
        return collectibles;
    }

    int getCount() const {
        return count;
    }
};
