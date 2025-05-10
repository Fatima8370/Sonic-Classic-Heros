#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Animations.h"
#include "GameEntity.h"

using namespace std;

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

    virtual void render(sf::RenderWindow& window, float offset) override{

        if (!isCollected) {
            // render the main collectible sprite (base sprite)
            a.setPosition(x - offset, y);
            window.draw(a);

            drawHitbox(window, offset);

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
    Rings() {
        b.loadFromFile("Data/rings.png");
         
		collect = Animation(b, 128, 32, 4, 0.1f);

		hitbox = Hitbox(x, y, 32, 32);
    }

    void render(sf::RenderWindow& window, float offset) override {
        if (!isCollected) {
            collect.draw(window);
            drawHitbox(window, offset);
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

		hitbox = Hitbox(x, y, 32, 32);
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

		hitbox = Hitbox(x, y, 32, 32);
    }
};


class CollectibleFactory {
private:
    static const int MAX_COLLECTIBLES = 100;
    Collectibles* collectibles[MAX_COLLECTIBLES];
    float usedPositions[MAX_COLLECTIBLES][2]; // Store x,y coordinates
    int count;
    int gridHeight, gridWidth;
    int levelNum;

public:
    CollectibleFactory(int collectibleCount, int height = 0, int width = 0, int lvl = 1)
        : count(0), gridHeight(height), gridWidth(width), levelNum(lvl)
    {
        for (int i = 0; i < MAX_COLLECTIBLES; ++i)
            collectibles[i] = nullptr;
        cout << "In Collectible Factory\n";
    }

    ~CollectibleFactory() {
        clearCollectibles();
    }

    Collectibles* createCollectible(const char& type, float spawnX, float spawnY) {
        Collectibles* collectible = nullptr;

        switch (type) {
        case 'R': collectible = new Rings(); break;
        case 'B': collectible = new Boosts(); break;
        case 'L': collectible = new ExtraLife(); break;
            // Add other collectible types as needed
        }

        if (collectible) {
            collectible->setPosition(spawnX, spawnY);
            cout << "Created " << type << " at (" << spawnX << ", " << spawnY << ")" << endl;

            // Store the position for the created collectible
            if (count < MAX_COLLECTIBLES) {
                collectibles[count] = collectible;
                usedPositions[count][0] = spawnX;
                usedPositions[count][1] = spawnY;
                count++;
            }
        }

        return collectible;
    }

    
    void updateCollectibles(RenderWindow& window, Collectibles* collectible[], int collectibleCount,
        float offsetX, float deltaTime, Player* player) {
        for (int i = 0; i < collectibleCount; i++) {
            if (collectible[i]) {
                // Update the collectible's state
                collectible[i]->update(deltaTime);

                //render with scrolling offset
                collectible[i]->render(window, offsetX);

                // Check if player collected this item
                if (collectible[i]->getHitbox().checkCollision(player->getHitbox())) {

                    collectible[i]->setCollected(true);

                    // Remove the collectible
                    delete collectible[i];
                    collectible[i] = nullptr;

                    // Compact the array (optional)
                    // This would require updating indices in the main game loop
                }
            }
        }
    }

    void clearCollectibles() {
        for (int i = 0; i < count; i++) {
            if (collectibles[i]) {
                delete collectibles[i];
                collectibles[i] = nullptr;
            }
        }
        count = 0;
        cout << "All collectibles cleared" << endl;
    }

    int getCount() const { return count; }
    Collectibles** getAllCollectibles() { return collectibles; }
};