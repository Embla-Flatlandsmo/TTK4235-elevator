#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "LinkedList.h"
#include "NextFloorNegotiator.h"

/**
 * @brief Struct that gives which floor is being ordered, and which type of order it is.
 */
typedef struct {
    int floor;
    HardwareOrder orderType;
} FloorOrder;

/**
 * @brief States for the state machine
 */
typedef enum {
    INITIAL_CHECK,
    INITIAL_DRIVE_DOWN,
    DRIVE_DOWN,
    IDLE,
    DRIVE_UP,
    STOP
} State;

static State ElevatorState;
static HardwareMovement drivingDirection;
static int goalFloor;
static int currentFloor;
struct Node* FloorOrders;

/**
 * @brief State Machine for the elevator.
 * @param currentState current state
 * @return new state
 */
State stateMachine(State currentState);

/**
 * @brief Checks if the timer has expired
 * @param start_time starting time
 * @return 0 if timer is still going, 1 if timer has expired.
 */
int timer_expired(time_t start_time);

/** @brief Utility function. Sets the start time for timer_expired
 * @return start time
 */
time_t start_time();

/**
 * @brief Creates a linked list of all the buttons currently being pressed
 * @return Head of the linked list
 */
struct Node* readOrders();

/**
 * @brief Checks the floor sensors of all the floors.
 * @return Current floor, 0 if not currently in a floor.
 */
int checkAllFloorSensors();