#ifndef NEXTFLOORNEGOTIATOR_H
#define NEXTFLOORNEGOTIATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#define ABOVE 1
#define BELOW -1
#define ATFLOOR 0
#define NO_ORDER_FOUND 0


/**
 * @brief checks if the current floor
*/
int nfn_at_ordered_floor(int current_floor);

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
int nfn_order_above(int current_floor, int between_floors, HardwareMovement driving_direction);


/**
 * @brief sets all of up_queue and down_queue's entries to 0. Also sets lights to 0.
 */
void nfn_clear_queue();

/**
 * @brief Removes an order from the up_queue or down_queue
 * @param floor Floor to be removed
 * @param driving_direction Elevator's driving direction.
 */
void nfn_remove_order(int floor, HardwareMovement driving_direction);

#endif
