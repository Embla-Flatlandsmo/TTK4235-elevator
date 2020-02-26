#ifndef NEXTFLOORNEGOTIATOR_H
#define NEXTFLOORNEGOTIATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"


/** 
 * @brief Polls all the floor sensors and puts them in up_queue or down_queue
 */
void next_floor_negotiator_poll_order_sensors();

/**
 * @brief Adds order to up_queue or down_queue. Used in poll_sensors.
 * @param floor floor to be added to queue
 * @param order_type order type to be added to queue
 */
void next_floor_negotiator_add_orders(int floor, HardwareOrder order_type);

/**
 * @brief Checks if the next order is above the current floor.
 * @param current_floor elevator's current floor
 * @param driving_direction elevator's driving direction
 * @return 1 if next order is above, -1 if next order is below, 0 if there is no next order.
 */
int next_floor_negotiator_order_above(int current_floor, HardwareMovement driving_direction);


/**
 * @brief sets all of up_queue and down_queue's entries to 0. Also sets lights to 0.
 */
void next_floor_negotiator_clear_queues();

/**
 * @brief Checks if the elevator is at the goal floor.
 * @param next_floor goal floor.
 * @return 1 if elevator is at the goal floor, 0 otherwise.
 */
int next_floor_negotiator_at_next_floor(int next_floor, int current_floor);

/**
 * @brief Figures out which floor to go to next.
 * @param driving_direction Elevator's current driving direction.
 * @return -1 if there are no orders, floor number otherwise.
 */
int next_floor_negotiator_get_next_floor(int current_floor, HardwareMovement driving_direction);

/**
 * @brief Removes an order from the up_queue or down_queue
 * @param floor Floor to be removed
 * @param driving_direction Elevator's driving direction.
 */
void next_floor_negotiator_remove_order(int floor, HardwareMovement driving_direction);

/**
 * @brief Checks is a queue is empty.
 * 
 * @param q An array of 4 elements (a queue).
 * @return 1 if queue empty, 0 if not
 */
int isEmpty(HardwareMovement up_or_down);

#endif
