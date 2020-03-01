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

int ordermanager_queue_is_empty_above(int* queue, int current_floor) {
    for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (queue[f] != 0) {
            return 0;
        }
    }
    return 1;
}

int ordermanager_queue_is_empty_below(int* queue, int current_floor) {
    for (int f = 0; f <= current_floor; f++) {
        if (queue[f] != 0)
        return 0;
    }
    return 1;
}

int ordermanager_at_ordered_floor(int current_floor, int order_above, int between_floors) {
    if (between_floors != ATFLOOR) {
        return 0;
    }
    int next_floor = -1;

    switch(order_above){
        case ABOVE:
            if (!ordermanager_queue_is_empty_above(up_queue, current_floor)) {
                for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                    if (up_queue[f]) {
                        next_floor = f;
                        break;
                    }
                }    
            } else {
                for (int f = HARDWARE_NUMBER_OF_FLOORS-1; f >= current_floor; f--) {
                    if (down_queue[f]) {
                        next_floor = f;
                        break;
                    }
                }              
            }
            break;
        case BELOW:
            if (!ordermanager_queue_is_empty_below(down_queue, current_floor)) {
                for (int f = current_floor; f>= 0; f--) {
                    if (down_queue[f]) {
                        next_floor = f;
                        break;
                    }
                }
            } else {
                for (int f = 0; f <= current_floor; f++) {
                    if (up_queue[f]) {
                        next_floor = f;
                        break;
                    }
                }
            }
            break;
        case NO_ORDER_FOUND:
            return 0;
            break;
    }
    if (hardware_read_floor_sensor(next_floor)) {
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
    if (ordermanager_queue_is_empty(up_queue) && ordermanager_queue_is_empty(down_queue)) {
        return NO_ORDER_FOUND;
    }

    int* tmp_queue;
    switch(driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            if (!ordermanager_queue_is_empty_above(up_queue, current_floor)) {
                return ABOVE;
            } else if (!ordermanager_queue_is_empty_above(down_queue, current_floor)) {
                return ABOVE;
            }
            return BELOW;
            break;

        case HARDWARE_MOVEMENT_DOWN:
            if (!ordermanager_queue_is_empty_below(down_queue, current_floor)){
                return BELOW;
            } else if (!ordermanager_queue_is_empty_below(up_queue, current_floor)){
                return BELOW;
            }
            return ABOVE;
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
            break;
    }
    return BELOW;
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