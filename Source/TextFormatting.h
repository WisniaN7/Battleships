#ifndef TEXTFORMATTING_H
#define TEXTFORMATTING_H

#include <iostream>

#define K_BLACK "\u001b[30m"
#define K_RED "\u001b[31m"
#define K_GREEN "\u001b[32m"
#define K_YELLOW "\u001b[33m"
#define K_BLUE "\u001b[34m"
#define K_MAGENTA "\u001b[35m"
#define K_CYAN "\u001b[36m"
#define K_WHITE "\u001b[37m"
#define K_RESET "\u001b[0m"

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

/*Function: gotoxy
  Places cursor in given coordinates.

  Parameters:
    x - x coordiante,
    y - y coordinate.
*/
void gotoxy(unsigned x, unsigned y) {
  printf("\033[%d;%dH\033[2K", y, x);
  return;
}

#endif
