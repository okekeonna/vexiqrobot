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
optical Optical1 = optical(PORT1);
motor x_motor = motor(PORT6, false);
bumper Bumper2 = bumper(PORT2);
touchled TouchLED3 = touchled(PORT3);
motor pen_motor = motor(PORT4, false);
motor y_motor = motor(PORT5, false);


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

// Converts a color to a string
const char* convertColorToString(color col) {
  if (col == colorType::red) return "red";
  else if (col == colorType::green) return "green";
  else if (col == colorType::blue) return "blue";
  else if (col == colorType::white) return "white";
  else if (col == colorType::yellow) return "yellow";
  else if (col == colorType::orange) return "orange";
  else if (col == colorType::purple) return "purple";
  else if (col == colorType::cyan) return "cyan";
  else if (col == colorType::black) return "black";
  else if (col == colorType::transparent) return "transparent";
  else if (col == colorType::red_violet) return "red_violet";
  else if (col == colorType::violet) return "violet";
  else if (col == colorType::blue_violet) return "blue_violet";
  else if (col == colorType::blue_green) return "blue_green";
  else if (col == colorType::yellow_green) return "yellow_green";
  else if (col == colorType::yellow_orange) return "yellow_orange";
  else if (col == colorType::red_orange) return "red_orange";
  else if (col == colorType::none) return "none";
  else return "unknown";
}


// Convert colorType to string
const char* convertColorToString(colorType col) {
  if (col == colorType::red) return "red";
  else if (col == colorType::green) return "green";
  else if (col == colorType::blue) return "blue";
  else if (col == colorType::white) return "white";
  else if (col == colorType::yellow) return "yellow";
  else if (col == colorType::orange) return "orange";
  else if (col == colorType::purple) return "purple";
  else if (col == colorType::cyan) return "cyan";
  else if (col == colorType::black) return "black";
  else if (col == colorType::transparent) return "transparent";
  else if (col == colorType::red_violet) return "red_violet";
  else if (col == colorType::violet) return "violet";
  else if (col == colorType::blue_violet) return "blue_violet";
  else if (col == colorType::blue_green) return "blue_green";
  else if (col == colorType::yellow_green) return "yellow_green";
  else if (col == colorType::yellow_orange) return "yellow_orange";
  else if (col == colorType::red_orange) return "red_orange";
  else if (col == colorType::none) return "none";
  else return "unknown";
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
  if (board[2][0] != EMPTY &&
        board[2][0] == board[1][1] &&
        board[1][1] == board[0][2]) {

        return board[2][0];
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

int check_color(){

  int myhue = Optical1.hue();
  int move = 0;

  Brain.Screen.setCursor(6, 1);
  wait(1, seconds);
  Brain.Screen.clearScreen();

  if (myhue < 150) {
    Brain.Screen.print("Green");
    move = 2;
  } else if (myhue > 150 && myhue < 330){
    Brain.Screen.print("Purple");
    move = 1;
  } else {
    Brain.Screen.print("White");
    move = 0;
  }

  return move;
}

//Minimax algorithm 
int minimax(int depth, bool isMaximizing) {
    int result = check_winnner();

    // Base cases: game has ended
    if (result == ROBOT) return 10 - depth;
    if (result == HUMAN) return depth - 10;
    if (result == 0) return 0;

    if (isMaximizing) {
        int bestScore = -1000;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (board[r][c] == EMPTY) {

                    // Simulate robot move
                    board[r][c] = ROBOT;

                    // Evaluate human's response
                    int score = minimax(depth + 1, false);

                    // Undo simulated move
                    board[r][c] = EMPTY;

                    if (score > bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;

    } else {
        int bestScore = 1000;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (board[r][c] == EMPTY) {

                    // Simulate human move
                    board[r][c] = HUMAN;

                    // Evaluate robot's response
                    int score = minimax(depth + 1, true);

                    // Undo simulated move
                    board[r][c] = EMPTY;

                    if (score < bestScore) {
                        bestScore = score;
                    }
                }
            }
        }

        return bestScore;
    }
}

// Find best move
void find_best_move(int &bestRow, int &bestCol) {
    int bestScore = -1000;

    bestRow = -1;
    bestCol = -1;

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (board[r][c] == EMPTY) {

                // Try placing the robot's piece
                board[r][c] = ROBOT;

                // Evaluate the move
                int score = minimax(0, false);

                // Undo the move
                board[r][c] = EMPTY;

                if (score > bestScore) {
                    bestScore = score;
                    bestRow = r;
                    bestCol = c;
                }
            }
        }
    }
}

// reset the game // Matthias function
void reset_game(){

  while(!TouchLED3.pressing()){};
  x_motor.setMaxTorque(100, percent);
  x_motor.spin(forward, 60, percent);
  while(!Bumper2.pressing()){}
  x_motor.stop();
  
}

// make the mark // Matthias function
void make_mark(){
  pen_motor.setVelocity(70, percent);
  pen_motor.spinFor(forward, 140, degrees);
  pen_motor.spinFor(reverse, 140, degrees);
}

void move_to_mark(int x, int y){

  int xspin = x*160;
  int yspin = y*160;

  x_motor.spinFor(reverse, xspin, degrees);
  y_motor.spinFor(forward, yspin, degrees);
  make_mark();

  x_motor.spinFor(forward, xspin, degrees);
  y_motor.spinFor(reverse, yspin, degrees);

  x_motor.stop();
  y_motor.stop();

}

void read_mark(int x, int y){
  // go to the mark
  move_to_mark(x, y);
  
  wait(2, seconds);
  // read color
  int mark = check_color();

  Brain.Screen.setCursor(5, 1);
  Brain.Screen.print("%d ", mark);

}

void make_dot_grid(){
  // makes three x three dots

  x_motor.spinFor(forward, 160, degrees);
  make_mark();
  x_motor.spinFor(forward, 160, degrees);
  make_mark();
  x_motor.spinFor(forward, 160, degrees);
  make_mark();

  y_motor.spinFor(forward, 160, degrees);
  make_mark();
  x_motor.spinFor(reverse, 160, degrees);
  make_mark();
  x_motor.spinFor(reverse, 160, degrees);
  make_mark();
  
  y_motor.spinFor(forward, 160, degrees);
  make_mark();
  x_motor.spinFor(forward, 160, degrees);
  make_mark();
  x_motor.spinFor(forward, 160, degrees);
  make_mark();
}

/*
void checkGrid(){
  for(int r = 0; r < 3; r++){
    for(int c = 0; c < 3; c++){
      if(board[r][c] == EMPTY){
        move_to_mark(x, y);
        check_color();
      }
    }
  }
}
*/

void setMotors(){
  x_motor.setVelocity(60, percent);
  y_motor.setVelocity(50, percent);
  pen_motor.setVelocity(70, percent);

  x_motor.setMaxTorque(100, percent);
  y_motor.setMaxTorque(100, percent);
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  setMotors();

  reset_game();

  //make_dot_grid
  //set one place to 0
  //then check if the thing there is 0

  //move
  //y_motor.setVelocity(20, percent);
  //y_motor.spin(reverse);


  // // Human plays two moves
  // update_board(HUMAN, 1, 1);
  // update_board(HUMAN, 0, 1);

  // // Robot already has one move
  // update_board(ROBOT, 0, 0);

  // wait(2, seconds);

  // // Ask minimax to find the best move
  // int bestRow, bestCol;
  // find_best_move(bestRow, bestCol);

  // if (bestRow != -1) {
  //     update_board(ROBOT, bestRow, bestCol);
  // }

  // Brain.Screen.setCursor(5, 1);
  // Brain.Screen.print("Robot: (%d,%d)", bestRow, bestCol);

  // while (true) {
  //     wait(100, msec);
  // }

  //make_dot_grid();

  //moves forward to test
  //x_motor.spinFor(forward, 160, degrees);
  //make_mark();

  move_to_mark(0, 0);
  move_to_mark(2, 2);
  move_to_mark(1, 1);

  //multiplied by the column number

  // // Begin project code
  // print_board();

  // wait(2, seconds);
  // update_board(ROBOT, 1, 2);
  // wait(2, seconds);
  // update_board(ROBOT, 0, 2);
  // wait(2, seconds);
  // update_board(ROBOT, 2, 2);

  // Brain.Screen.setCursor(6, 1);
  // int winner = check_winnner();
  // Brain.Screen.print("%d ", winner);

  // Optical8.setLight(ledState::on);
  // color mycolor = Optical8.color();
  // Brain.Screen.setCursor(6, 1);

  // while(true){
  //   check_color();
  // }


  

}
