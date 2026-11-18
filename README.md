# VEX IQ Robotic Tic-Tac-Toe

An interactive Tic-Tac-Toe robot for the MTE 100 engineering design project. A person plays against an autonomous opponent on a physical board: the robot senses the human's move with an optical sensor, picks its reply with minimax, and marks the board with a bingo dabber on an X-Y gantry.

> **Project status:** In progress. Game logic (board, win/draw detection, minimax) and basic gantry/pen movement exist in `main.cpp`. Sensing, Y/pen homing, and the full gameplay loop are **not yet implemented**. `main()` currently runs a gantry movement test only.

## Files

| File | Purpose |
| --- | --- |
| `main.cpp` | Current robot program (VEXcode IQ C++). **Source of truth.** |
| `tictacproject.iqcpp` | VEXcode IQ project file. Its embedded code is an older version without the device configuration; re-sync it from `main.cpp` before uploading through VEXcode. |
| `README.md` | This document. |

## Hardware configuration

Declared in the VEXcode generated region of `main.cpp`:

| Device | Port | Variable | Role |
| --- | --- | --- | --- |
| Optical sensor | 1 | `Optical1` | Reads cell colour (hue) to detect marks |
| Bumper | 2 | `Bumper2` | X-axis home limit switch |
| TouchLED | 3 | `TouchLED3` | Player "start / ready" button |
| Motor | 4 | `pen_motor` | Lowers / raises the bingo dabber |
| Motor | 5 | `y_motor` | Y axis |
| Motor | 6 | `x_motor` | X axis |
| Brain inertial | built-in | `BrainInertial` | Only used to seed `rand()` (VEXcode boilerplate) |

Motor settings (`setMotors()`): X 60 % velocity, Y 50 %, pen 70 %; X and Y max torque 100 %.

## Coordinate and direction convention

All gantry motion uses the constants defined near the top of the project code. Never use raw `forward` / `reverse` for the X or Y motors.

| Constant | Value | Meaning |
| --- | --- | --- |
| `X_AWAY` | `reverse` | X toward higher cell index (away from the bumper) |
| `X_HOME` | `forward` | X back toward cell 0 / into `Bumper2` |
| `Y_AWAY` | `forward` | Y toward higher cell index |
| `Y_HOME` | `reverse` | Y back toward cell 0 |
| `CELL_DEGREES` | `160` | Motor degrees between neighbouring cells |

- **Home is cell (0, 0)**: X pressed against `Bumper2`, Y at its start position.
- Gantry cell coordinates are `(x, y)`, each 0–2.
- The board array is indexed `board[row][col]`. **The mapping between `(row, col)` and gantry `(x, y)` is not yet defined in code**. Confirm it on the assembled robot and pick one mapping before connecting minimax output to `move_to_mark`.

## Board representation

```cpp
const int EMPTY = 0;
const int HUMAN = 1;
const int ROBOT = 2;

int board[3][3];   // global, starts all EMPTY
```

Optical colour mapping in `check_color()`: **purple → HUMAN (1)**, **green → ROBOT (2)**, otherwise **white → EMPTY (0)**.

## Functions in `main.cpp`

### Game logic

| Function | What it does |
| --- | --- |
| `print_board()` | Clears the brain screen and prints the 3×3 board as numbers |
| `update_board(move, bx, by)` | Writes `move` into `board[bx][by]` if empty and returns `true`; otherwise prints "Invalid input" and returns `false`. No range check. |
| `check_winnner()` | Returns `HUMAN` / `ROBOT` for three-in-a-row, `-1` if the game is still going, `0` for a draw |
| `minimax(depth, isMaximizing)` | Full-depth minimax over the global board. Scores `10 - depth` for a robot win, `depth - 10` for a human win, `0` for a draw |
| `find_best_move(bestRow, bestCol)` | Tries every empty cell for `ROBOT` and returns the highest-scoring one; `-1, -1` if the board is full |

### Motion and sensing

| Function | What it does |
| --- | --- |
| `setMotors()` | Applies velocity / torque settings |
| `reset_game()` | Waits for a `TouchLED3` press, then drives X in `X_HOME` until `Bumper2` is pressed. **X only**; Y and pen are not homed. |
| `make_mark()` | Pen down 140° then up 140° |
| `move_to_mark(x, y)` | From home: moves `x` cells `X_AWAY`, `y` cells `Y_AWAY`, makes a mark, then returns home |
| `make_dot_grid()` | Snake path from home marking all nine cells (0,0)…(2,2), then returns home |
| `read_mark(x, y)` | Intended to read a cell. **Currently calls `move_to_mark`, so it marks the cell and reads at home**. Needs a move-only helper. |
| `check_color()` | Reads `Optical1.hue()`: `< 150` green, `150–330` purple, else white. Displays and returns the matching board value |
| `checkGrid()` | Commented out; incomplete (`x`, `y` undefined) |

### `main()` today

```text
vexcodeInit() → setMotors() → reset_game()  (wait for touch, home X)
→ move_to_mark(0,0) → move_to_mark(2,2) → move_to_mark(1,1) → end
```

Earlier tests (minimax on a preset board, `check_winnner`, colour reading, dot grid) remain in `main()` as comments.

## Planned gameplay loop (not yet implemented)

```text
INITIALIZE → HOME → WAIT_FOR_PLAYER → SCAN_BOARD → VALIDATE_MOVE → CHECK_RESULT
→ THINK (find_best_move) → MOVE_AND_MARK → CHECK_RESULT → WAIT_FOR_PLAYER ...
Any win/draw → SHUTDOWN (display result, return home, stop motors)
Invalid scan → RETRY / ERROR (never act on an unverified move)
```

The board should only be updated after a human move is validated (exactly one new mark in an empty cell) or after the robot's mark is made.

## Known issues / to-do

- [ ] `read_mark` marks the paper instead of only moving. Split out a `move_to_cell(x, y)` without `make_mark()`.
- [ ] `check_color` reads hue *before* its 1 s wait, clears the screen (erasing the board), and never turns on the optical LED (`Optical1.setLight(ledState::on)`; old comment refers to `Optical8`).
- [ ] Hue alone is unreliable for white paper. Consider brightness/saturation, and recalibrate thresholds on the real board (hue exactly 150 and ≥ 330 currently fall to "white").
- [ ] Home Y and the pen; reset encoder positions after homing.
- [ ] Busy-wait loops in `reset_game` have no `wait()`, debounce, or timeout.
- [ ] Define the `board[row][col]` ↔ gantry `(x, y)` mapping.
- [ ] Add range checks to `update_board` / `move_to_mark`.
- [ ] Brain screen rows 5–6 are used for messages. Verify they are visible on the IQ brain.
- [ ] Implement the gameplay loop, scanning of all nine cells, and shutdown.
- [ ] Re-sync `tictacproject.iqcpp` with `main.cpp`.

## Testing plan

| Test | Procedure | Pass criterion |
| --- | --- | --- |
| Homing | Power on, press TouchLED | X stops on bumper; waits for input |
| Gantry positioning | `move_to_mark` on all nine cells / `make_dot_grid()` | Dots land on the intended cells, no collision |
| Pen mechanism | `make_mark()` ×5 | Visible mark, pen retracts each time |
| Optical detection | Place purple / green / blank under sensor | `check_color()` returns 1 / 2 / 0 |
| Game logic | Preset win, block and draw boards | `find_best_move` returns the optimal legal move; `check_winnner` correct |
| Full gameplay | Play an entire game | Alternates turns, detects the correct ending |

## Stretch goals

Drawing X's and O's, replacing the dabber with a pick-and-place claw, and rotating the game field to improve cycle time.

## Team

Brandon Choi · Matthias Six · Onna Okeke · Nathaniel Rayman

**Course:** MTE 100, University of Waterloo
