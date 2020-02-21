#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "LinkedList.h"

State ElevatorState;
HARDWARE_MOVEMENT drivingDirection;
int goalFloor;
int currentFloor;
struct Node* FloorOrders;


typedef struct {
    int floor;
    HardwareOrder orderType;
} FloorOrder;

typedef enum {
    INITIAL_CHECK,
    INITIAL_DRIVE_DOWN,
    DRIVE_DOWN,
    IDLE,
    DRIVE_UP,
    STOP
} State;

int timer_expired(time_t start_time);

time_t start_time();

/**
 * @brief Creates a linked list of all the buttons currently being pressed
 * @return Head of the linked list
 */
struct Node* readOrders();

/**
 * @brief Checks all floor sensors.
 * @return Floor number if currently in a floor, 0 otherwise
 */
int checkAllFloorSensors();

/**
 * @brief Clears all order lights.
 */
static void clear_all_order_lights();