#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "NextFloorNegotiator.h"


typedef enum {
    DRIVE_DOWN,
    IDLE,
    DOOR_OPEN,
    DRIVE_UP,
    STOP
} State;
/*
typedef struct {
    int floor;
    int between;
} Position;
*/

time_t start_timer() {
	return time(NULL); 
}

int timer_expired(time_t startTime){
	time_t end_time = time(NULL);
	if (end_time - startTime >= 3){ 
		return 1; 
	}
	return 0;
}

int poll_floor_indicator(int current_floor, int* between_floors) {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (hardware_read_floor_sensor(f)) {
            hardware_command_floor_indicator_on(f);
            *between_floors = 0;
            return f;
        }
    }
    *between_floors = 1;
    return current_floor;
}


static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}
//use if temp_next_floor != next_floor after a stop between floors
int current_floor_cheat(int current_floor, int next_floor, HardwareMovement driving_direction){ 
    switch(driving_direction) {
        case HARDWARE_MOVEMENT_DOWN:
            if(next_floor == current_floor){
                current_floor -= 1; //move current floor down so it foesnt think its actually at next_floor yet bc its between
            } 
            break;
        case HARDWARE_MOVEMENT_UP:
            if(next_floor == current_floor){
                current_floor += 1; //move current_floor up
            }
            break;
        default:
            break;
    }
    return current_floor;
}

int main(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    State current_state;
    time_t timer;
    HardwareMovement driving_direction = HARDWARE_MOVEMENT_DOWN;
    //Position elevator_position = { -1, 1 };
    int current_floor = -1;
    int between_floors;
    int next_floor;
    int temp_next_floor;
    int door_open = 0;
    int order_above;

    // Initial move down
    nfn_clear_queues();

    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

    while (current_floor == -1) {
        current_floor = poll_floor_indicator(current_floor, &between_floors);
    }
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    current_state = IDLE;

    while (1) {
        hardware_command_stop_light(0);
        nfn_poll_order_sensors();
        next_floor = nfn_get_next_floor(current_floor, driving_direction);
        temp_next_floor = next_floor;
        current_floor = poll_floor_indicator(current_floor, &between_floors);

        if (door_open) {
            current_state = DOOR_OPEN;
        }

        if (hardware_read_stop_signal()) {
            temp_next_floor = next_floor; //save next floor before it is deleted to know which floors you stand between
            current_state = STOP;
        }



        switch (current_state) {
            case IDLE:

                if(between_floors && (temp_next_floor != next_floor)){
                    current_floor = current_floor_cheat(current_floor, next_floor, driving_direction);
                }

                order_above = nfn_order_above(current_floor, driving_direction);
                if (order_above == 0) {
                    break;
                } else if (next_floor == current_floor && !(between_floors)){
                    hardware_command_door_open(1);
                    nfn_remove_order(next_floor, driving_direction);
                    timer = start_timer();
                    door_open = 1;                    
                    current_state = DOOR_OPEN;
                    break;
                } else if (order_above == 1) {
                    hardware_command_movement(HARDWARE_MOVEMENT_UP);
                    driving_direction = HARDWARE_MOVEMENT_UP;
                    current_state = DRIVE_UP;
                    break;
                } else if (order_above == -1){
                    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                    driving_direction = HARDWARE_MOVEMENT_DOWN;
                    current_state = DRIVE_DOWN;
                    break;
                }  
                break;

            case DOOR_OPEN:
                if (hardware_read_obstruction_signal()) {
                    timer = start_timer();
                    break;
                }

                if (timer_expired(timer)) { //check if the timer has expired
                    hardware_command_door_open(0);
                    door_open = 0;
                    current_state = IDLE;
                    break;
                }
                break;
            
            case DRIVE_UP:
                if ((next_floor == current_floor) && (!(between_floors))) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case DRIVE_DOWN:
                if ((next_floor == current_floor) && (!(between_floors))) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case STOP:
                nfn_clear_queues();
                hardware_command_stop_light(1);
                timer = start_timer();
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                current_state = IDLE; 
                break;
        }
    }

    return 0;
}
