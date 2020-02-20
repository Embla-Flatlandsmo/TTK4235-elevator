#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"

State ElevatorState;
global HARDWARE_MOVEMENT drivingDirection;
global int goalFloor;
global int currentFloor;
list<FloorOrder> FloorOrders;


typdef struct {
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

list<FloorOrder> readOrders();

int checkAllFloorSensors();