#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Hitbox.h"




class GameEntity {

protected:

	//RenderWindow& window;
	float position[2];
	char symbol;
	Hitbox hitbox;

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


	//virtual void draw(RenderWindow& window, float directionFaced = 0.0f, float offset = 0) = 0;
	virtual void update(char** grid, const int cell_size = 64) = 0;

	virtual bool checkCollision(const GameEntity& other) const {
		return hitbox.checkCollision(other.hitbox);
	}

	char getSymbol() const { return symbol; }
	void setSymbol(char s) { this->symbol = s; }
	virtual float* getPosition() { return position; }
	virtual void setPosition(float x, float y) {
		position[0] = x;  position[1] = y;
	}
};
