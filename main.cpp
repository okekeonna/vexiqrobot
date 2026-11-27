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
motor y_motor2 = motor(PORT11, true);


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
const int DEGREES_TO_LOWER_SENSOR = 30; // scanning: 3.1 cm down (2 cm saw green)
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
const int SENSOR_X_OFFSET = 0; // sensor is 1 cm toward the bumper from the pen (flip to 48 if it reads the wrong way)
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
const int PAPER_SEARCH_MAX = DEGREES_TO_LOWER_PEN + 140; // never go more than 2 cm below the old draw depth
const int CONTACT_SPEED = 10; // slow creep while looking for the paper
const double CONTACT_CURRENT = 1; // amps, very low so it stops at the first bit of resistance
const int SENSOR_ABOVE_PAPER = 48; // scan 1 cm above where the pen touched
const int PEN_BACKOFF_DEGREES = 10; // after touching, lift 0.2 cm so the pen doesn't press too deep
const int DAB_FROM_SENSOR_X = 0;  // 48 degrees per cm
const int DAB_FROM_SENSOR_Y = 170; // 50 degrees per cm

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

// Updates the board with a game move
// Author: Onna Okeke
bool update_board(int move, int bx, int by){
  if(board[bx][by] != EMPTY) {

    Brain.Screen.setCursor(5, 1);
    Brain.Screen.print("Invalid input");

    return false;

  }else{
    board[bx][by] = move;

    return true;
  }

}

// Author: Onna Okeke
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

// Author: Onna Okeke
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
// Author: Onna Okeke
// External Source: 
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
// Author: Onna Okeke
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
// Author: Brandon Choi
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
// Author: Brandon Choi
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
// Author: Matthias Six
void pen_down(){
  move_motor(pen_motor, forward, PAPER_SEARCH_START, PEN_SPEED); // fast to just above the paper (resets z to 0 first)
  pen_motor.spin(forward, CONTACT_SPEED, percent); // creep down
  wait(200, msec); // get moving before checking

  int last = pen_motor.position(degrees);
  wait(50, msec);
  while (pen_motor.current(amp) < CONTACT_CURRENT
         && pen_motor.position(degrees) < PAPER_SEARCH_MAX) {
    //last = pen_motor.position(degrees);
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
// Author: Matthias Six
void pen_up(){
  int depth = pen_motor.position(degrees); // how far down the pen really is (reset at the top of pen_down)
  pen_motor.stop(brake);
  move_motor(pen_motor, reverse, depth, PEN_SPEED); // back up by exactly that much
}

// drives x into the bumper, x is now at 0
// Author: Nathaniel Rayman
void home_x(){
  x_motor.spin(forward, X_SPEED, percent); // home x against the bumper
  while(!Bumper2.pressing()){}
  x_motor.stop(brake);
  wait(SETTLE_MSEC, msec);
  x_motor.resetPosition();
}

// checks if a motor has hit the end: pulling high current or barely moving
// Author: Nathaniel Rayman
bool is_stalled(motor &motor, int last_position){
  int change = motor.position(degrees) - last_position;
  if (change < 0) {
    change = -change;
  }
  return change < 1 || motor.current(amp) > HOMING_CURRENT;
}

// lifts z until it stops at the top, z is now at 0 (pen fully up)
// Author: Nathaniel Rayman
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
// Author: Nathaniel Rayman
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
// Author: Nathaniel Rayman
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
// Author: Matthias Six
void wait_for_press(){
  TouchLED3.setColor(blue);
  TouchLED3.setBrightness(100);
  while(TouchLED3.pressing()){} // wait for release
  while(!TouchLED3.pressing()){} // wait for press
  TouchLED3.setBrightness(0);
}

// reset the game
// Author: Matthias Six
void reset_game(){

  while(!TouchLED3.pressing()){};

  home_x();
}

// moves the pen to a position in degrees from home (x first, then y)
// uses how far each motor really went, so overshoot gets corrected on the next move
// Author: Brandon Choi
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

// reads one cell: lowers z, takes 10 readings, returns true if mostly green
// Author: Brandon Choi
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
// Author: Brandon Choi
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
      if (green_cells[row][2-col]) {
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

// scan status of each cell, only used to draw the grid while scanning
const int NOT_SCANNED = 0;
const int SCANNING = 1;
const int SCANNED_EMPTY = 2;
int scan_mark[3][3] = {
  { NOT_SCANNED, NOT_SCANNED, NOT_SCANNED },
  { NOT_SCANNED, NOT_SCANNED, NOT_SCANNED },
  { NOT_SCANNED, NOT_SCANNED, NOT_SCANNED }
};

// sets every cell back to not scanned
// Author: Brandon Choi
void clear_scan_marks(){
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      scan_mark[r][c] = NOT_SCANNED;
    }
  }
}

// shows the full 3 x 3 grid: G = player (green), R = robot, . = empty
// row 0 (nearest the y datum) is the top line
// shows the full 3 x 3 grid
// G = player, R = robot, ? = reading now, - = read and empty, . = not read yet
// row 0 (nearest the y datum) is the top line
// Author: Onna Okeke
void show_grid(){
  Brain.Screen.clearScreen();
  for (int r = 0; r < 3; r++) {
    Brain.Screen.setCursor(r + 1, 1);
    for (int c = 0; c < 3; c++) {
      if (board[r][2-c] == HUMAN) {
        Brain.Screen.print("G ");
      } else if (board[r][2-c] == ROBOT) {
        Brain.Screen.print("R ");
      } else if (scan_mark[r][2-c] == SCANNING) {
        Brain.Screen.print("? ");
      } else if (scan_mark[r][2-c] == SCANNED_EMPTY) {
        Brain.Screen.print("- ");
      } else {
        Brain.Screen.print(". ");
      }
    }
  }
}

// shows the board with a message underneath
// Author: Onna Okeke
void show_message(const char* msg){
  show_grid();
  Brain.Screen.setCursor(5, 1);
  Brain.Screen.print("%s", msg);
}

// reads one cell the same way read_position_test does: straight to the centre, no sweep
// returns true if the cell has the player's green mark
// shows each reading and the result on the brain screen
// reads one cell: straight to the centre, no sweep
// returns true if the cell has the player's green mark
// reads one cell: straight to the centre, no sweep
// returns true if the cell has the player's green mark
// Author: Brandon Choi
bool read_cell_centre(int row, int col){
  int x = GRID_X_START + col * SMALL_X_CELL + SMALL_X_CELL / 2 - SENSOR_X_OFFSET;
  int y = GRID_Y_START + row * SMALL_Y_CELL + SMALL_Y_CELL / 2 - SENSOR_Y_OFFSET;

  // show this cell as being read
  scan_mark[row][col] = SCANNING;
  show_message("Scanning...");

  // approach from the same side every time
  go_to(x - BACKLASH_DEGREES, y - BACKLASH_DEGREES);
  go_to(x, y);

  move_motor(pen_motor, forward, sensor_depth, PEN_SPEED); // lower z to reading height
  wait(200, msec);

  int green_count = 0;
  for (int i = 0; i < 10; i++) {
    int hue = Optical1.hue();
    printf("row %d col %d  hue: %d\n", row, col, hue); // log to the terminal only
    if (hue >= GREEN_HUE_MIN && hue <= GREEN_HUE_MAX) {
      green_count = green_count + 1;
    }
    wait(50, msec);
  }

  move_motor(pen_motor, reverse, sensor_depth, PEN_SPEED); // raise z

  return green_count >= 5; // at least half the readings green
}

// Author: Brandon Choi
void setMotors(){
  x_motor.setVelocity(X_SPEED, percent);
  y_motor.setVelocity(Y_SPEED, percent);
  y_motor2.setVelocity(Y_SPEED, percent);
  pen_motor.setVelocity(PEN_SPEED, percent);
}



// pen position that puts the sensor over a cell (same maths as read_cell_centre)
// Author: Brandon Choi
int read_x(int col){
  return GRID_X_START + col * SMALL_X_CELL + SMALL_X_CELL / 2 - SENSOR_X_OFFSET;
}
// Author: Brandon Choi
int read_y(int row){
  return GRID_Y_START + row * SMALL_Y_CELL + SMALL_Y_CELL / 2 - SENSOR_Y_OFFSET;
}

// dabs a cell: start from the read position (sensor over the cell), then move by the sensor to dabber distance
// Author: Brandon Choi
void dab_cell(int row, int col){
  int x = read_x(col) + DAB_FROM_SENSOR_X;
  int y = read_y(row) + DAB_FROM_SENSOR_Y;

  // approach from the same side every time, like the reads
  go_to(x - BACKLASH_DEGREES, y - BACKLASH_DEGREES);
  go_to(x, y);
  printf("dab row %d col %d at (%d, %d)\n", row, col, current_x, current_y);

  pen_down();
  wait(400, msec); // hold so the ink transfers
  pen_up();
}

// scans only the empty cells for a new green mark
// updates the grid on the brain after every cell
// returns how many it found, and where the last one was
// Author: Nathaniel Rayman
int find_human_move(int &move_row, int &move_col){
  Optical1.setLight(ledState::on);
  clear_scan_marks();

  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      if (board[r][c] == EMPTY) {
        if (read_cell_centre(r, c)) {
          move_row = r;
          move_col = c;

          board[r][c] = HUMAN; // G on the grid right away
          show_message("Green found!");
          printf("green at row %d col %d\n", r, c);
          wait(500,msec);
          clear_scan_marks(); // back to a normal grid
          go_to(0, 0); // out of the player's way
          return 1;
        } else {
          scan_mark[r][c] = SCANNED_EMPTY; // - on the grid right away
          show_message("Scanning...");
        }
        wait(500, msec); // time to see the update
      }
    }
  }

  clear_scan_marks(); // back to a normal grid
  go_to(0, 0); // out of the player's way
  return 0;
}

// plays one full game, human goes first
// Author: Brandon Choi
void play_game(){
  // start with an empty board
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      board[r][c] = EMPTY;
    }
  }

  go_to(0, 0);
  show_message("Your turn, press");

  while (check_winnner() == -1) {
    wait_for_press(); // human dabs a cell, then presses the TouchLED

    // read the human's move
    show_message("Scanning...");
    int hr = -1;
    int hc = -1;
    int found = find_human_move(hr, hc);

    if (found == 0) {
      show_message("No mark, press");
      continue; // dab or rescan, then press again
    }

    update_board(HUMAN, hr, hc);
    printf("human: row %d col %d\n", hr, hc);
    if (check_winnner() != -1) {
      break; // human's move ended the game
    }

    // robot's move
    show_message("Thinking...");
    int rr = -1;
    int rc = -1;
    find_best_move(rr, rc);
    printf("robot: row %d col %d\n", rr, rc);

    show_message("My move");
    dab_cell(rr, rc);
    update_board(ROBOT, rr, rc);
    go_to(0, 0); // out of the player's way

    if (check_winnner() == -1) {
      show_message("Your turn, press");
    }
  }

  // game over
  int result = check_winnner();
  if (result == ROBOT) {
    show_message("Robot wins!");
  } else if (result == HUMAN) {
    show_message("You win!");
  } else {
    show_message("Draw!");
  }
  printf("game over: %d\n", result);

  go_to(0, 0);
  home_x();
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  setMotors();
 
  motor_calibration(); // home z, x and y before anything moves
  wait_for_press();
  
  // move to middle spot in grid (for setup)
  dab_cell(1, 1);
  wait_for_press();
  go_to(0,0);

  //place paper
  play_game();  
  
}