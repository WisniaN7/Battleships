#ifndef SEA_H
#define SEA_H

#include "TextFormatting.h"
#include <thread>
#include <chrono>

using namespace std;

// Class: Ship
// Class representing ships.
class Ship {
  public:
    // Variable: row
    // Ship's bow y coordinate.
    char row; 
    // Variable: col
    // Ship's bow x coordinate.
    char col; 
    // Variable: size
    // Ship's size.
    char size; 
    // Variable: hits
    // Counts how many times ship has been hit.
    char hits;
    // Variable: verticalDir
    // Describes ship's alligment: *true* ‐ alligned vertically (downwards from bow coordinates); *false* ‐ horizontally (to the right from bow coordinates).
    bool verticalDir;
    // Constructor: Ship
    // Initializes the object.
    Ship(char _row, char _col, char _size, bool _verticalDir) : row(_row), col(_col), size(_size), verticalDir(_verticalDir), hits(0) { }
    /*Function: hasBeenSunk
      Checks wether ship has been sunk (hits == size).

      Returns:
        True - ship has been sunk,
        False - ship has not been sunk.
    */
    bool hasBeenSunk() { return size == hits; }
    // Function: hit
    // Increments hits counter
    void hit() { hits++; }
};

// Class: cell
// Represents one cell of a Sea.
class cell {
  public:
    // Variable: ship
    // Pointer to a Ship that occupies that cell. 
    Ship * ship = nullptr;
    // Variable: hasBeenShotAt
    // Flag set to *true* if cell has already been shot at and set to *false* if it has not been shot at yet.
    bool hasBeenShotAt = 0;
};

// Class: Sea
// Playing board.
class Sea {
  private:
    // Variable: cells
    // Building block of a Sea. Each sea consists of 10x10 cells.
    cell cells[10][10];
  public:
    /*Function: placeShip
      Places Ships on the Sea.

      Parameters:
        Ship - a Ship to be placed on the Sea.
    */
    void placeShip(Ship * ship) {
      if (ship->verticalDir)
        for (size_t i = 0; i < ship->size; i++)
          cells[ship->row + i][ship->col].ship = ship;
      else
        for (size_t i = 0; i < ship->size; i++)
          cells[ship->row][ship->col + i].ship = ship;
    }

    /*Function: displayBoard
      Dispalys playing board (Sea) with shot marking and optionally ship placement. 

      Parameters:
        showShips - parameter saying wether ship placement should be visible (*true*) or not (*false*).
    */
    void displayBoard(bool showShips) {
      cout << '\n'
           << string(17, ' ') << "  A   B   C   D   E   F   G   H   I   J  \n"
           << string(17, ' ') << "┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐\n";

      for (size_t i = 0; i < 10; i++) {
        cout << ((i == 9) ? string(14, ' ') : string(15, ' ')) << i + 1 << " │";

        for (size_t j = 0; j < 10; j++)  {
          if (showShips) {
            if (cells[i][j].ship != nullptr && cells[i][j].hasBeenShotAt) cout << K_RED << "███" << K_RESET << "│";
            else if (cells[i][j].ship != nullptr && !cells[i][j].hasBeenShotAt) cout << "███│";
            else if (cells[i][j].ship == nullptr && cells[i][j].hasBeenShotAt) cout << " • │";
            else cout << "   │";
          } else {
            if (cells[i][j].ship != nullptr && cells[i][j].hasBeenShotAt) {
              if (cells[i][j].ship->hasBeenSunk()) cout << K_RED << "███" << K_RESET << "│";
              else cout << K_RED << " X " << K_RESET << "│";
            }
            else if (cells[i][j].ship == nullptr && cells[i][j].hasBeenShotAt) cout << " • |";
            else cout << "   │";
          }
        }

        cout << '\n' << string(17, ' ') << ((i == 9) ? "└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘\n" : "├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤\n");
      }
    }

    /*Function: markCellsAroundSunkenShip
      Marks cells around sunken Ship as "shot at" (hasBeenShotAt = true).

      Parameters:
        row - y coordinate of ship's bow,
        col - x coordinate of ship's bow,
        size - size fo the ship,
        verticalDir - orientation of the ship; *true* means it is alligned vertically on the grid, *false* horizontally.

      See Also:
        <cell>
    */
    void markCellsAroundSunkenShip(char row, char col, char size, bool verticalDir) {
      if (verticalDir) {
        for (char i = row - 1; i < row + size + 1; i++) {
          for (char j = col - 1; j <= col + 1; j++) {
            if (i < 0 || i > 9 || j < 0 || j > 9) continue;
            cells[i][j].hasBeenShotAt = true;
          }
        }
      } else {
        for (char i = row - 1; i <= row + 1; i++) {
          for (char j = col - 1; j < col + size + 1; j++) {
            if (i < 0 || i > 9 || j < 0 || j > 9) continue;
            cells[i][j].hasBeenShotAt = true;
          }
        }
      }
    }

    /*Function: shootAtCoords
      Shoots at given coordinate. Marks given cell as "shot at", and informs about shot's success or failure.

      Parameters:
        row - y coordinate of ship's bow,
        col - x coordinate of ship's bow,
        shipSize - optional parameter, informs AI about Ship size, so it can flag cells around the Ship as invalid after sinking it.

      Returns:
        -1 - if cell has already been shot at (is invalid),
        0 - if shot missed,
        1 - if shot hit,
        2 - if shot hit, and the Ship has sunk.

      See Also:
        <AI>
        <cell>
    */
    char shootAtCoords(char row, char col) {
      if (cells[row][col].hasBeenShotAt) return -1;

      cells[row][col].hasBeenShotAt = true;

      if (cells[row][col].ship != nullptr) {
        cells[row][col].ship->hits++;

        gotoxy(0, 10);
        cout << string(22, ' ') << " __  __     __     ______  \n"
             << string(22, ' ') << "/\\ \\_\\ \\   /\\ \\   /\\__  _\\ \n"
             << string(22, ' ') << "\\ \\  __ \\  \\ \\ \\  \\/_/\\ \\/ \n"
             << string(22, ' ') << " \\ \\_\\ \\_\\  \\ \\_\\    \\ \\_\\ \n"
             << string(22, ' ') << "  \\/_/\\/_/   \\/_/     \\/_/ \n";

        if (cells[row][col].ship->hasBeenSunk()) {
           bool verticalDir = cells[row][col].ship->verticalDir;
           char size = cells[row][col].ship->size;
           row = cells[row][col].ship->row;
           col = cells[row][col].ship->col;

           markCellsAroundSunkenShip(row, col, size, verticalDir);

          cout << string(22, ' ') << " ______     __   __     _____\n"
               << string(22, ' ') << "/\\  __ \\   /\\ \"-.\\ \\   /\\  __-.\n"
               << string(22, ' ') << "\\ \\  __ \\  \\ \\ \\-.  \\  \\ \\ \\/\\ \\\n"
               << string(22, ' ') << " \\ \\_\\ \\_\\  \\ \\_\\\\\"\\_\\  \\ \\____-\n"
               << string(22, ' ') << "  \\/_/\\/_/   \\/_/ \\/_/   \\/____/\n"
               << string(18, ' ') << " ______     __  __     __   __     __  __    \n"
               << string(18, ' ') << "/\\  ___\\   /\\ \\/\\ \\   /\\ \"-.\\ \\   /\\ \\/ /    \n"
               << string(18, ' ') << "\\ \\___  \\  \\ \\ \\_\\ \\  \\ \\ \\-.  \\  \\ \\  _\"-.  \n"
               << string(18, ' ') << " \\/\\_____\\  \\ \\_____\\  \\ \\_\\\\\"\\_\\  \\ \\_\\ \\_\\ \n"
               << string(18, ' ') << "  \\/_____/   \\/_____/   \\/_/ \\/_/   \\/_/\\/_/ \n";

          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          return 2;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return 1;
      } else if (cells[row][col].hasBeenShotAt == true) {
        gotoxy(0, 10);

        cout << string(15, ' ') << "   __    __     __     ______     ______    \n"
             << string(15, ' ') << "  /\\ \"-./  \\   /\\ \\   /\\  ___\\   /\\  ___\\  \n"
             << string(15, ' ') << "  \\ \\ \\-./\\ \\  \\ \\ \\  \\ \\___  \\  \\ \\___  \\ \n"
             << string(15, ' ') << "   \\ \\_\\ \\ \\_\\  \\ \\_\\  \\/\\_____\\  \\/\\_____\\\n"
             << string(15, ' ') << "    \\/_/  \\/_/   \\/_/   \\/_____/   \\/_____/\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return 0;
      }
    }

    Ship * getShipFromCell(char row, char col) { return cells[row][col].ship; }
};

#endif
