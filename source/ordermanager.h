/**
 * @file
 * @brief Module for managing orders, determining where to go and determining where to stop.
 */

#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#define ABOVE 1
#define BELOW -1
#define ATFLOOR 0
#define NO_ORDER_FOUND 0


/**
 * @brief checks if the current floor contains an order.
 * @param[in] current_floor current floor.
 * @param[in] driving_direction 
 * @return 1 if we are at a floor with an order, 0 otherwise.
*/
int int ordermanager_at_ordered_floor(int current_floor, HardwareMovement driving_direction);

/** 
 * @brief Polls all the floor sensors and puts them in the queue. Also turns on the order lights.
 */
void ordermanager_poll_order_sensors();

/**
 * @brief Adds order to the queue. Used in ordermanager_poll_order_sensors.
 * @param[in]   floor floor to be added to queue
 * @param[in]   order_type order type to be added to queue
 */
void ordermanager_add_orders(int floor, HardwareOrder order_type);

/**
 * @brief Checks if there are orders above or below the current floor.
 * @param[in]   current_floor elevator's current floor
 * @param[in]   driving_direction elevator's driving direction
 * @return 1 if next order is above, -1 if next order is below, 0 if there are no orders.
 */
int ordermanager_order_above_or_below(int current_floor, int between_floors, HardwareMovement driving_direction);


/**
 * @brief sets all of the queue's entries to 0. Also turns all the order lights off.
 */
void ordermanager_clear_queues();

/**
 * @brief Removes an order from the queue
 * @param[in] floor Floor to be removed
 * @param[in] driving_direction Elevator's driving direction.
 */
void ordermanager_remove_order(int floor, HardwareMovement driving_direction);

/**
 * @brief Checks if the queue is empty.
 * @return 1 if empty, 0 otherwise.
 */
int ordermanager_queue_is_empty(int* queue);

#endif
