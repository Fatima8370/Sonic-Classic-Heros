#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Hitbox.h"
#include "Enemies.h"
#include "Obstacles.h"
#include "Collectibles.h"




class GameEntity {

protected:

	float position[2];
	char symbol;
	Hitbox hitbox; // everything should have a hitbox

	int currentLevel;
	int score;

	const int maxLevels = 4; // might be used in level class

	const int maxEnemies = 50; // might be used in level class

	char** levelGrid; // use levelgenerator , might use this specifically in level class, cuz different levels

	float levelWidth, levelHeight;

	float offsetX; // to implement horizontal scrolling

	Time time; // to store time taken to play single level, might put it in the level class

	ObstacleFactory obstacleFactory; // to create obstacles in the level

	EnemyFactory enemiesFactory; // to create enemies in the level
	
	PlayerFactory playerFactory; // to create players in the level

	CollectibleFactory collectibleFactory; // to create collectibles in the level

	Player* player; // to create player in the level

	Enemies** enemies; // to create enemies in the level

	Obstacles** obstacles; // to create obstacles in the level

	Collectibles** collectibles; // to create collectibles in the level



public:
	GameEntity(float x = 0.0f, float y = 0.0f, char s = ' ') : symbol(s) {
		position[0] = 0.0f;
		position[1] = 0.0f;

		hitbox = Hitbox(x, y, 0, 0);
		cout << "Game Entity" << endl;

	}

	virtual ~GameEntity() {
		cout << "~Game Entity" << endl;
	}




	virtual void draw(RenderWindow& window, float directionFaced = 0.0f, float offset = 0) = 0;
	virtual void update(char** grid, const int cell_size = 64) = 0;

	virtual void checkCollision(Player* player) = 0;	

	virtual void checkCollision(Enemies* enemy) = 0;

	virtual void checkCollision(Obstacles* obstacle) = 0;

	virtual void checkCollision(Collectibles* collectible) = 0;

	virtual void checkCollision(Enemies* enemy, const Hitbox& playerHitbox) = 0;

	virtual void checkCollision(Obstacles* obstacle, const Hitbox& playerHitbox) = 0;

	virtual void checkCollision(Collectibles* collectible, const Hitbox& playerHitbox) = 0;

	virtual void checkCollision(Enemies* enemy, const Hitbox& playerHitbox) = 0;

	virtual bool checkCollision(const GameEntity& other) const {
		return hitbox.checkCollision(other.hitbox);
	}

	char getSymbol() const { return symbol; }

	Hitbox getHitbox() const { return hitbox; }

	Hitbox& getHitbox() { return hitbox; }

	float getX() const { return position[0]; }

	float getY() const { return position[1]; }

	float getWidth() const { return hitbox.getWidth(); }

	float getHeight() const { return hitbox.getHeight(); }

	void setSymbol(char s) { this->symbol = s; }

	virtual float* getPosition() { return position; }

	virtual void setPosition(float x, float y) {
		position[0] = x;  position[1] = y;
	}
};
