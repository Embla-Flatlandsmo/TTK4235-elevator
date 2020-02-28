#include "NextFloorNegotiator.h"

#define UP 1
#define DOWN -1
#define ABOVE 1
#define BELOW 0
#define DESCENDING 0
#define ASCENDING 1
#define NO_ORDER_FOUND -1

static int up_queue[HARDWARE_NUMBER_OF_FLOORS];
static int down_queue[HARDWARE_NUMBER_OF_FLOORS];


int nfn_is_empty(int up_or_down){ 
    int* tmp_queue;
    switch (up_or_down) {
        case UP:
            tmp_queue = &up_queue[0];
            break;
        case DOWN:
            tmp_queue = &down_queue[0];
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

void nfn_add_order(int floor, HardwareOrder order_type) {
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


int nfn_order_above(int current_floor, HardwareMovement driving_direction) {
    int* tmp_queue;

    switch(driving_direction) {
        case HARDWARE_MOVEMENT_UP:
            if(nfn_is_empty(UP)){
                tmp_queue = &down_queue[0];
            } else{
                tmp_queue = &up_queue[0];
            }            
            break;
        case HARDWARE_MOVEMENT_DOWN:
            if(nfn_is_empty(DOWN)){
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

void nfn_clear_queues() {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_UP, 0);
        hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 0);
        up_queue[f] = 0;
        down_queue[f] = 0;
    }
}


int nfn_read_array_ascending_or_descending(int* queue, int ascend_or_descend, int lower_index, int higher_index) {
    if (ascend_or_descend) {
        for (int i = lower_index; i < higher_index; i++) {
            if (queue[i]) {
                return i;
            }
        }
    } else {
        for (int i = higher_index-1; i >= lower_index; i--) {
            if (queue[i]) {
                return i;
            }
        }
    }
    return NO_ORDER_FOUND;
}


int nfn_find_next_order(int* queue, int check_above, int ascend_or_descend, int current_floor) {
    if (check_above) {
        return nfn_read_array_ascending_or_descending(queue, ascend_or_descend, current_floor, HARDWARE_NUMBER_OF_FLOORS);
    }
    if (!check_above) {
        return nfn_read_array_ascending_or_descending(queue, ascend_or_descend, 0, current_floor);
    }
    return NO_ORDER_FOUND;
}

int nfn_get_next_floor(int current_floor, HardwareMovement driving_direction) {
    if (nfn_is_empty(UP) && nfn_is_empty(DOWN)) {
        return NO_ORDER_FOUND;
    }

    int tmp_next_floor = NO_ORDER_FOUND;

    while (tmp_next_floor == NO_ORDER_FOUND) {
        switch(driving_direction) {
            case HARDWARE_MOVEMENT_UP:
                tmp_next_floor = nfn_find_next_order(up_queue, ABOVE, ASCENDING, current_floor);
                if (tmp_next_floor == NO_ORDER_FOUND) {
                    tmp_next_floor = nfn_find_next_order(down_queue, ABOVE, DESCENDING, current_floor);
                }
                if (tmp_next_floor == NO_ORDER_FOUND) {
                    driving_direction = HARDWARE_MOVEMENT_DOWN;
                } else {
                    return tmp_next_floor;
                }
                break;
            case HARDWARE_MOVEMENT_DOWN:
                tmp_next_floor = nfn_find_next_order(down_queue, BELOW, DESCENDING, current_floor);
                if (tmp_next_floor == NO_ORDER_FOUND) {
                    tmp_next_floor = nfn_find_next_order(up_queue, BELOW, ASCENDING, current_floor);
                }
                if (tmp_next_floor == NO_ORDER_FOUND) {
                    driving_direction = HARDWARE_MOVEMENT_UP;
                } else {
                    return tmp_next_floor;
                }
                break;
            default:
                return NO_ORDER_FOUND;
        }
    }
}

/*
int nfn_get_next_floor(int current_floor, HardwareMovement driving_direction) {
    int order_above_or_below = nfn_order_above(current_floor, driving_direction);
    int want_this;
    switch(driving_direction){
        case HARDWARE_MOVEMENT_UP:
            if (order_above_or_below == 1) {
                if (!nfn_is_empty(UP)) {
                    want_this = nfn_find_next_order(up_queue, 1, 1, current_floor);
                    if (want_this == -1) {
                        want_this = nfn_find_next_order(up_queue, 0, 1, current_floor);
                    }
                    return want_this;
                } else {
                    return nfn_find_next_order(down_queue, 1, 0, current_floor);
                }
            } else if (order_above_or_below == -1) {
                if (!nfn_is_empty(DOWN))  {
                    want_this = nfn_find_next_order(down_queue, 0, 0, current_floor);
                    if (want_this == -1) {
                        want_this = nfn_find_next_order(down_queue, 1, 0, current_floor);
                    }
                    return want_this;
                } else {
                    return nfn_find_next_order(up_queue, 0, 1, current_floor);
                }
            } else { return -1; }
            break;
        case HARDWARE_MOVEMENT_DOWN:
            if (order_above_or_below == -1) {
                if (!nfn_is_empty(DOWN)) {
                    want_this = nfn_find_next_order(down_queue, 0, 0, current_floor);//checking lowest part of q
                    if (want_this == -1) {
                        want_this = nfn_find_next_order(down_queue, 1, 0, current_floor);//checking highest part of q
                    } 
                    return want_this;
                } else {
                    return nfn_find_next_order(up_queue, 0, 1, current_floor);
                }
            } else if (order_above_or_below == 1) {
                if (!nfn_is_empty(UP))  {
                    want_this = nfn_find_next_order(up_queue, 1, 1, current_floor);
                    if (want_this == -1) {
                        want_this = nfn_find_next_order(up_queue, 0, 1, current_floor);
                    }
                    return want_this;
                } else {
                    return nfn_find_next_order(down_queue, 1, 0, current_floor);
                }
            } else { return -1; }
            break;
        default:
            return -1;
    }
         
}
*/


void nfn_remove_order(int floor, HardwareMovement driving_direction) {
    up_queue[floor] = 0;
    down_queue[floor] = 0;
    hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
}