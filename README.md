# Gerrymandering Game
### About

This game was developed as the final project for the CSE 20311 - Fundamentals of Computing class. It was inspired by other gerrymandering games that intended to expose how anti-democratic this practice is. It runs using a graphic library provided in class dependent on the X window system. 

### Usage
To build this software in any Unix-like operating system, clone this repository:

    git clone https://github.com/cbidinot/gerrymandering-game
   Then build the project using Make (requires X11):
   

    make
   To run the game, execute the binary:
   

    ./gerrymandering

### Game Instructions

The game's objective is to draw electoral district lines so that the population minority manages to win the majority of districts. You choose the color you work for at the beginning of the game. You start a district by clicking a square tile and expand it by clicking adjacent tiles. You may click a tile in a complete district to remove it and edit that district. All districts consist of 3 tiles, and at the beginning of the game the map will contain 5 randomly generated districts. As the game progresses, the map will expand, making it more complicated. To quit the game at any point, simply press 'q'.
