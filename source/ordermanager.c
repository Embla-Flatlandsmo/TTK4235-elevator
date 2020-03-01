#include "ordermanager.h"



static int up_queue[HARDWARE_NUMBER_OF_FLOORS];
static int down_queue[HARDWARE_NUMBER_OF_FLOORS];


int ordermanager_queue_is_empty(int* queue){ 
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(queue[f] != 0){
            return 0;
        }
    }
    return 1;
}

int ordermanager_at_ordered_floor(int current_floor, HardwareMovement driving_direction) {
    int* tmp_queue;
    if (driving_direction == HARDWARE_MOVEMENT_UP) {
        tmp_queue = up_queue;
    } else {
        tmp_queue = down_queue;
    }
    if (hardware_read_floor_sensor(current_floor) && (tmp_queue[current_floor]) {
        return 1;
    } else { return 0; }
}


void ordermanager_add_order(int floor, HardwareOrder order_type) {
    switch(order_type) {
        case HARDWARE_ORDER_UP:
            up_queue[floor] = 1;
            hardware_command_order_light(floor, HARDWARE_ORDER_UP, 1);
            break;
        case HARDWARE_ORDER_DOWN:
            down_queue[floor] = 1;
            hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 1);
            break;
        case HARDWARE_ORDER_INSIDE:
            up_queue[floor] = 1;
            down_queue[floor] = 1;
            hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
            break;
    }
}

void ordermanager_poll_order_sensors() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        int up = hardware_read_order(f, HARDWARE_ORDER_UP);
        int inside = hardware_read_order(f, HARDWARE_ORDER_INSIDE);
        int down = hardware_read_order(f, HARDWARE_ORDER_DOWN);

        if (up) {
            ordermanager_add_order(f, HARDWARE_ORDER_UP);
        }

        if (inside) {
            ordermanager_add_order(f, HARDWARE_ORDER_INSIDE);
        }
        
        if (down) {
            ordermanager_add_order(f, HARDWARE_ORDER_DOWN);
        }

    }
}


int ordermanager_order_above_or_below(int current_floor, int between_floors, HardwareMovement driving_direction) {
    //int up_queue_empty = order_manager_queue_is_empty(up_queue);
    //int down_queue_empty = order_manager_queue_is_empty(down_queue);
    if (ordermanager_queue_is_empty(up_queue) && ordermanager_queue_is_empty(down_queue)) {
        return NO_ORDER_FOUND;
    }


    int* tmp_queue;
    switch(driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            if (ordermanager_queue_is_empty(up_queue)) {
                tmp_queue = down_queue;
            } else {
                tpm_queue = up_queue;
            }
            break;

        case HARDWARE_MOVEMENT_DOWN:
            if (ordermanager_queue_is_empty(down_queue)) {
                tmp_queue = up_queue;
            } else {
                tmp_queue = down_queue;
            }
            break;
        case HARDWARE_MOVEMENT_STOP:
            if (ordermanager_queue_is_empty(down_queue)) {
                tmp_queue = up_queue;
            } else {
                tmp_queue = down_queue;
            }

            for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                if (between_floors == ABOVE && tmp_queue[current_floor]) {
                    return BELOW;
                } else if (tmp_queue[f]) {
                    return ABOVE;
                }
            }
            return BELOW;
            break;

/*
            if (between_floors == ABOVE) {
                for (int f = 0; f <= current_floor; f++) {
                    if (tmp_queue[f]) {
                        return BELOW;
                    }
                }
                return ABOVE;
            } else if (between_floors == BELOW || between_floors == ATFLOOR) {
                for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                    if (tmp_queue[f]) {
                        return ABOVE;
                    }
                }
                return BELOW;
            }
            break;
*/
    }


    for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (between_floors == ABOVE && tmp_queue[current_floor]) {
            return BELOW;
        } else if (tmp_queue[f]) {
            return ABOVE;
        }
    }
    return BELOW;

/*
    switch(between_floors) {
        case ABOVE:
            for (int f = current_floor+1; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                if (tmp_queue[f]) {
                    return ABOVE;
                }
            }
            return BELOW;
            break;
        case BELOW: 
            for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                if (tmp_queue[f]) {
                    return ABOVE;
                }
            }
            return BELOW;
            break;
        case ATFLOOR:
            for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLORS; f++) {
                if (tmp_queue[f]) {
                    return ABOVE;
                }
            }
            return BELOW;
            break;
        default:
            break;
    }
*/
    return NO_ORDER_FOUND;
}

void ordermanager_clear_queues() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
        up_queue[f] = 0;
        down_queue[f] = 0;
    }
}

void ordermanager_remove_order(int floor, HardwareMovement driving_direction) {
    up_queue[floor] = 0;
    down_queue[floor] = 0;
    hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
}