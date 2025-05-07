#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "Character.h"
#include "Enemies.h"
#include "Collectibles.h"
#include "Obstacles.h"
#include "Level.h"
#include "Menu.h"
#include "Audio.h"

using namespace std;
using namespace sf;

class Game {

    float screen_x = 1200;
    float screen_y = 900;

    RenderWindow window;

    Menu menu;

    MusicManager gameMusic;


    bool isMenu;
    bool isPlaying;
    bool isPaused;
    bool isExiting;
    bool isInOptions;
    bool isLeaderboard;


   // Level levels[4];

    Leaderboard leaderboard;
    SaveData save;
    ScoreManager sm;
    string Playername;


public:
    Game() : window(VideoMode(screen_x, screen_y), "Sonic Classic Heroes", Style::Close), 
        menu(screen_x, screen_y),
        gameMusic(screen_x, screen_y),
        leaderboard(screen_x, screen_y) 
    {


        isMenu = true;
        isPlaying = false;
        isPaused = false;
        isExiting = false;
        isInOptions = false;
        isLeaderboard = false;
    }

    // Main game loop
    void run() {
        Event ev;
        while (window.isOpen()) {
            while (window.pollEvent(ev)) {
                if (ev.type == Event::Closed) {
                    window.close();
                }

                handleMenuInput(ev);
                ViewLeaderboard(ev);
                handleOptionsInput(ev);
                handleGameInput(ev);
                handlePauseInput(ev);

            }

            updateGame();
            render();
        }
    }

private:
    // Handle input in the main menu
    void handleMenuInput(Event& ev) {
        if (isMenu && !isInOptions && !isLeaderboard) {
            if (ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Up) {
                    menu.moveup();
                }
                if (ev.key.code == Keyboard::Down) {
                    menu.movedown();
                }
                if (ev.key.code == Keyboard::Enter) {
                    handleMenuSelection(menu.getSelectedOption());
                }
            }
        }
    }

    // View the leaderboard
    void ViewLeaderboard(Event& ev) {
        if (isLeaderboard) {

            leaderboard.draw(window);

            if (ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Escape) {
                    isLeaderboard = false;  // Return to the main menu
                    isMenu = true;
                }
            }
        }
    }



    // Handle input in the options menu
    void handleOptionsInput(Event& ev) {

        if (isInOptions) {

            gameMusic.handleInput(ev);

            if (ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Escape) {
                    isInOptions = false;  // Return to the main menu
                    isMenu = true;
                }
            }
        }
    }



/// CHARACTER CONTROLS

    void handleGameInput(Event& ev) {
        if (isPlaying && !isPaused) {
            if (ev.type == Event::KeyPressed) {
                if (ev.key.code == Keyboard::Escape) {
                    isPaused = true;  // Pause the game
                }
            }
        }
    }

    // Handle input while the game is paused
    void handlePauseInput(Event& ev) {

        if (isPaused) {

            if (ev.type == Event::KeyPressed) {

                if (ev.key.code == Keyboard::Enter) {
                    isPaused = false;  // Resume the game
                }
                else if (ev.key.code == Keyboard::Escape) {
                    isMenu = true;  // Return to the main menu
                    isPaused = false;  // Reset pause
                }
            }
        }
    }


    /// MENU INPUTS
    void handleMenuSelection(int selectedOption) {
        switch (selectedOption) {
        case 0:  // New Game
            StartGame();
            break;
        case 1:  // Continue
            ContinueGame();
            break;
        case 2:  // Leaderboard
            isLeaderboard = true;
            break;
        case 3:  // Options
            isInOptions = true; 
            break;
        case 4:  // Exit
            ExitGame();
            break;
        }
    }

    // Start the game
    void StartGame() {
        isPlaying = true;
        isPaused = false;
        isMenu = false;
    }

    // Continue a previously saved game
    void ContinueGame() {
        if (save.loadFromFile("saved.txt")) {
            save.printData();
        }
        else {
            cout << "No save file found.\n";
        }
        LoadGame();
    }

    // Load the game (for continuation or new game)
    void LoadGame() {
        isPlaying = true;
        isPaused = false;
        isMenu = false;
    }

   

    // Handle game state updates (e.g., gameplay updates, pauses, etc.)
    // Update game elements here (e.g., characters, level, etc.)

    void updateGame() {
        if (isPlaying && !isPaused) {
        
        
        
        }
    }

    // Render the screen based on the current game state
    void render() {

        window.clear();

        if (isMenu && !isInOptions && !isLeaderboard) { menu.draw(window); }

        else if (isInOptions) { gameMusic.drawMenu(window);}

        else if (isLeaderboard) { leaderboard.draw(window); }

        else if (isPlaying && !isPaused) {
            // Draw the gameplay here
        }

        else if (isPaused) {
            // Draw paused menu here
        }

        window.display();
    }

    // Exit the game
    void ExitGame() {
        isExiting = true;
        window.close();
    }

    string getPlayerName() { return Playername; }
    void setPlayerName(string name) { Playername = name; }

    // SET NAME AT THE START OF THE GAME

};
