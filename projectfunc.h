/*
 * CSE 20311
 * projectfunc.h
 * Author: Caio Kwasnicki
 */

#include "gfx2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GRIDMAX 8
#define DISTRICTSIZE 3
#define NEIGHBORSMAX 4
#define BUFFERMAX GRIDMAX * 4
#define WINSIZE 800

typedef enum { standby, selected, districted } tileState;

typedef struct {
  short x, y;
} Tile;

typedef struct {
  short size, nRed, nBlue;
  char alignment;
} GameState;

typedef struct {
  Tile arr[BUFFERMAX];
  short size;
} TileStack;

typedef struct {
  short r, g, b;
} Color;

typedef struct {
  short isOccupied;
  tileState state;
  char alignment, districtAlignment;
  Tile district[DISTRICTSIZE - 1];
} GameTile;

int getRandInt(int min, int max);
char getRandAlignment();
void initializeGrid(char grid[][GRIDMAX]);
int placeDistrict(char grid[][GRIDMAX], int x, int y, GameState state,
                  char majority);
int findNeighbors(char grid[][GRIDMAX], Tile neighbors[], int x, int y);
int rmTile(Tile arr[], int size, int index);
int findDistrictNeighbors(char grid[][GRIDMAX], Tile initialTile,
                          Tile neighbors[]);
void generateDistricts(char grid[][GRIDMAX], GameState state, int nDistricts);
void drawSquare(int x, int y, int halfSide);
void drawSquareBackground(int x, int y, int boardSize, Color bgColor);
void convertToGameBoard(char grid[][GRIDMAX], GameTile board[][GRIDMAX]);
void renderBoard(GameTile board[][GRIDMAX], int boardSize);
int handleClick(GameTile board[][GRIDMAX], int x, int y, int boardSize,
                int nSelected, Tile *lastSelected, GameState *state);
void updateDistrictR(GameTile *tile, int x, int y);
int checkIfAdjacent(Tile disTiles[], Tile lastPlaced, int nSelected, int x,
                    int y);
void updateDistrictA(GameTile board[][GRIDMAX], Tile lastPlaced, int nPlaced,
                     int x, int y);
void finishDistrict(GameTile board[][GRIDMAX], int x, int y, GameState *state);
void displayCount(GameState state, int boardSize);
GameState startScreen(GameState state);
int checkCount(GameState state, int nDistricts);
int checkIfMiddle(GameTile tile);
