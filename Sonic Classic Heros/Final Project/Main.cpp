#include <iostream>
#include <fstream>
#include <cmath>


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "Character.h"
#include "BuildLevel.h"


using namespace sf;
using namespace std;

float screen_x = 1280;
float screen_y = 896;

int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "Sonic the Hedgehog-OOP", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    const int cell_size = 64;
    const int height = 14;
    const int width = 200;

    Texture bg;
    Sprite Sbg;

    bg.loadFromFile("Data/BG.png");
    Sbg.setTexture(bg);

    float offsetX = 0;

    PlayerFactory factory(width);


    for (int i = 0; i < 3; i++) {
        Player* player = factory.getPlayer(i);
        if (player) {

            player->updateHitbox();
        }
    }

    Player* activePlayer = factory.getActivePlayer();

    BuildLevel level;
    level.loadLevel(2);

    Clock clock;

    Event ev;
    while (window.isOpen())
    {
        Time deltaTime = clock.restart();

        while (window.pollEvent(ev)) {
            switch (ev.type) {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:

                if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Z)
                {
                    factory.switchActivePlayer();
                    activePlayer = factory.getActivePlayer();
                }

                if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::X)
                {
                    activePlayer->activateSpecialAbility();

                    if (factory.getActivePlayerIndex() == 1) {
                        factory.getActivePlayer()->applySpecialAbilityEffect(ev);
                    }
                }

                break;

            default:
                break;
            }
        }

        float directionX = 0.0f;
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            directionX = 1.0f;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left)) {
            directionX = -1.0f;
        }
        if (directionX != 0.0f) {
            activePlayer->move(directionX, width);
        }

        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            activePlayer->jump();
        }


        if (activePlayer->getX() >= screen_x / 2  && activePlayer->getX() <= width * 64 - screen_x / 2) 
        {
            offsetX = activePlayer->getX() - screen_x / 2  ;  // Camera follows player
        }
		else if (activePlayer->getX() >= width * 64 - screen_x / 2)
		{
			offsetX = width * 64 - screen_x;  // Keep the player at the right edge
		}
        else {
			offsetX = 0;  // Keep the player at the left if they are close to the left edge
        }
		

        window.clear();


		window.draw(Sbg);

        level.update(window, offsetX, deltaTime.asSeconds(), activePlayer);

        factory.draw(window, directionX ,offsetX); 

        factory.update(level.getObstacleLayout(), 64);

		


        window.display();
    }




    return 0;
}

