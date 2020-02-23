#ifndef ELEVATORSTATEMACHINE_H
#define ELEVATORSTATEMACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "NextFloorNegotiator.h"
#include "LinkedList.h"

HardwareMovement* drivingDirection;
int* goalFloor;
int* currentFloor;
struct Node* FloorOrders;
int pollingSensors;

typedef enum {
    INITIAL_CHECK,
    INITIAL_DRIVE_DOWN,
    DRIVE_DOWN,
    IDLE,
    DRIVE_UP,
    STOP
} State;

void stateMachine(State currentState);

int timer_expired(time_t start_time);

time_t start_time();

/**
 * @brief Creates a linked list of all the buttons currently being pressed
 * @return Head of the linked list
 */
void readOrders();

/**
 * @brief Checks all floor sensors.
 * @return Floor number if currently in a floor, 0 otherwise
 */
int checkAllFloorSensors();

/**
 * @brief Clears all order lights.
 */
void clear_all_order_lights();

#endif //ELEVATORSTATEMACHINE_H