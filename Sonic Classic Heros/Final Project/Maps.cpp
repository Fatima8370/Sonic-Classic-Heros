//#include <iostream>
//#include <fstream>
//#include "Obstacles.h"
//#include "Enemies.h"
//#include "Collectibles.h"
//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>
//#include <SFML/Audio.hpp>
//
//using namespace sf;
//using namespace std;    
//
//const int height = 14;
//const int width = 200;
//
//int main() {
//    char position[height][width];
//    char obstacles[height][width] = {};
//    char collectibles[height][width] = {};
//    char enemies[height][width] = {};
//
//
//	int enemyCount = 0;
//	int collectibleCount = 0;
//	int obstacleCount = 0;
//
//
//    ifstream file("Data/Level/level1.csv");
//
//    if (!file) {
//        cerr << "Failed to open file.\n";
//        return 1;
//    }
//
//    for (int i = 0; i < height; ++i) {
//        for (int j = 0; j < width; ++j) {
//            char ch;
//            file.get(ch);
//
//            if (file.eof()) {
//                cerr << "Unexpected end of file.\n";
//                return 1;
//            }
//
//            position[i][j] = ch;
//
//            if (ch == ' ') {
//                continue; // ignore spaces
//            }
//            else if (ch >= 'A' && ch <= 'Z') {
//                obstacles[i][j] = ch;
//				obstacleCount++;
//            }
//            else if (ch >= 'a' && ch <= 'z') {
//                collectibles[i][j] = ch;
//				collectibleCount++;
//            }
//            else if (ch >= '0' && ch <= '9') {
//                enemies[i][j] = ch;
//				enemyCount++;
//            }
//        }
//        file.ignore(1); // skip newline
//    }
//
//
//	file.close();
//
//
//	//CollectibleFactory CF(14, 200, 1);
//
//	Obstacles** obstacle;
//	obstacle = new Obstacles * [obstacleCount]; 
//	int count = 0;
//
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			if (obstacles[i][j] != '\0') {
//				Obstacles* o = ObstacleFactory::createObstacle(obstacles[i][j], j * 64, i * 64);
//				if (o) {
//					obstacle[count++] = o;
//					cout << "Spawned obstacle of type: " << obstacles[i][j] << endl;
//				}
//			}
//		}
//	}
//	//delete[] obstacle;
//
//
//    EnemyFactory EM(14, 200, 1);
//
//    Enemies** enemy;
//	enemy = new Enemies * [enemyCount];
//	count = 0;
//
//	for (int i = 0; i < height; ++i) {  
//		for (int j = 0; j < width; ++j) {
//			if (enemies[i][j] != '\0') {
//				Enemies* e = EM.createEnemy(enemies[i][j], j * 64, i * 64);
//				if (e) {
//					enemy[count++] = e;
//					cout << "Spawned enemy of type: " << enemies[i][j] << endl;
//				}
//			}
//		}
//	}
//	//delete[] enemy;
//
//
//
//
//    
//
//    return 0;
//}
