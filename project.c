/*
 * CSE 20311
 * project.c - Gerrymandering Game
 * Author: Caio Kwasnicki
 */

#include "projectfunc.h"

int main(int argc, char *argv[]) {

  char grid[GRIDMAX][GRIDMAX], cmd;
  GameTile board[GRIDMAX][GRIDMAX];
  GameState state = {3, 0, 0, 'R'};
  Tile lastSelected;
  int boardSize = WINSIZE * 0.7, nSelected, keepRunning = 1, hasWon = 0,
      nDistricts = 3;
  double difficulty = 0;

  gfx_open(WINSIZE, WINSIZE, "Gerrymandering");
  srand(time(NULL));

  state = startScreen(state);

  while (keepRunning) {

    // Increases the number of districts every 3 games (except in the start)
    difficulty += 0.35;
    if (difficulty >= 1 && nDistricts <= 17) {
      nDistricts += 2;
      difficulty = 0;
    }

    // Generates a random map with nDistricts
    initializeGrid(grid);
    generateDistricts(grid, state, nDistricts);
    convertToGameBoard(grid, board);

    nSelected = 0;
    hasWon = 0;
    state.nBlue = 0;
    state.nRed = 0;

    gfx_clear();
    displayCount(state, boardSize);
    renderBoard(board, boardSize);

    while (!hasWon && keepRunning) {
      cmd = gfx_wait();
      switch (cmd) {
      case 1:
        nSelected = handleClick(board, gfx_xpos(), gfx_ypos(), boardSize,
                                nSelected, &lastSelected, &state);
        break;
      case 'q':
        keepRunning = 0;
        break;
      default:
        continue;
      }
      displayCount(state, boardSize);
      renderBoard(board, boardSize);
      hasWon = checkCount(state, nDistricts);
    }
  }

  return 0;
}
