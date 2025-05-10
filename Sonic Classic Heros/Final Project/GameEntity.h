#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Hitbox.h"




class GameEntity {

protected:
	float position[2];
	char symbol;
	Hitbox hitbox;


	const int cell_size = 64;

public:
	GameEntity(float x = 0.0f, float y = 0.0f, char s = ' ') : symbol(s) {
		position[0] = 64*20;
		position[1] = 64*7;

		hitbox = Hitbox(x, y, 0, 0);
	}

	virtual ~GameEntity() {	}



	//virtual -> virtual override -> override 
	virtual void draw(RenderWindow& window, float directionFaced , float offsetX ) = 0;


	// add position[0] - offset
	virtual void update(char** grid, const int cell_size) = 0{};

	virtual bool checkCollision(const GameEntity& other) const {
		return hitbox.checkCollision(other.hitbox);
	}

	char getSymbol() const { return symbol; }
	void setSymbol(char s) { this->symbol = s; }

	virtual void render(RenderWindow& window, float offset) = 0;

	virtual float* getPosition() { return position; }
	virtual void setPosition(float x, float y) {
		position[0] = x;  position[1] = y;
	}
};
