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
motor y_motor2 = motor(PORT11, true); // mirror of y_motor, reversed so both push the same way


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

// Allows for easier use of the VEX Library
using namespace vex;

const int EMPTY = 0;
const int HUMAN = 1;
const int ROBOT = 2;


// 24 tooth gear on a rack: 1 cm = 45 degrees. There are a total of 35 teeth that the x motor runs on. Honing resets at 0. 
const int DEGREES_TO_LOWER_PEN = 135; // degrees to lower the pen
const int STROKE_DEGREES = 95; // length of each plus line, 2 cm (measured: 525 degrees drew 11 cm, about 48 per cm)

// Grid: 10 x 10 cm, 3 x 3 cells. Measured: x 48 degrees per cm, y 50 degrees per cm
const int X_CELL_DEGREES = 160; // 3.33 cm, grid is 480 wide
const int Y_CELL_DEGREES = 167; // 3.33 cm, grid is 501 tall

// Cell centres in degrees from home (x = away from bumper, y = away from datum)
//   1: (80, 83)    2: (240, 83)    3: (400, 83)
//   4: (80, 250)   5: (240, 250)   6: (400, 250)
//   7: (80, 417)   8: (240, 417)   9: (400, 417)

// Motor speeds
const int X_SPEED = 100;
const int Y_SPEED = 100;
const int PEN_SPEED = 30; // slow so the pen stops close to its target


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

  if (myhue <= 150) {
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

// spins a motor a set number of degrees
void move_motor(motor &motor, directionType direction, int angle, int speed){
  motor.resetPosition(); // reset position
  motor.spin(direction, speed, percent); // start spinning

  if (direction == forward) {
    while (motor.position(degrees) < angle) {}
  } else {
    while (motor.position(degrees) > -angle) {}
  }

  motor.stop();
}

// spins both y motors together a set number of degrees
void move_y(directionType direction, int angle, int speed){
  y_motor.resetPosition(); // reset positions
  y_motor2.resetPosition();

  y_motor.spin(direction, speed, percent); // start both spinning
  y_motor2.spin(direction, speed, percent);

  if (direction == forward) {
    while (y_motor.position(degrees) < angle) {}
  } else {
    while (y_motor.position(degrees) > -angle) {}
  }

  y_motor.stop();
  y_motor2.stop();
}

// lowers the pen onto the paper
void pen_down(){
  move_motor(pen_motor, forward, DEGREES_TO_LOWER_PEN, PEN_SPEED);
}

// lifts the pen off the paper
void pen_up(){
  move_motor(pen_motor, reverse, DEGREES_TO_LOWER_PEN, PEN_SPEED);
}

// draws a plus centred where the pen is
void draw_plus(){
  int half = STROKE_DEGREES / 2;

  // x line
  move_motor(x_motor, reverse, half, X_SPEED); // go to start of line
  pen_down();
  move_motor(x_motor, forward, STROKE_DEGREES, X_SPEED); // draw line
  pen_up();
  move_motor(x_motor, reverse, half, X_SPEED); // back to centre

  // y line
  move_y(reverse, half, Y_SPEED); // go to start of line
  pen_down();
  move_y(forward, STROKE_DEGREES, Y_SPEED); // draw line
  pen_up();
  move_y(reverse, half, Y_SPEED); // back to centre
}

// drives x into the bumper, x is now at 0
void home_x(){
  x_motor.spin(forward, X_SPEED, percent); // home x against the bumper
  while(!Bumper2.pressing()){}
  x_motor.stop();
}

// reset the game // Matthias function
void reset_game(){

  while(!TouchLED3.pressing()){};

  home_x();
}

// draws the 3 x 3 grid (two lines each way)
// y must already be at its datum (0)
void draw_grid(){
  reset_game(); // press TouchLED, home x. pen is now at (0, 0)

  // first line along x, at y = 167, starting right at the bumper
  move_y(forward, Y_CELL_DEGREES, Y_SPEED); // now at (0, 167)
  pen_down();
  move_motor(x_motor, reverse, 3 * X_CELL_DEGREES, X_SPEED); // now at (480, 167)
  pen_up();

  // second line along x, at y = 334
  move_y(forward, Y_CELL_DEGREES, Y_SPEED); // now at (480, 334)
  pen_down();
  move_motor(x_motor, forward, 3 * X_CELL_DEGREES, X_SPEED); // now at (0, 334)
  pen_up();

  // first line along y, at x = 160
  move_y(reverse, 2 * Y_CELL_DEGREES, Y_SPEED); // now at (0, 0)
  move_motor(x_motor, reverse, X_CELL_DEGREES, X_SPEED); // now at (160, 0)
  pen_down();
  move_y(forward, 3 * Y_CELL_DEGREES, Y_SPEED); // now at (160, 501)
  pen_up();

  // second line along y, at x = 320
  move_motor(x_motor, reverse, X_CELL_DEGREES, X_SPEED); // now at (320, 501)
  pen_down();
  move_y(reverse, 3 * Y_CELL_DEGREES, Y_SPEED); // now at (320, 0)
  pen_up();

  // back to home
  home_x(); // now at (0, 0)
}


void read_mark(int x, int y){
  wait(2, seconds);
  // read color
  int mark = check_color();

  Brain.Screen.setCursor(5, 1);
  Brain.Screen.print("%d ", mark);

}

void setMotors(){
  x_motor.setVelocity(X_SPEED, percent);
  y_motor.setVelocity(Y_SPEED, percent);
  y_motor2.setVelocity(Y_SPEED, percent);
  pen_motor.setVelocity(PEN_SPEED, percent);
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  setMotors();

  draw_grid();
}
