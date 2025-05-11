#pragma once
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;



// 1- Leaderboard
// 2- MENU
// 3- Score Manager
// 4- Save data


class Leaderboard {
    float x, y;

    static const int Max_Players = 10;
    string names[Max_Players];
    int scores[Max_Players];
    int entryCount;
    Font font;

    Texture backgroundTexture;
    Sprite backgroundSprite;

    RectangleShape whiteOverlay;

public:
    Leaderboard( float x, float y) :x(x), y(y), entryCount(0) {
        if (!font.loadFromFile("OOP-Final/SyneMono-Regular.ttf")) {
            cerr << "Font loading failed\n";
        }
        if (!backgroundTexture.loadFromFile("Data/background.jpg")) {
            cerr << "Background image not found!\n";
        }
        else {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setScale(0.2, 0.2);

            whiteOverlay.setSize(Vector2f(x,y)); // not use vector
			whiteOverlay.setPosition(0, 0);
            whiteOverlay.setFillColor(Color(255, 255, 255, 100)); 

        }
    }

    void addScore(const string& name, int score) {
        if (entryCount < Max_Players) {
            names[entryCount] = name;
            scores[entryCount] = score;
            entryCount++;
        }
    }

    bool saveLeaderboard() {
        ofstream out("scores.txt");
        if (!out) return false;

        for (int i = 0; i < entryCount; ++i) {
            out << names[i] << " " << scores[i] << "\n";
        }

        out.close();
        return true;
    }

    bool loadLeaderboard() {
        ifstream in("scores.txt");
        if (!in) return false;

        entryCount = 0;
        while (in >> names[entryCount] >> scores[entryCount]) {
            entryCount++;
            if (entryCount >= Max_Players) break;
        }

        in.close();
        return true;
    }

    int calculateTopScore() {
        int maxScore = 0;
        for (int i = 0; i < entryCount; ++i) {
            if (scores[i] > maxScore) {
                maxScore = scores[i];
            }
        }
        return maxScore;
    }

    void sort() {

        for (int i = 0; i < entryCount - 1; ++i) {
            for (int j = 0; j < entryCount - i - 1; ++j) {
                if (scores[j] < scores[j + 1]) {

                    int tempScore = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = tempScore;

                    string tempName = names[j];
                    names[j] = names[j + 1];
                    names[j + 1] = tempName;
                }
            }
        }

    }

    void draw(RenderWindow& window) {
        sort();

        window.draw(backgroundSprite);
        window.draw(whiteOverlay);


        Text title;
        title.setFont(font);
        title.setCharacterSize(40);
        title.setFillColor(Color::Yellow);
        title.setString("Leaderboard");
        title.setPosition(x / 2 - 120, 40);
        window.draw(title);

        Text header;
        header.setFont(font);
        header.setCharacterSize(28);
        header.setFillColor(Color::Blue);

        header.setString("Name");
        header.setPosition(250, 100);
        window.draw(header);

        header.setString("Score");
        header.setPosition(850, 100); 
        window.draw(header);

        Text entry;
        entry.setFont(font);
        entry.setCharacterSize(24);
        entry.setFillColor(Color::Black);

        for (int i = 0; i < entryCount && i < 10; ++i) {
            // Name
            entry.setString(names[i]);
            entry.setPosition(100, 140 + i * 35);
            window.draw(entry);

            // Score
            entry.setString(to_string(scores[i]));
            entry.setPosition(400, 140 + i * 35);
            window.draw(entry);
        }
    }

};

/////////////////////////////////////////////////////////////////////////////////


class Menu {

	int selected_item_index; 

	Font font;
	Text menu[5]; // new game , continue,leaderboard, options, exit

    float width, height;

    Texture Logo;
    Sprite A;

public :
	Menu(float width, float height): width(width), height(height){
		
		if(!font.loadFromFile("OOP-Final/SyneMono-Regular.ttf"))
			cout<< "\nCannot load Font\n";
		
		menu[0].setFont(font);
		menu[0].setString("New Game");
		menu[0].setFillColor(Color::Blue);
		menu[0].Bold;
		menu[0].setPosition((width / 5)*3, (height / 6) * 1);

		menu[1].setFont(font);
		menu[1].setString("Continue");
		menu[1].setPosition((width / 5)*3, (height / 6) * 2);

		menu[2].setFont(font);
		menu[2].setString("View Leaderboard");
		menu[2].setPosition((width / 5) * 3, (height / 6) * 3);

		menu[3].setFont(font);
		menu[3].setString("Options");
		menu[3].setPosition((width / 5) * 3, (height / 6) * 4);

        menu[4].setFont(font);
        menu[4].setString("Exit");
        menu[4].setPosition((width / 5) * 3, (height / 6) * 5);


		selected_item_index = 0;

        Logo.loadFromFile("Data/LOGO.png");
        A.setTexture(Logo);
        A.setPosition(width / 12, height / 3);
        A.setScale(2.0, 2.0);

	}
	~Menu() {}


	void draw(RenderWindow &window) {
		
        window.draw(A);

		for (int i = 0; i < 5; i++) {
			window.draw(menu[i]);
		}
	}


	void moveup (){

		menu[selected_item_index].setFillColor(Color::White);

		selected_item_index--;

		if (selected_item_index == -1) {
			selected_item_index = 4;
		}
		menu[selected_item_index].setFillColor(Color::Blue);
	}


	void movedown (){

		menu[selected_item_index].setFillColor(Color::White);

		selected_item_index++;

		if (selected_item_index == 5) {
			selected_item_index = 0;
		}
		menu[selected_item_index].setFillColor(Color::Blue);
	}

	int getSelectedOption () { return selected_item_index; }

};


//////////////////////////////
class ScoreManager {
private:
    int currentScore;
    int highScore;

public:
    ScoreManager() : currentScore(0), highScore(0) {}

    int calculateScore() const {
        return currentScore; // or apply formula if needed
    }

    void updateScore(int value) {
        currentScore += value;
        if (currentScore > highScore)
            highScore = currentScore;
    }

    int getHighScore() const {
        return highScore;
    }

    void resetScore() {
        currentScore = 0;
    }
};

///////////////////////////////////////
class SaveData {
private:
    string playerName;
    int level;
    int health;
    float position[2];
    int activeCharacter;
    int collectedRings;
    int defeatedEnemies;
    int score;

public:
    SaveData() {
        playerName = "Unnamed";
        level = 0;
        health = 100;
        position[0] = 0.0f;
        position[1] = 0.0f;
        activeCharacter = 0;
        collectedRings = 0;
        defeatedEnemies = 0;
        score = 0;
    }

    void setPlayerName(const string& name) {
        playerName = name;
    }

    string getPlayerName() const {
        return playerName;
    }

    void setData(int lvl, int hp, float x, float y, int character, int rings, int enemies, int scr) {
        level = lvl;
        health = hp;
        position[0] = x;
        position[1] = y;
        activeCharacter = character;
        collectedRings = rings;
        defeatedEnemies = enemies;
        score = scr;
    }

    bool saveToFile(const string& filename) {
        ofstream out(filename);
        if (!out.is_open()) return false;

        out << playerName << '\n';
        out << level << ' ' << health << ' '
            << position[0] << ' ' << position[1] << ' '
            << activeCharacter << ' '
            << collectedRings << ' '
            << defeatedEnemies << ' '
            << score << '\n';

        out.close();
        return true;
    }

    bool loadFromFile(const string& filename) {
        ifstream in(filename);
        if (!in.is_open()) return false;

        getline(in, playerName);
        in >> level >> health >> position[0] >> position[1]
            >> activeCharacter >> collectedRings >> defeatedEnemies >> score;

        in.close();
        return true;
    }

    void printData() const { // FOR DEBUGGING

        cout << "Player Name: " << playerName
        << "\nLevel: " << level
        << "\nHealth: " << health
        << "\nPosition: (" << position[0] << ", " << position[1] << ")"
        << "\nCharacter: " << activeCharacter
        << "\nRings: " << collectedRings
        << "\nEnemies Defeated: " << defeatedEnemies
        << "\nScore: " << score << "\n";
    }
};

//////////////////////////////
