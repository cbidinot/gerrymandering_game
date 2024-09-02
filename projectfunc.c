/*
 * CSE 20311
 * projectfunc.c
 * Author: Caio Kwasnicki
 */

#include "projectfunc.h"

// Returns random in inside the desired interval
int getRandInt(int min, int max) { return min + rand() % (max - min + 1); }

// Returns either 'R' or 'B'
char getRandAlignment() {
  if (rand() % 2)
    return 'B';
  else
    return 'R';
}

// Sets the entire grid to 0
void initializeGrid(char grid[][GRIDMAX]) {
  int i, j;
  for (i = 0; i < GRIDMAX; i++)
    for (j = 0; j < GRIDMAX; j++)
      grid[i][j] = 0;
}

// Allocates and places voters in the pattern of a district around the starting
// coordinates
int placeDistrict(char grid[][GRIDMAX], int x, int y, GameState state,
                  char majority) {

  int nMaj, nMin, randIndex, hasPlaced = 0;
  char minority = (majority == 'R') ? 'B' : 'R';
  char nextTile;
  Tile neighbors[NEIGHBORSMAX];

  int count = findNeighbors(grid, neighbors, x, y);

  // Checks if there is enough space to place the district, returning 1
  // otherwise
  if (count < state.size - 1)
    return 1;

  // Separates the function in two scenarios:
  // - When the district's winning party is the party the player is aligned with
  //    - District should have minimum majority of the winning party's voters
  // - When the district's winning party is the player's opponent
  //    - District should have only opposing party's voters
  if (majority == state.alignment) {
    nMaj = (state.size / 2) + 1;
    nMin = (state.size / 2);

    // Fills the district in a random manner while fullfiling the voter
    // requirements
    nextTile = getRandAlignment();
    if (nextTile == majority) {
      grid[y][x] = majority;
      nMaj--;
    } else {
      grid[y][x] = minority;
      nMin--;
    }

    while (nMin || nMaj) {
      randIndex = getRandInt(0, count - 1);
      hasPlaced = 0;
      while (!hasPlaced) {
        nextTile = getRandAlignment();
        if (nextTile == majority) {
          if (nMaj) {
            grid[neighbors[randIndex].y][neighbors[randIndex].x] = majority;
            nMaj--;
            hasPlaced = 1;
          }
        } else if (nMin) {
          grid[neighbors[randIndex].y][neighbors[randIndex].x] = minority;
          nMin--;
          hasPlaced = 1;
        }
      }
      count = rmTile(neighbors, count, randIndex);
    }

  } else {
    nMaj = state.size;
    grid[y][x] = majority;
    nMaj--;

    // Selects available neighbors to fill the district, chooses a random member
    // of the neighbors array and then removes it
    while (nMaj) {
      randIndex = getRandInt(0, count - 1);
      grid[neighbors[randIndex].y][neighbors[randIndex].x] = majority;
      nMaj--;
      count = rmTile(neighbors, count, randIndex);
    }
  }
  return 0;
}

// Finds free neighboring tiles and stores their coordinates in an array,
// returning the quantity
int findNeighbors(char grid[][GRIDMAX], Tile neighbors[], int x, int y) {
  int i, count = 0;

  for (i = -1; i < 2; i += 2) {
    if (x + i < GRIDMAX && x + i >= 0 && !grid[y][x + i]) {
      neighbors[count].x = x + i;
      neighbors[count].y = y;
      count++;
    }
    if (y + i < GRIDMAX && y + i >= 0 && !grid[y + i][x]) {
      neighbors[count].x = x;
      neighbors[count].y = y + i;
      count++;
    }
  }
  return count;
}

// Removes a tile from a Tile array
int rmTile(Tile arr[], int size, int index) {
  int i = index;
  while (i < size - 1) {
    arr[i] = arr[i + 1];
    i++;
  }
  return size - 1;
}

// Finds all the neighboring tiles around already placed districts, storing
// their coordinates in an array and returning the quantity
int findDistrictNeighbors(char grid[][GRIDMAX], Tile initialTile,
                          Tile neighbors[]) {
  char neighborsGrid[GRIDMAX][GRIDMAX];
  TileStack occupiedStack;
  Tile currentTile;
  int i, j, x, y, count = 0;

  // Initializes a second grid
  initializeGrid(neighborsGrid);

  // Initializes the stack containing non-empty tiles
  occupiedStack.size = 1;
  occupiedStack.arr[0] = initialTile;
  neighborsGrid[initialTile.y][initialTile.x] = 'O';

  // Uses the stack to search through the main grid and mark every neighboring
  // tile in the neighborsGrid
  while (occupiedStack.size) {
    currentTile = occupiedStack.arr[occupiedStack.size - 1];
    occupiedStack.size--;

    x = currentTile.x;
    y = currentTile.y;

    for (i = -1; i < 2; i += 2) {
      if (y + i < GRIDMAX && y + i >= 0) {
        if (grid[y + i][x] && neighborsGrid[y + i][x] != 'O') {
          occupiedStack.arr[occupiedStack.size].x = x;
          occupiedStack.arr[occupiedStack.size].y = y + i;
          (occupiedStack.size)++;
          neighborsGrid[y + i][x] = 'O';
        } else if (!neighborsGrid[y + i][x]) {
          neighborsGrid[y + i][x] = 'N';
        }
      }
      if (x + i < GRIDMAX && x + i >= 0) {
        if (grid[y][x + i] && neighborsGrid[y][x + i] != 'O') {
          occupiedStack.arr[occupiedStack.size].x = x + i;
          occupiedStack.arr[occupiedStack.size].y = y;
          (occupiedStack.size)++;
          neighborsGrid[y][x + i] = 'O';
        } else if (!neighborsGrid[y][x + i]) {
          neighborsGrid[y][x + i] = 'N';
        }
      }
    }
  }

  // Goes through the marked neighbors in the neighbors grid and stores them in
  // the array
  for (i = 0; i < GRIDMAX; i++)
    for (j = 0; j < GRIDMAX; j++)
      if (neighborsGrid[i][j] == 'N') {
        neighbors[count].x = j;
        neighbors[count].y = i;
        count++;
      }
  return count;
}

// Allocates and places voters in the grid for a puzzle with nDistricts
void generateDistricts(char grid[][GRIDMAX], GameState state, int nDistricts) {

  char alignment;
  int nWin, nLos, neighborsCount;
  Tile districtRoot, districtNeighbors[BUFFERMAX];

  nWin = nDistricts / 2 + 1;
  nLos = nDistricts / 2;

  // Place first district somewhere in the middle
  districtRoot.x = getRandInt(GRIDMAX / 4, 3 * GRIDMAX / 4);
  districtRoot.y = getRandInt(GRIDMAX / 4, 3 * GRIDMAX / 4);

  // Places districts making sure to make the puzzle possible
  while (nWin > 0 || nLos > 0) {

    alignment = getRandAlignment();

    if (alignment == state.alignment) {
      if (nWin <= 0)
        continue;
    } else {
      if (nLos <= 0)
        continue;
    }

    if (placeDistrict(grid, districtRoot.x, districtRoot.y, state, alignment)) {
      districtRoot = districtNeighbors[getRandInt(0, neighborsCount - 1)];
      continue;
    }

    if (alignment == state.alignment)
      nWin--;
    else
      nLos--;

    neighborsCount =
        findDistrictNeighbors(grid, districtRoot, districtNeighbors);

    districtRoot = districtNeighbors[getRandInt(0, neighborsCount - 1)];
  }
}

void drawSquare(int x, int y, int halfSide) {
  int i, j;
  for (j = -1; j < 2; j += 2)
    for (i = -1; i < 2; i += 2)
      gfx_line(x + j * halfSide, y + j * halfSide, x - i * halfSide,
               y + i * halfSide);
}

void drawSquareBackground(int x, int y, int boardSize, Color bgColor) {

  int delta = (boardSize / GRIDMAX) / 2;

  gfx_color(bgColor.r, bgColor.g, bgColor.b);

  gfx_fill_rectangle(x - delta, y - delta, boardSize / GRIDMAX,
                     boardSize / GRIDMAX);
}

void convertToGameBoard(char grid[][GRIDMAX], GameTile board[][GRIDMAX]) {
  int i, j;

  for (i = 0; i < GRIDMAX; i++)
    for (j = 0; j < GRIDMAX; j++) {
      if (grid[i][j]) {
        board[i][j].isOccupied = 1;
        board[i][j].alignment = grid[i][j];
        board[i][j].state = standby;
      } else {
        board[i][j].isOccupied = 0;
      }
    }
}

void renderBoard(GameTile board[][GRIDMAX], int boardSize) {

  int i, j, iPos = (WINSIZE - boardSize) / 2 + boardSize / (2 * GRIDMAX);
  Color black = {0, 0, 0}, gray = {40, 40, 40}, white = {255, 255, 255},
        lightBlue = {0, 96, 191}, lightRed = {191, 77, 77}, red = {255, 0, 0},
        blue = {0, 0, 255};

  gfx_color(white.r, white.g, white.b);
  drawSquare(WINSIZE / 2, WINSIZE / 2, boardSize / 2);

  for (i = 0; i < GRIDMAX; i++)
    for (j = 0; j < GRIDMAX; j++) {
      if (board[i][j].isOccupied) {

        // Tile Background Color
        switch (board[i][j].state) {
        case standby:
          drawSquareBackground(iPos + j * boardSize / GRIDMAX,
                               iPos + i * boardSize / GRIDMAX, boardSize,
                               black);
          break;
        case selected:
          drawSquareBackground(iPos + j * boardSize / GRIDMAX,
                               iPos + i * boardSize / GRIDMAX, boardSize, gray);
          break;
        case districted:
          if (board[i][j].districtAlignment == 'R')
            drawSquareBackground(iPos + j * boardSize / GRIDMAX,
                                 iPos + i * boardSize / GRIDMAX, boardSize,
                                 lightRed);
          else
            drawSquareBackground(iPos + j * boardSize / GRIDMAX,
                                 iPos + i * boardSize / GRIDMAX, boardSize,
                                 lightBlue);
          break;
        }

        // Tile Square
        gfx_color(white.r, white.g, white.b);
        drawSquare(iPos + j * boardSize / GRIDMAX,
                   iPos + i * boardSize / GRIDMAX, boardSize / (2 * GRIDMAX));

        // Voter circle
        if (board[i][j].alignment == 'R')
          gfx_color(red.r, red.g, red.b);
        else
          gfx_color(blue.r, blue.g, blue.b);

        gfx_fill_circle(iPos + j * boardSize / GRIDMAX,
                        iPos + i * boardSize / GRIDMAX,
                        boardSize / (4 * GRIDMAX));
      }
    }
}

// Handles the state of the tiles and districts after a mouse click. Also
// identifies in which tile the mouse clicked
int handleClick(GameTile board[][GRIDMAX], int x, int y, int boardSize,
                int nSelected, Tile *lastSelected, GameState *state) {
  int boardX, boardY, i, j, delta = boardSize / GRIDMAX,
                            iPos = (WINSIZE - boardSize) / 2, foundX = 0,
                            foundY = 0;

  for (i = 0; i < GRIDMAX && !foundY; i++)
    if (y > iPos + i * delta && y < iPos + (i + 1) * delta) {
      boardY = i;
      foundY = 1;
    }

  for (j = 0; j < GRIDMAX && !foundX; j++)
    if (x > iPos + j * delta && x < iPos + (j + 1) * delta) {
      boardX = j;
      foundX = 1;
    }

  if (!foundX || !foundY || !board[boardY][boardX].isOccupied)
    return nSelected;

  // Is there already selected tiles?
  if (nSelected) {

    // Expand Selection
    if (board[boardY][boardX].state == standby) {

      if (checkIfAdjacent(board[lastSelected->y][lastSelected->x].district,
                          *lastSelected, nSelected, boardX, boardY)) {
        updateDistrictA(board, *lastSelected, nSelected, boardX, boardY);
        nSelected++;
        if (nSelected == state->size) {
          finishDistrict(board, boardX, boardY, state);
          nSelected = 0;
        } else {
          board[boardY][boardX].state = selected;
          lastSelected->x = boardX;
          lastSelected->y = boardY;
        }
      }

      // Unselect a tile
    } else if (board[boardY][boardX].state == selected) {
      board[boardY][boardX].state = standby;
      for (i = 0; i < nSelected - 1; i++)
        updateDistrictR(&board[board[boardY][boardX].district[i].y]
                              [board[boardY][boardX].district[i].y],
                        boardX, boardY);
      nSelected--;
      if (nSelected)
        *lastSelected = board[boardY][boardX].district[nSelected - 1];
    }

  } else {
    // First selected tile
    if (board[boardY][boardX].state == standby) {
      board[boardY][boardX].state = selected;
      nSelected++;
      lastSelected->x = boardX;
      lastSelected->y = boardY;

      // Breaking up an already finished district
    } else {
      board[boardY][boardX].state = standby;
      for (i = 0; i < DISTRICTSIZE - 1; i++) {
        board[board[boardY][boardX].district[i].y]
             [board[boardY][boardX].district[i].x]
                 .state = selected;
        nSelected++;

        updateDistrictR(&board[board[boardY][boardX].district[i].y]
                              [board[boardY][boardX].district[i].x],
                        boardX, boardY);
      }
      if (board[boardY][boardX].districtAlignment == 'R')
        (state->nRed)--;
      else
        (state->nBlue)--;
      *lastSelected = board[boardY][boardX].district[DISTRICTSIZE - 2];
    }
  }
  return nSelected;
}

// Unselects the clicked tile and removes it from the district array of other
// selected tiles
void updateDistrictR(GameTile *tile, int x, int y) {
  int i, hasRemoved = 0;

  for (i = 0; i < DISTRICTSIZE - 1 && !hasRemoved; i++)
    if (tile->district[i].x == x && tile->district[i].y == y) {
      rmTile(tile->district, DISTRICTSIZE - 1, i);
      hasRemoved = 1;
    }
}

// Checks if the clicked tile is neighboring selected tiles
int checkIfAdjacent(Tile disTiles[], Tile lastPlaced, int nSelected, int x,
                    int y) {
  int i, j;

  for (i = -1; i < 2; i += 2) {
    if ((x == lastPlaced.x && y == lastPlaced.y + i) ||
        (y == lastPlaced.y && x == lastPlaced.x + i))
      return 1;
  }

  for (i = 0; i < nSelected - 1; i++)
    for (j = -1; j < 2; j += 2)
      if ((x == disTiles[i].x && y == disTiles[i].y + j) ||
          (y == disTiles[i].y && x == disTiles[i].x + j))
        return 1;

  return 0;
}

// Adds a selected tile to a WIP district. Makes sure every already selected
// tile receives the new tile's coordinates in the district array
void updateDistrictA(GameTile board[][GRIDMAX], Tile lastPlaced, int nPlaced,
                     int x, int y) {

  int i;

  board[lastPlaced.y][lastPlaced.x].district[nPlaced - 1].x = x;
  board[lastPlaced.y][lastPlaced.x].district[nPlaced - 1].y = y;

  board[y][x].district[0].x = lastPlaced.x;
  board[y][x].district[0].y = lastPlaced.y;

  for (i = 0; i < nPlaced - 1; i++) {
    board[board[lastPlaced.y][lastPlaced.x].district[i].y]
         [board[lastPlaced.y][lastPlaced.x].district[i].x]
             .district[nPlaced - 1]
             .x = x;
    board[board[lastPlaced.y][lastPlaced.x].district[i].y]
         [board[lastPlaced.y][lastPlaced.x].district[i].x]
             .district[nPlaced - 1]
             .y = y;
  }

  for (i = 1; i < nPlaced; i++)
    board[y][x].district[i] = board[lastPlaced.y][lastPlaced.x].district[i - 1];
}

// Finishes a district by changin the state of all the tiles. Counts up the
// majority and adds it to the count
void finishDistrict(GameTile board[][GRIDMAX], int x, int y, GameState *state) {
  int i, nRed = 0, nBlue = 0;

  board[y][x].state = districted;
  if (board[y][x].alignment == 'R')
    nRed++;
  else
    nBlue++;

  for (i = 0; i < state->size - 1; i++) {
    board[board[y][x].district[i].y][board[y][x].district[i].x].state =
        districted;
    if (board[board[y][x].district[i].y][board[y][x].district[i].x].alignment ==
        'R')
      nRed++;
    else
      nBlue++;
  }

  if (nRed > nBlue) {
    board[y][x].districtAlignment = 'R';
    for (i = 0; i < state->size - 1; i++)
      board[board[y][x].district[i].y][board[y][x].district[i].x]
          .districtAlignment = 'R';
    state->nRed++;
  } else {
    board[y][x].districtAlignment = 'B';
    for (i = 0; i < state->size - 1; i++)
      board[board[y][x].district[i].y][board[y][x].district[i].x]
          .districtAlignment = 'B';
    state->nBlue++;
  }
}

// Clears the count area and displays the score
void displayCount(GameState state, int boardSize) {

  char count[16];
  int x = WINSIZE / 2 - 10;
  int y = (WINSIZE - boardSize) / 4;

  gfx_color(0, 0, 0);
  gfx_fill_rectangle(x, y - 50, 50, 50);

  snprintf(count, 16, "%d", state.nRed);
  gfx_color(255, 0, 0);
  gfx_text(x, y, count);
  x += 10;
  snprintf(count, 16, "x");
  gfx_color(255, 255, 255);
  gfx_text(x, y, count);
  x += 10;
  snprintf(count, 16, "%d", state.nBlue);
  gfx_color(0, 0, 255);
  gfx_text(x, y, count);
}

// Displays start screen to allow player to pick a side
GameState startScreen(GameState state) {
  int hasPicked = 0, x, y;
  char cmd;

  gfx_color(255, 255, 255);
  gfx_text(WINSIZE / 2 - 60, 60, "Gerrymandering");
  gfx_text(WINSIZE / 2 - 60, 80, "Choose your allegiance:");

  gfx_color(255, 0, 0);
  gfx_fill_rectangle(WINSIZE / 8, WINSIZE / 2, WINSIZE / 4, WINSIZE / 4);
  gfx_color(0, 0, 255);
  gfx_fill_rectangle(5 * WINSIZE / 8, WINSIZE / 2, WINSIZE / 4, WINSIZE / 4);

  while (!hasPicked) {
    cmd = gfx_wait();
    if (cmd == 1) {
      x = gfx_xpos();
      y = gfx_ypos();
      if (y > WINSIZE / 2 && y < 3 * WINSIZE / 4) {
        if (x > WINSIZE / 8 && x < 3 * WINSIZE / 8) {
          state.alignment = 'R';
          hasPicked = 1;
        }
        if (x > 5 * WINSIZE / 8 && x < 7 * WINSIZE / 8) {
          state.alignment = 'B';
          hasPicked = 1;
        }
      }
    }
  }

  return state;
}

// Checks for when the game should end
int checkCount(GameState state, int nDistricts) {

  gfx_color(0, 0, 0);
  gfx_fill_rectangle(0, 0, 300, 40);

  if (state.nRed + state.nBlue != nDistricts)
    return 0;

  if (state.alignment == 'R') {
    if (state.nRed > nDistricts / 2) {
      gfx_color(255, 255, 0);
      gfx_text(40, 40, "You won!");
      gfx_wait();
      return 1;
    } else if (state.nBlue > nDistricts / 2) {
      gfx_color(255, 0, 0);
      gfx_text(30, 30, "We are losing! Please fix the map.");
    } else {
      if (state.nBlue > nDistricts / 2) {
        gfx_color(255, 255, 0);
        gfx_text(40, 40, "You won!");
        gfx_wait();
        return 1;
      } else if (state.nRed > nDistricts / 2) {
        gfx_color(0, 0, 255);
        gfx_text(40, 40, "We are losing! Please fix the map.");
      }
    }
  }
  return 0;
}
