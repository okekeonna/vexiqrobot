#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


// START IQ MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END IQ MACROS


// Robot configuration code.
inertial BrainInertial = inertial();


// generating and setting random seed
void initializeRandomSeed(){
  wait(100,msec);
  double xAxis = BrainInertial.acceleration(xaxis) * 1000;
  double yAxis = BrainInertial.acceleration(yaxis) * 1000;
  double zAxis = BrainInertial.acceleration(zaxis) * 1000;
  // Combine these values into a single integer
  int seed = int(
    xAxis + yAxis + zAxis
  );
  // Set the seed
  srand(seed); 
}



void vexcodeInit() {

  // Initializing random seed.
  initializeRandomSeed(); 
}

#pragma endregion VEXcode Generated Robot Configuration

//----------------------------------------------------------------------------
//                                                                            
//    Module:       main.cpp                                                  
//    Author:       Brandon Choi · Matthias Six · Onna Okeke · Nathaniel Rayman                                                  
//    Created:      Wednesday November 11 2026                                                    
//    Description:  IQ project                                                
//                                                                            
//----------------------------------------------------------------------------

// Include the IQ Library
#include "iq_cpp.h"

// Allows for easier use of the VEX Library
using namespace vex;

const int EMPTY = 0;
const int HUMAN = 1;
const int ROBOT = 2;

// Creates an empty board
int board[3][3] = {
  { EMPTY, EMPTY, EMPTY }, 
  { EMPTY, EMPTY, EMPTY }, 
  { EMPTY, EMPTY, EMPTY }
};

// Prints the board
void print_board(){
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);

  for(int r = 0; r < 3; r ++){
    for(int c = 0; c < 3; c++){
      int move = board[r][c];
      Brain.Screen.print("%d | ", move);
    }
    Brain.Screen.setCursor(r+2, 1);
  }
}

// Updates the board with a game move
bool update_board(int move, int bx, int by){
  if(board[bx][by] != EMPTY) {
    print_board();

    Brain.Screen.setCursor(5, 1);
    Brain.Screen.print("Invalid input");

    return false;

  }else{
    board[bx][by] = move;
    print_board();

    return true;
  }

}

int check_winnner(){
  // checks for any rows of winners
  for (int r = 0; r < 3; r++) {
    if (board[r][0] != EMPTY &&
        board[r][0] == board[r][1] &&
        board[r][1] == board[r][2]) {

        return board[r][0];
    }
  }

  //checks for any cols of winners
  for (int c = 0; c < 3; c++) {
    if (board[0][c] != EMPTY &&
        board[0][c] == board[1][c] &&
        board[1][c] == board[2][c]) {

        return board[0][c];
    }
  }

  //checks for first diagonal of winners
  if (board[0][0] != EMPTY &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {

        return board[0][0];
  }
  //checks for any other diagonal of winners
  if (board[2][1] != EMPTY &&
        board[2][1] == board[1][1] &&
        board[1][1] == board[0][2]) {

        return board[2][1];
  }

  for (int r = 0; r < 3; r++){
    for(int c = 0; c < 3; c++){
      if(board[r][c] == EMPTY){
        return -1;
      }
    }
  }

  return 0;
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // Begin project code
  print_board();

  wait(2, seconds);
  update_board(HUMAN, 1, 2);
  wait(2, seconds);
  update_board(ROBOT, 0, 2);
  wait(2, seconds);
  update_board(HUMAN, 0, 2);
    
}
