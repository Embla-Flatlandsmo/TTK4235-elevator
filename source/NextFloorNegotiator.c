#include "NextFloorNegotiator.h"

static int up_queue[HARDWARE_NUMBER_OF_FLOORS];
static int down_queue[HARDWARE_NUMBER_OF_FLOORS];


int next_floor_negotiator_is_empty(HardwareMovement up_or_down){ 
    int* tmp_queue
    switch (up_or_down) {
        case HARDWARE_MOVEMENT_UP:
            tmp_queue = up_queue;
            break;
        case HARDWARE_MOVEMENT_DOWN:
            tmp_queue = down_queue;
            break;
        default:
            break;
    }

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        if(tmp_queue[f] != 0){
            return 0;
        }
    }
    return 1;
}

void next_floor_negotiator_add_order(int floor, HardwareOrder order_type) {
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

void next_floor_negotiator_poll_order_sensors() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        int up = hardware_read_order(f, HARDWARE_ORDER_UP);
        int inside = hardware_read_order(f, HARDWARE_ORDER_INSIDE);
        int down = hardware_read_order(f, HARDWARE_ORDER_DOWN);

        if (up) {
            next_floor_negotiator_add_order(f, HARDWARE_ORDER_UP);
        }

        if (inside) {
            next_floor_negotiator_add_order(f, HARDWARE_ORDER_INSIDE);
        }
        
        if (down) {
            next_floor_negotiator_add_order(f, HARDWARE_ORDER_DOWN);
        }

    }
}


int next_floor_negotiator_order_above(int current_floor, HardwareMovement driving_direction) {
    int* tmp_queue;

    switch(driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            if(next_floor_negotiator_is_empty(driving_direction)){
                tmp_queue = &down_queue[0];
            } else{
                tmp_queue = &up_queue[0];
            }            
            break;
        case HARDWARE_MOVEMENT_DOWN:
            if(next_floor_negotiator_is_empty(driving_direction)){
                tmp_queue = &up_queue[0];
            }else{
                tmp_queue = &down_queue[0];
            }
            break;
        default:
            break;
    }
    for (int f = current_floor; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (tmp_queue[f]) {
            return 1;//order above
        }
    }
    for(int g = 0; g < current_floor; g++){
        if(tmp_queue[g]){
            return -1; //order underneath
        }
    }
    return 0;//no orders
}

void next_floor_negotiator_clear_queues() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
        up_queue[f] = 0;
        down_queue[f] = 0;
    }
}

int next_floor_negotiator_at_next_floor(int next_floor) {
    if (next_floor == -1) {
        return 0;
    }
    if (hardware_read_floor_sensor(next_floor)) {
        return 1;
    }
    return 0;
}



int next_floor_negotiator_get_next_floor(HardwareMovement driving_direction) {
    int* tmp_queue;
    switch (driving_direction) {
        case HARDWARE_MOVEMENT_DOWN:
            if(next_floor_negotiator_is_empty(driving_direction)){
                tmp_queue = &up_queue[0];
            } else{
                tmp_queue = &down_queue[0];
            }            
            break;
        case HARDWARE_MOVEMENT_UP:
            if(next_floor_negotiator_is_empty(driving_direction)){
                tmp_queue = &down_queue[0];
            }else{
                tmp_queue = &up_queue[0];
            }            
            break;
        default:
            break;
    }
    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (tmp_queue[f] == 1) {
            return f;
        }
    }
    return -1;
}

void next_floor_negotiator_remove_order(int floor, HardwareMovement driving_direction) {
    int* tmp_queue;
    switch (driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
            hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
            tmp_queue = up_queue;
            break;
        case HARDWARE_MOVEMENT_DOWN:
            hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
            hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
            tmp_queue = down_queue;
            break;
        default:
            break;
    }
    tmp_queue[floor] = 0;
}