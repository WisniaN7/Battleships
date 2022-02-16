#ifndef AI_H
#define AI_H

// TODO: Still sometimes shoots at cells that have already been shot at on normal difficulty

#include "Sea.h"
#include <vector>
#include <numeric> // iota()
#include <algorithm> // random_shuffle
#include <functional> // funcion<>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

// Class: AI
// Creates an AI opponent for player to play against.
class AI {
  private:
    // Variable: targetSea
    // Board at which the AI will shot at. (Player's board)
    Sea * targetSea = nullptr;
    // Variable: coordinates
    // Contains coordinates (numbered from 0 to 99, where first digit means row and the second column) which are valid to shot at. 
    vector<char> coordinates = vector<char>(100);
    // Variable: turn
    // Indicates current turn for easy AI.
    char turn;
    // Variable: shootingDirection
    // Indicates shooting direction for normal AI. Takes values from -10, -1, 1, 10, meaning *up*, *left*, *right*, *down* accordingly.
    char shootingDirection;
    // Variable: lastSuccessfulShot
    // Stores coordinates of last succesfull shot.
    char lastSuccessfulShot;
    /*Variable: firstSuccessfulShot
      Stores coordinates of first succesfull shot at Ship that AI is trying currently to sink.

      See Also:
        <Ship>
    */
    char firstSuccessfulShot;
    /*Variable: numberOfHits
      Stores number of hits on the Ship that AI is trying currently to sink.

      See Also:
        <Ship>
    */
    char numberOfHits;
    /*Variable: shootingAtShip
      Flag indicating wheter AI hit a Ship, and therefore should pursue sinking that Ship.

      See Also:
        <Ship>
    */
    bool shootingAtShip = false;
    /*Variable: shootingDirectionDetermined
      Flag indicating wheter AI can be sure of direction in which Ship is placed.

      See Also:
        <Ship>
    */
    bool shootingDirectionDetermined = false;
    // Variable: coordinatesDisplay
    // Contains string to display that show player at which coordinate AI is shooting.
    string coordinatesDisplay[2][10] = {{"                                ______   \n                               /\\  __ \\  \n                               \\ \\  __ \\ \n                                \\ \\_\\ \\_\\\n                                 \\/_/\\/_/\n", "                                ______   \n                               /\\  == \\  \n                               \\ \\  __<  \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                ______   \n                               /\\  ___\\  \n                               \\ \\ \\____ \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                _____   \n                               /\\  __-. \n                               \\ \\ \\/\\ \\\n                                \\ \\____-\n                                 \\/____/\n", "                                ______   \n                               /\\  ___\\  \n                               \\ \\  __\\  \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                ______ \n                               /\\  ___\\\n                               \\ \\  __\\\n                                \\ \\_\\  \n                                 \\/_/  \n", "                                ______   \n                               /\\  ___\\  \n                               \\ \\ \\__ \\ \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                __  __   \n                               /\\ \\_\\ \\  \n                               \\ \\  __ \\ \n                                \\ \\_\\ \\_\\\n                                 \\/_/\\/_/\n", "                                __   \n                               /\\ \\  \n                               \\ \\ \\ \n                                \\ \\_\\\n                                 \\/_/\n", "                                  __   \n                                 /\\ \\  \n                                _\\_\\ \\ \n                               /\\_____\\\n                               \\/_____/\n"}, {"                                _____   \n                               /\\__  \\  \n                               \\/_ \\  \\ \n                                  \\ \\__\\\n                                   \\___/\n", "                                ______   \n                               /\\___  \\  \n                               \\/\\  ___\\ \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                ______   \n                               /\\___  \\  \n                               \\/\\___  \\ \n                                /\\______\\\n                                \\/______/\n", "                                __  __   \n                               /\\ \\_\\ \\  \n                               \\ \\___  \\ \n                                \\ ___\\__\\\n                                    \\/__/\n", "                                ______   \n                               /\\  ___\\  \n                               \\ \\___  \\ \n                                \\/\\_____\\\n                                 \\/_____/\n", "                                ______   \n                               /\\  __/   \n                               \\ \\  _  \\ \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                ______   \n                               /\\___  \\  \n                               \\/__/\\  \\ \n                                   \\ \\__\\\n                                    \\/__/\n", "                                ______   \n                               /\\  __ \\  \n                               \\ \\  __ \\ \n                                \\ \\_____\\\n                                 \\/_____/\n", "                                ______   \n                               /\\  __ \\  \n                               \\ \\___  \\ \n                                \\/__/\\__\\\n                                    \\/__/\n", "                                __     ______   \n                               /\\ \\   /\\  __ \\  \n                               \\ \\ \\  \\ \\ \\/\\ \\ \n                                \\ \\_\\  \\ \\_____\\\n                                 \\/_/   \\/_____/\n"}};
  public:
    // Variable: play
    // Stores a function determining AI difficulty and play style.
    function<void()> play;

    // Constructor: AI
    // Initializes the object.
    AI(char difficulty, Sea * _targetSea) : turn(0), targetSea(_targetSea) {
      iota(begin(coordinates), end(coordinates), 0);

      if (difficulty == 0) {
        random_device rd;
        mt19937 g(rd());
        shuffle(coordinates.begin(), coordinates.end(), g);

        play = [&]() {
          char row = coordinates[turn] / 10;
          char col = coordinates[turn] % 10;

          system(CLEAR);
          gotoxy(0, 7);
          cout << coordinatesDisplay[0][col] << coordinatesDisplay[1][row] << '\n';

          this_thread::sleep_for(chrono::milliseconds(1000));

          system(CLEAR);
          targetSea->shootAtCoords(row, col);
          turn++;
        };
      } else if (difficulty == 1) {
        play = [&]() {
          char index, row, col, coordinatesID;

          if (!shootingAtShip) {
            coordinatesID = rand() % coordinates.size();
            index = coordinates[coordinatesID];
            coordinates.erase(coordinates.begin() + coordinatesID);
          } else {
            bool oppositeDirIsBlocked = false;

            while (true) {
              if (find(coordinates.begin(), coordinates.end(), lastSuccessfulShot - shootingDirection) == coordinates.end()) {
                if (oppositeDirIsBlocked) {
                  shootingDirection = shootingDirection % 10 == 0 ? shootingDirection / 10 : shootingDirection * 10;
                } else {
                  shootingDirection = -shootingDirection;
                  oppositeDirIsBlocked = true;
                }
              } else {
                if (lastSuccessfulShot - shootingDirection < 0 || lastSuccessfulShot - shootingDirection >= 100) {
                  shootingDirection = -shootingDirection;
                  continue;
                }

                index = lastSuccessfulShot - shootingDirection;
                break;
              }
            }

            coordinates.erase(remove(coordinates.begin(), coordinates.end(), index), coordinates.end());
          }

          row = index / 10;
          col = index % 10;

          system(CLEAR);
          gotoxy(0, 7);
          cout << coordinatesDisplay[0][col] << coordinatesDisplay[1][row] << '\n';
          this_thread::sleep_for(chrono::milliseconds(1000));

          system(CLEAR);
          char hit = targetSea->shootAtCoords(row, col);

          if (!shootingAtShip && hit == 1) {
            shootingAtShip = true;
            char temp[] = {-10, -1, 1, 10};
            shootingDirection = temp[rand() % 4];
            numberOfHits = 1;
            firstSuccessfulShot = lastSuccessfulShot = index;
          } else if (shootingAtShip) {
            switch (hit) {
              case 0:
                shootingDirection = shootingDirectionDetermined ? -shootingDirection : (shootingDirection % 10 == 0 ? shootingDirection / 10 : shootingDirection * 10);
                if (shootingDirectionDetermined) lastSuccessfulShot = firstSuccessfulShot;
                break;
              case 1:
                if (++numberOfHits == 2) shootingDirectionDetermined = true;
                lastSuccessfulShot = index;
                break;
              case 2:
                shootingAtShip = false;
                shootingDirectionDetermined = false;
                Ship * sunkenShip = targetSea->getShipFromCell(row, col);

                if (sunkenShip->verticalDir) {
                  for (char i = (sunkenShip->row - 1) * 10; i <= (sunkenShip->row + sunkenShip->size) * 10; i += 10) {
                    for (char j = sunkenShip->col - 1; j <= sunkenShip->col + 1; j++) {
                      if (i + j >= 0 && i + j + 1 < 100 && i + j >= i && i + j <= i + 9) {
                        coordinates.erase(remove(coordinates.begin(), coordinates.end(), i + j), coordinates.end()); // correct version
                      }
                    }
                  }
                } else {
                  for (char i = (sunkenShip->row - 1) * 10; i <= (sunkenShip->row + 1) * 10; i += 10) {
                    for (char j = sunkenShip->col - 1; j <= sunkenShip->col + sunkenShip->size; j++) {
                      if (i + j >= 0 && i + j < 100 && i + j >= i && i + j <= i + 9) {
                        coordinates.erase(remove(coordinates.begin(), coordinates.end(), i + j), coordinates.end()); // correct version // doesn't work
                      }
                    }
                  }
                }

                break;
            }
          }
        };
      }
    }
};

#endif
