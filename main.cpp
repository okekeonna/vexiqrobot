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
// Z depths, measured down from the top (z homes there), 48 degrees per cm
const int DEGREES_TO_LOWER_SENSOR = 149; // scanning: 3.1 cm down (2 cm saw green)
const int DEGREES_TO_LOWER_PEN = DEGREES_TO_LOWER_SENSOR + 48; // drawing: 1 cm lower than scanning (4.1 cm)

// Colour sensing, measured with hue_test: paper 304-321, green 134-151, purple 256-290
const int GREEN_HUE_MIN = 60; // red can wrap around to 0-20, so keep it out of green
const int GREEN_HUE_MAX = 250; // under 250 is green, paper and red read 300+
const int PURPLE_HUE_MIN = 250;
const int PURPLE_HUE_MAX = 295;
const int PURPLE_READINGS = 15; // purple readings in a row needed to count as purple (0.3 s)
const int STROKE_DEGREES = 95; // length of each plus line, 2 cm (measured: 525 degrees drew 11 cm, about 48 per cm)

// Grid: 10 x 10 cm, 3 x 3 cells. Measured: x 48 degrees per cm, y 50 degrees per cm
const int X_CELL_DEGREES = 160; // 3.33 cm, grid is 480 wide
const int Y_CELL_DEGREES = 167; // 3.33 cm, grid is 501 tall

// Cell centres in degrees from home (x = away from bumper, y = away from datum)
//   1: (80, 83)    2: (240, 83)    3: (400, 83)
//   4: (80, 250)   5: (240, 250)   6: (400, 250)
//   7: (80, 417)   8: (240, 417)   9: (400, 417)

// Small grid: 6.6 x 6.6 cm, 3 x 3 cells of 2.2 cm, so the sensor can reach every cell
const int SMALL_X_CELL = 106; // 2.2 cm
const int SMALL_Y_CELL = 110; // 2.2 cm
const int SENSOR_Y_OFFSET = 192; // sensor is 3.83 cm ahead of the pen in +y (measured: 3.33 was 0.5 cm short)
const int SENSOR_X_OFFSET = -48; // sensor is 1 cm toward the bumper from the pen (flip to 48 if it reads the wrong way)
const int GRID_Y_START = 167; // grid starts one sensor offset up so the sensor can reach the first row
const int GRID_X_START = 103; // centres the 318 wide grid in the 525 of x travel

// Small grid cell centres (pen position to draw there)
//   x: 156, 262, 368       y: 222, 332, 442
// Pen position to read a cell = centre - sensor offset
//   x: 204, 310, 416       y: 30, 140, 250
const int GREEN_READINGS = 1; // green readings needed to count as green
const int SWEEP_DEGREES = 30; // reads 3 spots across each cell: centre and 0.6 cm either side
const int BACKLASH_DEGREES = 20; // overshoot before reading so every read is approached from the same side

// Motor speeds
const int X_SPEED = 30;
const int Y_SPEED = 30;
const int PEN_SPEED = 20; // slow so the pen stops close to its target
const int SCAN_SPEED = 15; // slow so the sensor doesn't skip over a mark
const int SETTLE_MSEC = 300; // pause after every move so the robot is still before the next one

// Finding the paper: lower fast to just above it, then creep down until the pen touches
const int PAPER_SEARCH_START = DEGREES_TO_LOWER_SENSOR - 48; // fast down to here, 1 cm above the scan depth
const int PAPER_SEARCH_MAX = DEGREES_TO_LOWER_PEN + 96; // never go more than 2 cm below the old draw depth
const int CONTACT_SPEED = 10; // slow creep while looking for the paper
const double CONTACT_CURRENT = 0.1; // amps, very low so it stops at the first bit of resistance
const int SENSOR_ABOVE_PAPER = 48; // scan 1 cm above where the pen touched
const int PEN_BACKOFF_DEGREES = 10; // after touching, lift 0.2 cm so the pen doesn't press too deep

// Homing: drive slowly into the end of travel and stop when the motor stalls
const int HOMING_SPEED = 10; // very slow so hitting the end is gentle
const double HOMING_CURRENT = 0.2; // amps, above this the motor is pushing against the end
const int HOMING_TIMEOUT_MSEC = 5000; // give up and stop after 5 s no matter what
const int Y_HOME_OFFSET = 30; // degrees from the y end stop to the y datum (2 teeth), measure and update


// Creates an empty board
int board[3][3] = {
  { EMPTY, EMPTY, EMPTY }, 
  { EMPTY, EMPTY, EMPTY }, 
  { EMPTY, EMPTY, EMPTY }
};

// Where the pen is right now, in degrees from home
int paper_depth = DEGREES_TO_LOWER_PEN; // updated every time the pen finds the paper
int sensor_depth = DEGREES_TO_LOWER_SENSOR; // updated from paper_depth
int current_x = 0;
int current_y = 0;

// Green cells found by scan_grid, [row][col], row 0 is nearest the y datum
bool green_cells[3][3];

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

// spins a motor a set number of degrees, returns how far it really went (including overshoot)
int move_motor(motor &motor, directionType direction, int angle, int speed){
  motor.resetPosition(); // reset position
  motor.spin(direction, speed, percent); // start spinning

  if (direction == forward) {
    while (motor.position(degrees) < angle) {}
  } else {
    while (motor.position(degrees) > -angle) {}
  }

  motor.stop(brake); // brake so it doesn't coast past
  wait(SETTLE_MSEC, msec); // let it settle

  int moved = motor.position(degrees);
  if (moved < 0) {
    moved = -moved;
  }
  return moved;
}

// spins both y motors together a set number of degrees, returns how far it really went
int move_y(directionType direction, int angle, int speed){
  y_motor.resetPosition(); // reset positions
  y_motor2.resetPosition();

  y_motor.spin(direction, speed, percent); // start both spinning
  y_motor2.spin(direction, speed, percent);

  if (direction == forward) {
    while (y_motor.position(degrees) < angle) {}
  } else {
    while (y_motor.position(degrees) > -angle) {}
  }

  y_motor.stop(brake); // brake so it doesn't coast past
  y_motor2.stop(brake);
  wait(SETTLE_MSEC, msec); // let it settle

  int moved = y_motor.position(degrees);
  if (moved < 0) {
    moved = -moved;
  }
  return moved;
}

// lowers the pen onto the paper
void pen_down(){
  move_motor(pen_motor, forward, PAPER_SEARCH_START, PEN_SPEED); // fast to just above the paper (resets z to 0 first)
  pen_motor.spin(forward, CONTACT_SPEED, percent); // creep down
  wait(200, msec); // get moving before checking

  int last = pen_motor.position(degrees);
  wait(50, msec);
  while (pen_motor.position(degrees) - last >= 1 && pen_motor.current(amp) < CONTACT_CURRENT
         && pen_motor.position(degrees) < PAPER_SEARCH_MAX) {
    last = pen_motor.position(degrees);
    wait(50, msec);
  }

  paper_depth = pen_motor.position(degrees); // measured from the top, since move_motor reset z there
  sensor_depth = paper_depth - SENSOR_ABOVE_PAPER;
  printf("paper at %d  current %.2f\n", paper_depth, pen_motor.current(amp));
  pen_motor.stop(brake); // stop at the paper, no extra push
  wait(SETTLE_MSEC, msec);

  // back off a little so the pen isn't pressed too deep
  // (not using move_motor here because it would reset z, and pen_up needs the depth from the top)
  pen_motor.spin(reverse, CONTACT_SPEED, percent);
  while (pen_motor.position(degrees) > paper_depth - PEN_BACKOFF_DEGREES) {}
  pen_motor.stop(brake);
  wait(SETTLE_MSEC, msec);
}

// lifts the pen off the paper
void pen_up(){
  int depth = pen_motor.position(degrees); // how far down the pen really is (reset at the top of pen_down)
  pen_motor.stop(brake);
  move_motor(pen_motor, reverse, depth, PEN_SPEED); // back up by exactly that much
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
  x_motor.stop(brake);
  wait(SETTLE_MSEC, msec);
  x_motor.resetPosition();
}

// checks if a motor has hit the end: pulling high current or barely moving
bool is_stalled(motor &motor, int last_position){
  int change = motor.position(degrees) - last_position;
  if (change < 0) {
    change = -change;
  }
  return change < 1 || motor.current(amp) > HOMING_CURRENT;
}

// lifts z until it stops at the top, z is now at 0 (pen fully up)
void home_z(){
  pen_motor.spin(reverse, HOMING_SPEED, percent); // up
  wait(200, msec); // get moving before checking

  int last = pen_motor.position(degrees);
  int time = 200;
  wait(50, msec);
  while (!is_stalled(pen_motor, last) && time < HOMING_TIMEOUT_MSEC) {
    last = pen_motor.position(degrees);
    wait(50, msec);
    time = time + 50;
  }

  pen_motor.stop(brake);
  wait(SETTLE_MSEC, msec);
  pen_motor.resetPosition();
}

// drives y toward the datum until it stops at the end, then moves out to the datum, y is now at 0
void home_y(){
  y_motor.spin(reverse, HOMING_SPEED, percent); // toward the datum end
  y_motor2.spin(reverse, HOMING_SPEED, percent);
  wait(200, msec); // get moving before checking

  int last = y_motor.position(degrees);
  int last2 = y_motor2.position(degrees);
  int time = 200;
  wait(50, msec);
  // stop as soon as either y motor stalls
  while (!is_stalled(y_motor, last) && !is_stalled(y_motor2, last2) && time < HOMING_TIMEOUT_MSEC) {
    last = y_motor.position(degrees);
    last2 = y_motor2.position(degrees);
    wait(50, msec);
    time = time + 50;
  }

  y_motor.stop(brake);
  y_motor2.stop(brake);
  wait(SETTLE_MSEC, msec);

  move_y(forward, Y_HOME_OFFSET, HOMING_SPEED); // out to the datum
}

// homes every axis: z up first so the pen can't drag, then x, then y
void motor_calibration(){
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Calibrating...");

  home_z();
  home_x();
  home_y();

  current_x = 0;
  current_y = 0;

  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Ready, press TouchLED");
}

// waits for a fresh TouchLED press
void wait_for_press(){
  while(TouchLED3.pressing()){} // wait for release
  while(!TouchLED3.pressing()){} // wait for press
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


// test: scans along x away from the bumper and stops when it sees purple
// logs x position and hue to the brain screen and the VS Code terminal
void scan_color_test(){
  reset_game(); // press TouchLED, home x
  pen_down(); // lower z

  Optical1.setLight(ledState::on); // turn on the sensor light
  wait(500, msec);

  x_motor.resetPosition();
  x_motor.spin(reverse, SCAN_SPEED, percent); // start scanning

  bool found_purple = false;
  int purple_count = 0; // purple readings in a row

  while (!found_purple && x_motor.position(degrees) > -3 * X_CELL_DEGREES) {
    int x = -x_motor.position(degrees);
    int hue = Optical1.hue();

    printf("x: %d  hue: %d\n", x, hue); // log to the VS Code terminal

    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("x: %d hue: %d   ", x, hue);

    if (hue >= PURPLE_HUE_MIN && hue <= PURPLE_HUE_MAX) {
      purple_count = purple_count + 1;
      if (purple_count >= PURPLE_READINGS) {
        found_purple = true;
      }
    } else {
      purple_count = 0;
    }

    if (hue >= GREEN_HUE_MIN && hue <= GREEN_HUE_MAX) {
      Brain.Screen.setCursor(3, 1);
      Brain.Screen.print("green seen at x: %d   ", x);
    }

    wait(20, msec);
  }

  x_motor.stop();

  Brain.Screen.setCursor(1, 1);
  if (found_purple) {
    Brain.Screen.print("purple detected");
    printf("purple detected\n");
  } else {
    Brain.Screen.print("white detected");
    printf("white detected\n");
  }

  pen_up(); // raise z
}

// test: scans the full grid width along x and only shows the hue, never stops early
void hue_test(){
  reset_game(); // press TouchLED, home x
  pen_down(); // lower z

  Optical1.setLight(ledState::on); // turn on the sensor light
  wait(500, msec);

  x_motor.resetPosition();
  x_motor.spin(reverse, SCAN_SPEED, percent); // start scanning

  while (x_motor.position(degrees) > -3 * X_CELL_DEGREES) {
    int x = -x_motor.position(degrees);
    int hue = Optical1.hue();

    printf("x: %d  hue: %d\n", x, hue); // log to the VS Code terminal

    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("x: %d   ", x);
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("hue: %d   ", hue);

    wait(20, msec);
  }

  x_motor.stop();
  pen_up(); // raise z

  printf("done\n"); // end of log
}

// moves the pen to a position in degrees from home (x first, then y)
// uses how far each motor really went, so overshoot gets corrected on the next move
void go_to(int x, int y){
  if (x > current_x) {
    current_x = current_x + move_motor(x_motor, reverse, x - current_x, X_SPEED); // away from bumper
  } else if (x < current_x) {
    current_x = current_x - move_motor(x_motor, forward, current_x - x, X_SPEED); // toward bumper
  }

  if (y > current_y) {
    current_y = current_y + move_y(forward, y - current_y, Y_SPEED);
  } else if (y < current_y) {
    current_y = current_y - move_y(reverse, current_y - y, Y_SPEED);
  }
}

// draws the 6.6 x 6.6 cm grid (two lines each way), run motor_calibration first
void draw_small_grid(){
  int x_end = GRID_X_START + 3 * SMALL_X_CELL;
  int y_end = GRID_Y_START + 3 * SMALL_Y_CELL;

  // first line along x
  go_to(GRID_X_START, GRID_Y_START + SMALL_Y_CELL);
  pen_down();
  go_to(x_end, GRID_Y_START + SMALL_Y_CELL);
  pen_up();

  // second line along x
  go_to(x_end, GRID_Y_START + 2 * SMALL_Y_CELL);
  pen_down();
  go_to(GRID_X_START, GRID_Y_START + 2 * SMALL_Y_CELL);
  pen_up();

  // first line along y
  go_to(GRID_X_START + SMALL_X_CELL, GRID_Y_START);
  pen_down();
  go_to(GRID_X_START + SMALL_X_CELL, y_end);
  pen_up();

  // second line along y
  go_to(GRID_X_START + 2 * SMALL_X_CELL, y_end);
  pen_down();
  go_to(GRID_X_START + 2 * SMALL_X_CELL, GRID_Y_START);
  pen_up();
}

// reads one cell: lowers z, takes 10 readings, returns true if mostly green
bool read_cell(int row, int col){
  int x = GRID_X_START + col * SMALL_X_CELL + SMALL_X_CELL / 2 - SENSOR_X_OFFSET;
  int y = GRID_Y_START + row * SMALL_Y_CELL + SMALL_Y_CELL / 2 - SENSOR_Y_OFFSET;

  // start before the first spot so x and y always arrive moving the same way (removes gear slack)
  go_to(x - SWEEP_DEGREES - BACKLASH_DEGREES, y - BACKLASH_DEGREES);
  go_to(x - SWEEP_DEGREES, y);

  move_motor(pen_motor, forward, sensor_depth, PEN_SPEED); // lower z to reading height
  wait(200, msec);

  int green_count = 0;

  // read 3 spots across the cell, always moving the same way
  for (int spot = 0; spot < 3; spot++) {
    go_to(x - SWEEP_DEGREES + spot * SWEEP_DEGREES, y);

    for (int i = 0; i < 10; i++) {
      int hue = Optical1.hue();
      printf("row %d col %d spot %d  hue: %d\n", row, col, spot, hue); // log

      if (hue >= GREEN_HUE_MIN && hue <= GREEN_HUE_MAX) {
        green_count = green_count + 1;
      }
      wait(50, msec);
    }
  }

  move_motor(pen_motor, reverse, sensor_depth, PEN_SPEED); // raise z

  return green_count >= GREEN_READINGS;
}

// scans all 9 cells in a snake path and shows the results on the brain
void scan_grid(){
  Optical1.setLight(ledState::on); // turn on the sensor light

  for (int row = 0; row < 3; row++) {
    for (int i = 0; i < 3; i++) {
      int col = i;
      if (row == 1) {
        col = 2 - i; // snake: go back the other way on the middle row
      }
      green_cells[row][col] = read_cell(row, col);
    }
  }

  // show results, top line is the row nearest the y datum
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Green scan:");

  for (int row = 0; row < 3; row++) {
    Brain.Screen.setCursor(row + 2, 1);
    for (int col = 0; col < 3; col++) {
      if (green_cells[row][col]) {
        Brain.Screen.print("G ");
        printf("G ");
      } else {
        Brain.Screen.print(". ");
        printf(". ");
      }
    }
    printf("\n");
  }

  printf("done\n"); // end of log
}

// test: lowers z to reading height, drives y forward and stops on green or near the paper edge
// logs y position and hue to the brain screen and the VS Code terminal
void y_green_scan_test(){
  Optical1.setLight(ledState::on); // turn on the sensor light
  move_motor(pen_motor, forward, sensor_depth, PEN_SPEED); // lower z to reading height
  wait(500, msec);

  int start_y = current_y;
  int y_limit = 480 - start_y; // stop before the end of y travel (500)

  y_motor.resetPosition();
  y_motor2.resetPosition();
  y_motor.spin(forward, SCAN_SPEED, percent); // start scanning
  y_motor2.spin(forward, SCAN_SPEED, percent);

  bool found_green = false;
  int green_count = 0; // green readings in a row

  while (!found_green && y_motor.position(degrees) < y_limit) {
    int y = start_y + y_motor.position(degrees);
    int hue = Optical1.hue();

    printf("y: %d  hue: %d\n", y, hue); // log to the VS Code terminal

    Brain.Screen.setCursor(2, 1);
    Brain.Screen.print("y: %d hue: %d   ", y, hue);

    if (hue >= GREEN_HUE_MIN && hue <= GREEN_HUE_MAX) {
      green_count = green_count + 1;
      if (green_count >= GREEN_READINGS) {
        found_green = true;
      }
    } else {
      green_count = 0;
    }

    wait(20, msec);
  }

  y_motor.stop(brake);
  y_motor2.stop(brake);
  wait(SETTLE_MSEC, msec);
  current_y = start_y + y_motor.position(degrees);

  Brain.Screen.setCursor(1, 1);
  if (found_green) {
    Brain.Screen.print("green detected  ");
    printf("green detected at y: %d\n", current_y);
  } else {
    Brain.Screen.print("no green, edge  ");
    printf("no green, reached y: %d\n", current_y);
  }

  move_motor(pen_motor, reverse, sensor_depth, PEN_SPEED); // raise z
  printf("done\n"); // end of log
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

  motor_calibration(); // home z, x and y before anything moves

  // grid scan test
  wait_for_press(); // press 1: draw grid
  draw_small_grid();
  go_to(0, 0); // out of the way while cells are coloured
  wait_for_press(); // press 2: start scan
  scan_grid();
  go_to(0, 0); // back to home
  home_x();

  // y green scan test
  // wait_for_press(); // press: start scanning along y
  // y_green_scan_test();

  // scan_color_test();

  // hue_test();

  // draw_grid();
}
