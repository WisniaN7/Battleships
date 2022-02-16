#include "Sea.h"
#include "AI.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <sstream> // string_split()
#include <algorithm> // get<>()
#include <iterator>
#include <thread>
#include <chrono>

#ifdef _WIN32
#define TERMINALSIZE "mode con:cols=76 lines=55"
#define ENCODING "chcp 65001"
#else
#define TERMINALSIZE "resize -s 55 76"
#define ENCODING ""
#endif

using namespace std;

/* Function: string_split
   Splits string by whitespaces.

   Parameters:
      str - string to split.

   Returns:
      Vector containing seperated string in each element.
*/
vector<string> string_split(const string &str) {
  vector<string> result;
  istringstream iss(str);
  copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(result));
  return result;
}

/* Function: shipFits
   Checks wether ship will fit onto the Sea grid, without colliding with other ships, or sailing right next to them.

   Parameters:
      grid - a 2D array of flags: *true* - cell is occupied (is invalid for placement); *false* - cell is free (is valid for placement),
      row - y coordinate of ship's bow,
      col - x coordinate of ship's bow,
      size - size fo the ship,
      verticalDir - orientation of the Ship: *true*- Ship is alligned vertically on the grid; *false* - Ship is alligned horizontally.

   Returns:
      True - Ship fits,
      False - Ship does not fit.

    See Also:
      <Ship>
*/
bool shipFits(const bool grid[10][10], const char &row, const char &col, const char &size, const bool &verticalDir) {
  if (verticalDir) {
    for (char i = row; i < row + size; i++) {
      if (grid[i][col]) return false;
    }
  } else {
    for (char i = col; i < col + size; i++) {
      if (grid[row][i]) return false;
    }
  }

  return true;
}

/* Function: markUnaviableCells
   Sets grid cells to *true* on the coordinates of the Ship, and all the cells around the ship.

   Parameters:
      grid - a 2D array of flags: *true* - cell is occupied (is invalid for placement); *false* - cell is free (is valid for placement),
      row - y coordinate of Ship's bow,
      col - x coordinate of Ship's bow,
      size - size fo the Ship,
      verticalDir - orientation of the Ship; *true* - Ship is alligned vertically on the grid, *false* horizontally.

    See Also:
      <Ship>
*/
void markUnaviableCells(bool grid[10][10], const char &row, const char &col, const char &size, const bool &verticalDir) {
  if (verticalDir) {
    for (int i = row - 1; i < row + size + 1; i++) {
      for (int j = col - 1; j <= col + 1; j++) {
        if (i < 0 || i > 9 || j < 0 || j > 9) continue;
        grid[i][j] = true;
      }
    }
  } else {
    for (int i = row - 1; i <= row + 1; i++) {
      for (int j = col - 1; j < col + size + 1; j++) {
        if (i < 0 || i > 9 || j < 0 || j > 9) continue;
        grid[i][j] = true;
      }
    }
  }
}

/* Function: createShips
   Creates player's vector of Ships.

   Parameters:
    sea - Sea, on which the Ships will be placed,
    random - decides wether Ships should be palced randomly (*true*), or manually by player (*false*).

  Returns:
    Vector containing player's Ships.
*/
vector<Ship *> createShips(Sea &sea, const bool &random) {
  bool grid[10][10];
  vector<Ship *> ships;

  for (size_t i = 0; i < 10; i++)
    for (size_t j = 0; j < 10; j++)
      grid[i][j] = false;

  pair<string, char> shipNames[5] = {make_pair("Carrier", 5), make_pair("Battleship", 4), make_pair("Crusier", 3), make_pair("Submarine", 3), make_pair("Destroyer", 2)};

  for (size_t i = 5; i > 0; i--) {
    char size = get<1>(shipNames[5 - i]), row, col;
    bool verticalDir;
    string name = get<0>(shipNames[5 - i]);

    if (random) {
      verticalDir = rand() % 2;

      if (verticalDir) {
        do {
          row = rand() % (10 - size);
          col = rand() % 10;
        } while(!shipFits(grid, row, col, size, verticalDir));
      } else {
        do {
          row = rand() % 10;
          col = rand() % (10 - size);
        } while(!shipFits(grid, row, col, size, verticalDir));
      }
    } else {
      bool correctPlacement = true;

      do {
        sea.displayBoard(true);
        cout << '\n';

        if (!correctPlacement) {
          cout << string(16, ' ') << K_RED << "Given coordinates are incorrect or ship doesn't fit there" << K_RESET << '\n';
          correctPlacement = true;
        } else {
          cout << '\n';
        }

        string input;
        cout << string(16, ' ') << "Choose direction (|, ─) and coordinates for\n" << string(16, ' ') << get<0>(shipNames[5 - i]) << " (size " << +size << ") (for example: \"- G 3\")\n\n" << string(16, ' ') << "Ships are placed downwards and to the right\n" << string(16, ' ') << "from given coordinates [DIR COL ROW]: ";
        getline(cin, input);
        vector<string> coordinates = string_split(input);
        system(CLEAR);

        if (coordinates.size() != 3 || !(coordinates[0][0] == '|' || coordinates[0][0] == '-') || toupper(coordinates[1][0]) < 'A' || toupper(coordinates[1][0]) > 'J' || stoi(coordinates[2]) < 1 || stoi(coordinates[2]) > 10) {
          correctPlacement = false;
          continue;
        }

        verticalDir = (coordinates[0][0] == '|');
        col = toupper(coordinates[1][0]) - 'A';
        row = stoi(coordinates[2]) - 1;

        if (!shipFits(grid, row, col, size, verticalDir)) correctPlacement = false;
      } while(!correctPlacement);
    }

    markUnaviableCells(grid, row, col, size, verticalDir);
    ships.push_back(new Ship(row, col, size, verticalDir));
    sea.placeShip(ships[5 - i]);
  }

  return ships;
}

/* Function: checkLoser
   Checks whether player has lost (all of their Ships had been sunk) and the game should end.

   Parameters:
    ships - player's Ships.

   Returns:
      True - player has lost (all Ships had been sunk),
      False - player has not lost (not all Ships had been sunk).
*/
bool checkLoser(vector<Ship *> &ships) {
  char counter = 0;
  for (size_t i = 0; i < ships.size(); i++) if (ships[i]->hasBeenSunk()) counter++;
  return counter == ships.size();
}

/* Function: playerTurn
   A controller for player's turn. Displays palyers board with visible ships shot indicators, and opponent's board with only shot indicatros, asks for coordinates to shoot at, and informs player whether the soot was sucessfull.

   Parameters:
    sea - array containing palyer's and opponents board,
    player - player id.

  See Also:
    <Sea.displayBoard>
*/
void playerTurn(Sea sea[2], const bool &player) {
  string input;
  bool validCoordinates = true, cellHasBennShotAt = false;
  vector<string> coordinates;
  char hit;

  do {
    do {
      system(CLEAR);

      sea[player].displayBoard(true);
      sea[(player + 1) % 2].displayBoard(false);

      if (!validCoordinates) {
        cout << K_RED << "\n" << string(17, ' ') << "Given coordinates are incorrect (valid\n" << string(17, ' ') << "example: F 9)\n" << K_RESET << '\n';
        validCoordinates = true;
      } else if (cellHasBennShotAt) {
        cout << K_YELLOW << "\n" << string(17, ' ') << "Cell at given coordinates has already\n" << string(17, ' ') << "been shot at\n" << K_RESET << '\n';
      } else {
        cout << "\n\n\n\n";
      }

      cout << string(17, ' ') << "Shoot at [col row]: ";
      getline(cin, input);
      coordinates = string_split(input);

      if (coordinates.size() != 2 || toupper(coordinates[0][0]) < 'A' || toupper(coordinates[0][0]) > 'J' || atoi(coordinates[1].c_str()) < 1 || atoi(coordinates[1].c_str()) > 10) validCoordinates = false;
    } while(!validCoordinates);

    char col = toupper(coordinates[0][0]) - 'A';
    char row = atoi(coordinates[1].c_str()) - 1;

    system(CLEAR);
    hit = sea[(player + 1) % 2].shootAtCoords(row, col);

    cellHasBennShotAt = (hit == -1);
  } while(hit == -1);
}

/* Function: playerChange
   Provides a fair way to change players in hotseat mode. Displays blank screen with prompt to input "ready" when the player change is complete.

  See Also:
    <hotseat>
*/
void playerChange(bool player) {
  string readynessCheck;

  do {
    system(CLEAR);
    gotoxy(0, 16);
    cout << string(30, ' ') << "Player change\n" << string(10, ' ') << "Player " << player + 1 << " input \"ready\" and press enter to continue: ";
    getline(cin, readynessCheck);
  } while(readynessCheck.compare("ready") != 0);
}

/* Function: displayWinner
   Displays which player has won.

  Parameters:
    winner - victorious player id.
*/
void displayWinner(const char &winner) {
  system(CLEAR);
  gotoxy(0, 7);

  cout << string(5, ' ') << " ______   __         ______     __  __     ______     ______    \n"
       << string(5, ' ') << "/\\  == \\ /\\ \\       /\\  __ \\   /\\ \\_\\ \\   /\\  ___\\   /\\  == \\   \n"
       << string(5, ' ') << "\\ \\  _-/ \\ \\ \\____  \\ \\  __ \\  \\ \\____ \\  \\ \\  __\\   \\ \\  __<   \n"
       << string(5, ' ') << " \\ \\_\\    \\ \\_____\\  \\ \\_\\ \\_\\  \\/\\_____\\  \\ \\_____\\  \\ \\_\\ \\_\\ \n"
       << string(5, ' ') << "  \\/_/     \\/_____/   \\/_/\\/_/   \\/_____/   \\/_____/   \\/_/ /_/ \n";

  if (winner) cout << string(32, ' ') << " ______   \n" << string(32, ' ') <<"/\\___  \\  \n" << string(32, ' ') <<"\\/\\  ___\\ \n" << string(32, ' ') <<" \\ \\_____\\\n" << string(32, ' ') <<"  \\/_____/\n";
  else cout << string(32, ' ') << " _____   \n" << string(32, ' ') <<"/\\__  \\  \n" << string(32, ' ') <<"\\/_ \\  \\ \n" << string(32, ' ') <<"   \\ \\__\\\n" << string(32, ' ') <<"    \\___/\n";

  cout << string(18, ' ') << " __     __     __     __   __     ______   \n"
       << string(18, ' ') << "/\\ \\  _ \\ \\   /\\ \\   /\\ \"-.\\ \\   /\\  ___\\  \n"
       << string(18, ' ') << "\\ \\ \\/ \".\\ \\  \\ \\ \\  \\ \\ \\-.  \\  \\ \\___  \\ \n"
       << string(18, ' ') << " \\ \\__/\".~\\_\\  \\ \\_\\  \\ \\_\\\\\"\\_\\  \\/\\_____\\\n"
       << string(18, ' ') << "  \\/_/   \\/_/   \\/_/   \\/_/ \\/_/   \\/_____/\n";

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

/* Function: playWithAI
   Controller for a game with AI. Asks player wether they want to place thier Ships manually, or randomly and starts the game.

  Parameters:
    difficulty - AI difficulty.

  See Also:
    <AI>
    <Ship>
*/
void playWithAI(char difficulty) {
  Sea sea[2];
  vector<Ship *> ships[2];
  string action;

  do {
    gotoxy(9, 10);
    cout << "Would you like to randomly select ship locations (Y/N)? ";
    getline(cin, action);
    system(CLEAR);
  } while(toupper(action[0]) != 'Y' && toupper(action[0]) != 'N');

  ships[0] = createShips(sea[0], toupper(action[0]) == 'Y');

  AI ai(difficulty, &sea[0]);
  ships[1] = createShips(sea[1], true);

  char winner = -1;

  while (1) {
    // Player turn
    playerTurn(sea, 0);

    if (checkLoser(ships[1])) {
      winner = 0;
      break;
    }

    // AI turn
    ai.play();

    if (checkLoser(ships[0])) {
      winner = 1;
      break;
    }
  }

  displayWinner(winner);
}

// Debug function
void AIvsAI(char difficulty) {
  Sea sea[2];
  vector<Ship *> ships[2];
  string action;

  ships[0] = createShips(sea[0], true);
  ships[1] = createShips(sea[1], true);

  AI aitest(difficulty, &sea[0]);
  AI ai(difficulty, &sea[1]);

  char winner = -1;

  while (1) {
    aitest.play();

    if (checkLoser(ships[1])) {
      winner = 0;
      break;
    }

    ai.play();

    if (checkLoser(ships[0])) {
      winner = 1;
      break;
    }
  }

  displayWinner(winner);

  sea[0].displayBoard(true);
  sea[1].displayBoard(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/* Function: hotseat
   Controller for a hotseat game. Asks players wether they want to place thier Ships manually, or randomly and starts the game.

  See Also:
    <Ship>
*/
void hotseat() {
  Sea sea[2];
  vector<Ship *> ships[2];
  string action;

  for (size_t i = 1; i <= 2; i++) {
    do {
      gotoxy(5, 10);
      cout << "Player " << i << " would you like to randomly select ship locations (Y/N)? ";
      getline(cin, action);
      system(CLEAR);
    } while(toupper(action[0]) != 'Y' && toupper(action[0]) != 'N');

    ships[i - 1] = createShips(sea[i - 1], toupper(action[0]) == 'Y');
  }

  char winner = -1;

  while (1) {
    playerTurn(sea, 0);

    if (checkLoser(ships[1])) {
      winner = 0;
      break;
    }

    playerChange(1);
    playerTurn(sea, 1);

    if (checkLoser(ships[0])) {
      winner = 1;
      break;
    }

    playerChange(0);
  }

  displayWinner(winner);
}

/* Function: displayLogo
   Displays game's logo in the main menu.
*/
void displayLogo() {
  cout << string(12, ' ') <<  " _           _   _   _           _     _           \n"
       << string(12, ' ') <<  "| |         | | | | | |         | |   (_)          \n"
       << string(12, ' ') <<  "| |__   __ _| |_| |_| | ___  ___| |__  _ _ __  ___ \n"
       << string(12, ' ') <<  "| '_ \\ / _` | __| __| |/ _ \\/ __| '_ \\| | '_ \\/ __|\n"
       << string(12, ' ') <<  "| |_) | (_| | |_| |_| |  __/\\__ \\ | | | | |_) \\__ \\\n"
       << string(12, ' ') <<  "|_.__/ \\__,_|\\__|\\__|_|\\___||___/_| |_|_| .__/|___/\n"
       << string(12, ' ') <<  "                                        | |        \n"
       << string(12, ' ') <<  "                                        |_|        \n\n";
}

int main(int argc, char const *argv[]) {
  system(ENCODING);
  system(TERMINALSIZE);
  srand(time(NULL));
  string action, difficulty;

  while (1) {
    system(CLEAR);
    displayLogo();

    gotoxy(31, 10);
    cout << "1. Play with AI";
    gotoxy(31, 11);
    cout << "2. Hotseat mode";
    gotoxy(35, 12);
    cout << "3. Quit\n";

    do {
      cout << '\r';
      gotoxy(39, 14);
      getline(cin, action);
    } while(action[0] < '1' || action[0] > '3');

    system(CLEAR);

    switch (action[0]) {
      case '1':
        displayLogo();

        gotoxy(30, 10);
        cout << "Select difficulty";
        gotoxy(35, 11);
        cout << "1. Easy";
        gotoxy(34, 12);
        cout << "2. Normal\n";

        do {
          cout << '\r';
          gotoxy(39, 14);
          getline(cin, difficulty);
        } while(difficulty[0] < '1' || difficulty[0] > '2');

        system(CLEAR);
        // AIvsAI(difficulty[0] - '1');
        playWithAI(difficulty[0] - '1');
        break;
      case '2':
        hotseat();
        break;
      case '3':
        exit(0);
    }
  }

  return 0;
}
