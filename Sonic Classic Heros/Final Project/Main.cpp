#include <iostream>
#include <fstream>
#include <cmath>


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>


#include "Enemies.h"
#include "Hitbox.h"
#include "Level.h"
#include "Obstacles.h"
#include "BuildLevel.h"
#include "Character.h"

using namespace sf;
using namespace std;

float screen_x = 1200;
float screen_y = 900;

// prototypes 




void generateLevel(char** lvl, int height, int width);
void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, Sprite& wallSprite2, const int cell_size, float offsetX);

int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "Sonic the Hedgehog-OOP", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    const int cell_size = 64;
    const int height = 14;
    const int width = 200;

    char** lvl = NULL;

    Texture wallTex1;
    Sprite wallSprite1;

    Texture wallTex2;
    Sprite wallSprite2;

    // Initialize level
    lvl = new char* [height];
    for (int i = 0; i < height; i += 1)
    {
        lvl[i] = new char[width] {'\0'};
    }

    // Load textures for walls
    wallTex1.loadFromFile("Data/brick1.png");
    wallSprite1.setTexture(wallTex1);

    wallTex2.loadFromFile("Data/brick2.png");
    wallSprite2.setTexture(wallTex2);



    float scale_x = 2.5;
    float scale_y = 2.5;

    int raw_img_x = 24;
    int raw_img_y = 35;

    int Pheight = raw_img_y * scale_y;
    int Pwidth = raw_img_x * scale_x;

    int hit_box_factor_x = 8 * scale_x;
    int hit_box_factor_y = 5 * scale_y;




    Texture bg;
    Sprite Sbg;

    bg.loadFromFile("Data/background25.png");
    Sbg.setTexture(bg);

    Clock clock;

    // Initial offset
    float offsetX = 0;




    // TESTING OBSTACLES

	generateLevel(lvl, height, width);



    //////////////// PLAYER SHIT ////////////////////


    // Create the player factory and characters
    PlayerFactory factory(width);

    // Update/initialize all players
    for (int i = 0; i < 3; i++) {
        Player* player = factory.getPlayer(i);
        if (player) {
            // Make sure hitboxes are initialized
            player->updateHitbox();
        }
    }

    // Main character is active player
    Player* activePlayer = factory.getActivePlayer();

    // ////////// PLAYER SHIT ///////////////////////

    // //generateLevel(lvl, 14, 200);

    // const int TILE_SIZE = 64;
    // const int GRID_WIDTH = 200;
    // const int GRID_HEIGHT = 14;
    // int currentLevel = 1;

    // 

    // char** grid = LevelGenerator::generateLevel(currentLevel);

    // ObstacleFactory obs;

    // Obstacles** level1;
    // 
    //////////////////////// // ENEMY SHIT//

    // 
    // EnemyFactory EM(grid, GRID_HEIGHT, GRID_WIDTH, currentLevel);

    // const char* enemyTypes[] = { "batbrain", "beebot", "motobug", "crabmeat" };
    // const int numTypes = 4;

    // Enemies* enemy[60];  
    // int count = 0;       

    // for (int j = 0; j < numTypes; ++j) {
    //     for (int i = 0; i < 5; ++i) {  // Try creating 5 of each type
    //         Enemies* e = EM.createWithSpawn(enemyTypes[j]);
    //         if (e) {
    //             cout << "Spawned enemy of type: " << enemyTypes[j] << endl;
    //             enemy[count++] = e;
    //         }
    //     }
    // }




   // ENEMY SHIT ////////////////////////////////


     ///////////////////////////////////////////////////////////////////////////////////



    char position[height][width];

    char obstacles[height][width] = {};

    char collectibles[height][width] = {};

    char enemies[height][width] = {};


    int enemyCount = 0;
    int collectibleCount = 0;
    int obstacleCount = 0;


    ifstream file("Data/Level/level1.csv");

    if (!file) {
        cerr << "Failed to open file.\n";
        return 1;
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            char ch;
            file.get(ch);

            if (file.eof()) {
                cerr << "Unexpected end of file.\n";
                return 1;
            }

            position[i][j] = ch;

            if (ch == ' ') {
                continue; // ignore spaces
            }
            else if (ch >= 'A' && ch <= 'Z') {
                obstacles[i][j] = ch;
                obstacleCount++;
            }
            else if (ch >= 'a' && ch <= 'z') {
                collectibles[i][j] = ch;
                collectibleCount++;
            }
            else if (ch >= '0' && ch <= '9') {
                enemies[i][j] = ch;
                enemyCount++;
            }
        }
        file.ignore(1); // skip newline
    }


    file.close();


    //CollectibleFactory CF(14, 200, 1);

    ObstacleFactory OF(obstacleCount);

    Obstacles** obstacle;
    obstacle = new Obstacles * [obstacleCount];
    int count = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (obstacles[i][j] != '\0') {
                Obstacles* o = OF.createObstacle(obstacles[i][j], j * 64, i * 64);
                if (o) {
                    obstacle[count++] = o;
                    cout << "Spawned obstacle of type: " << obstacles[i][j] << endl;
                }
            }
        }
    }
    //delete[] obstacle;


    EnemyFactory EM(14, 200, 1);

    Enemies** enemy;
    enemy = new Enemies * [enemyCount];
    count = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (enemies[i][j] != '\0') {
                Enemies* e = EM.createEnemy(enemies[i][j], j * 64, i * 64);
                if (e) {
                    enemy[count++] = e;
                    cout << "Spawned enemy of type: " << enemies[i][j] << endl;
                }
            }
        }
    }
    //delete[] enemy;

///////////////////////////////////////////////////////////////////////////////////


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
                // Handle character switching with Z key
                if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Z)
                {
                    factory.switchActivePlayer();
                    activePlayer = factory.getActivePlayer();
                }



                // Handle special ability with X key
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


        // Handle jumping
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            activePlayer->jump();
        }


        if (activePlayer->getX() > screen_x / 2 && 
            activePlayer->getX() < width * 64 - screen_x / 2) 
        {
            offsetX = activePlayer->getX() - screen_x / 2;  // Camera follows player
        }
        else if (activePlayer->getX() > width * 64 - screen_x / 2) {
            offsetX = 0;  // Keep the player at the left if they are close to the left edge
        }
        else {
			offsetX = 0;  // Keep the player at the left if they are close to the left edge
        }
		

        window.clear();

        //EM.updateEnemies(window, enemy, enemyCount, offsetX, deltaTime.asSeconds(), activePlayer);

		OF.update(window, activePlayer, offsetX, obstacle); // displaying and u pdating obstacles

        factory.draw(window, directionX ,offsetX);

        factory.update(lvl, 64);



		// display_level(window, height, width, lvl, wallSprite1, wallSprite2, cell_size, offsetX);


        window.display();
    }



    return 0;
}

///// functions /////


void generateLevel(char** lvl, int height, int width) {
    // Clear level
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            lvl[i][j] = ' ';

    // Ground — fill entire bottom row
    for (int j = 0; j < width; j++)
        lvl[height - 1][j] = 'w';

    // Platform parameters 
    int platformCount = (width - 20) / 20;  // adjusted for margin
    int platformHeight = height - 4; // a few tiles above ground

    for (int p = 0; p < platformCount; p++) {
        int x = 10 + rand() % (width - 20 - 3);  // leave 10 columns on each side
        int y = platformHeight - (rand() % 3);  // slightly random height

        // Place 3-tile platform
        for (int i = 0; i < 3; i++)
            lvl[y][x + i] = 'w';

        // Maybe place a wall on platform
        if (rand() % 2 == 0 && y > 0)
            lvl[y - 1][x + 1] = 'w';
    }

    // Optional: random walls on ground (but skip first and last 10 columns)
   /* for (int i = 8; i < width - 10; i += 7) {
        if (rand() % 2 == 0)
            lvl[height - 2][i] = 'w';
    }*/


}


void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, Sprite& wallSprite2, const int cell_size, float offsetX)
{
    // Only render the portion of the level that is visible
    int startX = max(0, static_cast<int>(offsetX / cell_size));  // Convert offset to tile coordinates
    int endX = min(width, static_cast<int>((offsetX + screen_x) / cell_size));

    for (int i = 0; i < height; i++)
    {
        for (int j = startX; j < endX; j++)
        {
            if (lvl[i][j] == 'w')
            {
                wallSprite1.setPosition((j * cell_size) - offsetX, i * cell_size);  // Offset for horizontal scrolling
                window.draw(wallSprite1);
            }

            if (lvl[i][j] == 'p')
            {
                wallSprite2.setPosition((j * cell_size) - offsetX, i * cell_size);  // Offset for horizontal scrolling
                window.draw(wallSprite2);
            }
        }
    }
}