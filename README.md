# VEX IQ Robotic Tic-Tac-Toe

An interactive Tic-Tac-Toe robot designed for the MTE 100 engineering design project. The robot is intended to let a person play against an autonomous opponent while interacting with a physical game board.

> **Project status:** Design and implementation in progress. This README describes the **planned software architecture from our formal presentation**, not a claim that every feature has been implemented or tested.

## Project goal

Build a robot that can:
1. Present the game board to a human player.
2. Wait for the player to indicate that they have completed a move.
3. Scan the board with an optical sensor to identify the new move.
4. Calculate an optimal legal response using the minimax algorithm.
5. Move an X-Y mechanism to the selected cell and place a mark with a bingo dabber.
6. Repeat until a player wins or the game ends in a draw.
7. Display the result and return to a safe home position.

## Hardware and inputs

The mechanical concept is a **bedslinger-style X-Y gantry**, inspired by 3D printers. The intended actuators and inputs are:

| Component | Planned responsibility |
| --- | --- |
| X-axis motor | Position the marking/sensing mechanism along one axis |
| Y-axis motor | Position the bed or mechanism along the other axis |
| Pen-lift motor | Lower and raise the bingo dabber |
| Touch sensor | Signal when the human has finished their turn / is ready |
| Optical sensor | Read the board's cell colours to detect the player's move |
| Bumper | Support homing, limit detection, or a safety input, depending on final wiring |
| Motor encoders | Provide position feedback for controlled movement |

**Note:** The exact axis-to-motor mapping, bumper behaviour, and sensor thresholds must be confirmed against the assembled robot.

## High-level software architecture

The controller is organized around three phases:

```text
STARTUP
  |
  v
Initialize motors, sensors and board state
  |
Home / calibrate axes
  |
Wait for player to start
  |
  v
GAMEPLAY LOOP ------------------------------------+
  |                                              |
Move board toward player                         |
  |                                              |
Wait for touch-sensor confirmation                |
  |                                              |
Move board to scanning position                   |
  |                                              |
Scan nine cells using optical sensor              |
  |                                              |
Validate the detected human move                  |
  |                                              |
Update board state                                |
  |                                              |
Check for human win / draw --------------------+  |
  |                                           |  |
Calculate optimal robot move (minimax)         |  |
  |                                           |  |
Move gantry to target cell                     |  |
  |                                           |  |
Lower and raise bingo dabber                   |  |
  |                                           |  |
Update board state                             |  |
  |                                           |  |
Check for robot win / draw                     |  |
  |                                           |  |
  +-- if game continues ----------------------+--+
  |
  v
SHUTDOWN
  |
Display winner or draw on VEX brain
  |
Return robot to home position
  |
Stop motors / end program
```

The gameplay loop must terminate on **either a win or a draw**, not only when someone wins.

## Board representation

Represent the 3×3 board as a two-dimensional array, for example:

```text
EMPTY = 0
HUMAN = 1
ROBOT = 2

board = [
  [0, 0, 0],
  [0, 0, 0],
  [0, 0, 0]
]
```

The values above are illustrative; choose constants appropriate for the actual VEX IQ programming environment. The software should maintain a single authoritative board state and update it **only after a move has been validated or successfully executed**.

## Proposed modules / functions

These are **conceptual interfaces**, not confirmed source-code signatures.

| Function | Responsibility |
| --- | --- |
| `move_gantry(x, y)` | Convert a target cell/position into controlled X-Y motor movement |
| `move_pen(up_down)` | Lower the dabber to mark the paper, then retract it |
| `sense_move()` | Scan the board and return a candidate human move |
| `update_board(move, player)` | Validate and record a legal move in the 3×3 state |
| `make_decision(board)` | Run minimax to choose the robot's best legal countermove |
| `output_winner(board)` | Determine/display a win, loss, or draw |

Additional helpers will likely be useful:

- `home_axes()`: establish a repeatable origin before moving to game cells.
- `cell_to_position(row, col)`: map a board cell to motor target coordinates.
- `is_valid_move(row, col, board)`: reject out-of-range or occupied cells.
- `check_game_result(board)`: detect three-in-a-row, a draw, or an ongoing game.
- `stop_motors()`: bring actuators to a safe stop.

### 1. Startup and calibration

On startup, initialize the motors and sensors, set the board to empty, and establish a known reference position. Homing should use the selected bumper/limit approach or another verified calibration method. The robot should **not** enter gameplay until calibration is complete.

### 2. Player interaction and sensing

The bed moves into the player-accessible position. The robot waits for a touch-sensor press indicating that the player has made their move. It then moves to the scanning position and reads the relevant cells using the optical sensor.

Compare the sensed board with the stored board state to identify **exactly one new, legal human move**. An unclear scan, a mark in an occupied cell, or multiple new marks should not silently overwrite the internal board state. The final strategy for rescan/retry feedback is still to be implemented.

### 3. Decision-making: minimax

After a valid human move is stored, evaluate whether the game has already ended. If not, run minimax over the remaining legal moves.

- A robot win is a favourable terminal state.
- A human win is an unfavourable terminal state.
- A draw is neutral.
- The robot maximizes its outcome; the human is assumed to choose the strongest response.

Optionally prefer faster wins and slower losses by incorporating search depth into terminal scores. The selected move must be in an empty cell.

### 4. Robot movement and marking

Convert the chosen row and column into calibrated motor coordinates. Move the gantry to the target, lower the bingo dabber to mark the square, and raise it again before travelling. Record the robot move in the board array only after the marking sequence succeeds, or use a clearly defined recovery state if success cannot be verified.

### 5. End-of-game shutdown

After each human and robot turn, check for a winner or draw. When the game ends, display the result on the VEX brain, return to the home position if safe, stop the motors, and exit the gameplay loop.

## State machine (suggested)

A state-based implementation can make the main program easier to debug:

`INITIALIZE → HOME → WAIT_FOR_PLAYER → SCAN_BOARD → VALIDATE_MOVE → CHECK_RESULT → THINK → MOVE_AND_MARK → CHECK_RESULT → WAIT_FOR_PLAYER`

Any end-of-game result transitions to `SHUTDOWN`. Invalid scans should transition to a controlled `RETRY` or `ERROR` state rather than executing an unverified move.

## Testing and validation plan

These are **planned acceptance tests**. Record actual results separately as the robot is built.

| Test | Procedure | Pass criterion |
| --- | --- | --- |
| Startup | Power on and home/calibrate | Homes safely and waits for input |
| Gantry positioning | Command all nine cell centres | Reaches intended cell without collision |
| Pen mechanism | Raise/lower five times | Makes a visible mark and retracts each time |
| Optical detection | Place a mark in different cells | Identifies the correct new cell |
| Game logic | Feed win, block and draw board states | Returns a legal, optimal move or correct result |
| Full gameplay | Play an entire game | Alternates turns and detects the correct ending |
| Shutdown | Complete a game | Displays result, homes safely, and stops motors |

### Fault-handling tests to consider

| Situation | Desired response |
| --- | --- |
| Player marks an occupied cell | Reject the move; keep stored board unchanged |
| Optical sensor produces an unclear reading | Do not move until the reading is resolved |
| Touch sensor is pressed repeatedly | Do not count duplicate turns |
| Limit bumper activates unexpectedly | Stop or enter a safe fault state |
| Gantry is obstructed | Stop or enter a safe fault state if detectable |
| Sensor becomes unavailable | Do not execute a move based on invalid data |
| Game reaches a draw | Display draw and shut down normally |

Fault detection and recovery must be implemented and tested before they can be described as supported behaviour. Set quantitative positioning and sensing thresholds after measuring the actual hardware.

## Implementation checklist

- [ ] Assemble and calibrate the bedslinger gantry
- [ ] Verify X-axis, Y-axis, and pen-lift motor movement
- [ ] Implement homing and cell-to-position mapping
- [ ] Read and debounce the touch sensor
- [ ] Calibrate optical readings for all nine board cells
- [ ] Implement board-state validation and winner/draw detection
- [ ] Implement and test minimax independently
- [ ] Integrate sensing, decision-making, and marking
- [ ] Add safe shutdown and feasible fault handling
- [ ] Execute and document end-to-end acceptance tests

## Stretch goals

Beyond the MVP, the team has considered drawing X's and O's, replacing the bingo dabber with a pick-and-place claw, and rotating the game field to improve cycle time. These are **proposed enhancements**, not required for the baseline gameplay loop.

## Team

Brandon Choi · Matthias Six · Onna Okeke · Nathaniel Rayman

**Course:** MTE 100 — University of Waterloo

---

This document captures the architecture presented in the team's formal design presentation. As implementation progresses, update it with actual file names, function signatures, hardware port assignments, calibration values, and verified test results.
