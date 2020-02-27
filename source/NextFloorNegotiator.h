#ifndef NEXTFLOORNEGOTIATOR_H
#define NEXTFLOORNEGOTIATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"


/** 
 * @brief Polls all the floor sensors and puts them in up_queue or down_queue
 */
void nfn_poll_order_sensors();

/**
 * @brief Adds order to up_queue or down_queue. Used in poll_sensors.
 * @param floor floor to be added to queue
 * @param order_type order type to be added to queue
 */
void nfn_add_orders(int floor, HardwareOrder order_type);

/**
 * @brief Checks if the next order is above the current floor.
 * @param current_floor elevator's current floor
 * @param driving_direction elevator's driving direction
 * @return 1 if next order is above, -1 if next order is below, 0 if there is no next order.
 */
int nfn_order_above(int current_floor, HardwareMovement driving_direction);


/**
 * @brief sets all of up_queue and down_queue's entries to 0. Also sets lights to 0.
 */
void nfn_clear_queues();

/**
 * @brief Checks if the elevator is at the goal floor.
 * @param next_floor goal floor.
 * @return 1 if elevator is at the goal floor, 0 otherwise.
 */
int nfn_at_next_floor(int next_floor, int current_floor);

/**
 * @brief finds the first 1 in the array in either descending or ascending order.
 * @param queue which array to iterate through
 * @param ascend_or_descend 0 to read descending, truthy value to read ascending.
 * @param lower_index starting index for the array
 * @param higher_index end index for the array
 * @return The index of the first entry in queue with a 1.
 */
int nfn_read_array_ascending_or_descending(int* queue, int ascend_or_descend, int lower_index, int higher_index);


/**
 * @brief finds the first 1 in the array
 * @param queue array to iterate through
 * @param above_or_below Iterates from current_floor and above (1), or from current_floor and below (0).
 * @param ascend_or_descend Goes through the array in ascending (1) or descending (0) order.
 * @param current_floor descides where to start and stop reading.
 * @return the index of the first 1 in the array, based on the conditions given.
 */
int nfn_find_first_one(int* queue, int above_or_below, int ascend_or_descend, int current_floor);

/**
 * @brief Figures out which floor to go to next.
 * @param driving_direction Elevator's current driving direction.
 * @return -1 if there are no orders, floor number otherwise.
 */
int nfn_get_next_floor(int current_floor, HardwareMovement driving_direction);

/**
 * @brief Removes an order from the up_queue or down_queue
 * @param floor Floor to be removed
 * @param driving_direction Elevator's driving direction.
 */
void nfn_remove_order(int floor, HardwareMovement driving_direction);

#endif
