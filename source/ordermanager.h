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
 * @param[in]   current_floor The last registered floor of the elevator.
 * @param[in]   order_above Tells us if we have an order above, below or no order.
 * @param[in]   between_floors Parameter for knowing if we are below, above or at @p current_floor.
 * @return 1 if we are at a floor with an order, 0 otherwise.
*/
int ordermanager_at_ordered_floor(int current_floor, int order_above, int between_floors);

/**
 * @brief Checks if the queue is empty below @p current_floor.
 * @param[in]   queue The queue we are checking.
 * @param[in]   current_floor The last registered floor of the elevator.
 * @return 1 if the @p queue is empty below @p current_floor, 0 otherwise.
 */
int ordermanager_queue_is_empty_below(int *queue, int current_floor);

/**
 * @brief Checks if the queue is empty above @p current_floor .
 * @param[in]   queue The queue we are checking.
 * @param[in]   current_floor The last registered floor of the elevator.
 * @return 1 if the @p queue is empty above @p current_floor , 0 otherwise.
 */
int ordermanager_queue_is_empty_above(int *queue, int current_floor);

/** 
 * @brief Polls all the floor sensors and puts them in the queue. Also turns on the order lights.
 */
void ordermanager_poll_order_sensors();

/**
 * @brief Adds order to the queue. Used in ordermanager_poll_order_sensors.
 * @param[in]   floor floor to be added to queue
 * @param[in]   order_type order type to be added to queue
 */
void ordermanager_add_order(int floor, HardwareOrder order_type);

/**
 * @brief Checks if there are orders above or below the current floor.
 * @param[in]   current_floor elevator's current floor
 * @param[in]   driving_direction elevator's driving direction
 * @return 1 if next order is above, -1 if next order is below, 0 if there are no orders.
 */
int ordermanager_order_above_or_below(int current_floor, int between_floors, HardwareMovement driving_direction);

/**
 * @brief Sets all of the queue's entries to 0. Also turns all the order lights off.
 */
void ordermanager_clear_queues();

/**
 * @brief Removes an order from the queue in the @p driving_direction .
 * @param[in] floor Floor to be removed
 * @param[in] driving_direction Up or down. Elevator's driving direction.
 */
void ordermanager_remove_order(int floor, HardwareMovement driving_direction);

/**
 * @brief Checks if the @p queue is empty.
 * @param[in]   queue The queue we are checking.
 * @return 1 if empty, 0 otherwise.
 */
int ordermanager_queue_is_empty(int *queue);

#endif
