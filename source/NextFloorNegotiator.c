#include "NextFloorNegotiator.h"



static int queue[HARDWARE_NUMBER_OF_FLOORS];


int nfn_queue_is_empty(){ 
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(queue[f] != 0){
            return 0;
        }
    }
    return 1;
}

int nfn_at_ordered_floor(int current_floor) {
    if (hardware_read_floor_sensor(current_floor) && queue[current_floor]) {
        return 1;
    } else { return 0; }
}


void nfn_add_order(int floor, HardwareOrder order_type) {
    switch(order_type) {
        case HARDWARE_ORDER_UP:
            hardware_command_order_light(floor, HARDWARE_ORDER_UP, 1);
            break;
        case HARDWARE_ORDER_DOWN:
            hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 1);
            break;
        case HARDWARE_ORDER_INSIDE:
            hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 1);
            break;
    }
    queue[floor] = 1;
}

void nfn_poll_order_sensors() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        int up = hardware_read_order(f, HARDWARE_ORDER_UP);
        int inside = hardware_read_order(f, HARDWARE_ORDER_INSIDE);
        int down = hardware_read_order(f, HARDWARE_ORDER_DOWN);

        if (up) {
            nfn_add_order(f, HARDWARE_ORDER_UP);
        }

        if (inside) {
            nfn_add_order(f, HARDWARE_ORDER_INSIDE);
        }
        
        if (down) {
            nfn_add_order(f, HARDWARE_ORDER_DOWN);
        }

    }
}


int nfn_order_above(int current_floor, int between_floors, HardwareMovement driving_direction) {
    if (nfn_queue_is_empty()) {
        return NO_ORDER_FOUND;
    }

    switch(driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                if (queue[f]) {
                    return ABOVE;
                }
            }
            return BELOW;
            break;

        case HARDWARE_MOVEMENT_DOWN:
            for (int f = 0; f < current_floor; f++) {
                if (queue[f]) {
                    return BELOW;
                }
            }
            return ABOVE;
            break;
        case HARDWARE_MOVEMENT_STOP:
            if (between_floors == ABOVE) {
                for (int f = 0; f <= current_floor; f++) {
                    if (queue[f]) {
                        return BELOW;
                    }
                }
                return ABOVE;
            } else if (between_floors == BELOW) {
                for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
                    if (queue[f]) {
                        return ABOVE;
                    }
                }
                return BELOW;
            }
            break;
    }
    return NO_ORDER_FOUND;//no orders
}

void nfn_clear_queue() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
        queue[f] = 0;
    }
}

void nfn_remove_order(int floor, HardwareMovement driving_direction) {
    queue[floor] = 0;
    hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
}