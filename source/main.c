#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "NextFloorNegotiator.h"

#define OPEN 1
#define CLOSED 0
#define UNDEFINED -1




typedef enum {
    DRIVE_DOWN,
    IDLE,
    DOOR_OPEN,
    DRIVE_UP,
    STOP
} State;

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

void poll_floor_indicator(int* current_floor, int* between_floors, HardwareMovement driving_direction) {

    if (driving_direction == HARDWARE_MOVEMENT_STOP) {
        
    }
    int old_current_floor = *current_floor;
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (hardware_read_floor_sensor(f)) {
            hardware_command_floor_indicator_on(f);
            *between_floors = ATFLOOR;
            *current_floor = f;
            return;
        }
    }
    if (driving_direction == HARDWARE_MOVEMENT_DOWN && old_current_floor != current_floor) {
        *between_floors = BELOW;
    } else if (driving_direction == HARDWARE_MOVEMENT_UP) {
        *between_floors = ABOVE;
    } else if(driving_direction == HARDWARE_MOVEMENT_STOP && old_current_floor == current_floor){

    }
}


static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
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
    static int current_floor = UNDEFINED;
    static int between_floors;
    static int door_open = CLOSED;
    int order_above;

    // Initial move down
    nfn_clear_queue();
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

    while (current_floor == UNDEFINED) {
        poll_floor_indicator(&current_floor, &between_floors, driving_direction);
    }
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    current_state = IDLE;

    while (1) {
        hardware_command_stop_light(0);
        nfn_poll_order_sensors();
        poll_floor_indicator(&current_floor, &between_floors, driving_direction);

        if (door_open) {
            current_state = DOOR_OPEN;
        }

        if (hardware_read_stop_signal()) {
            current_state = STOP;
        }



        switch (current_state) {
            case IDLE:

                order_above = nfn_order_above(current_floor, between_floors, driving_direction);

                if (nfn_at_ordered_floor(current_floor) && between_floors == ATFLOOR){
                    hardware_command_door_open(1);
                    nfn_remove_order(current_floor, driving_direction);
                    timer = start_timer();
                    door_open = OPEN;                    
                    current_state = DOOR_OPEN;
                    break;
                } else if (order_above == ABOVE) {
                    hardware_command_movement(HARDWARE_MOVEMENT_UP);
                    driving_direction = HARDWARE_MOVEMENT_UP;
                    current_state = DRIVE_UP;
                    break;
                } else if (order_above == BELOW){
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
                    door_open = CLOSED;
                    current_state = IDLE;
                    break;
                }
                break;
            
            case DRIVE_UP:
                if (nfn_at_ordered_floor(current_floor) && (between_floors == ATFLOOR)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case DRIVE_DOWN:
                if (nfn_at_ordered_floor(current_floor) && (between_floors == ATFLOOR)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case STOP:
                if (between_floors != ATFLOOR) {
                    driving_direction = HARDWARE_MOVEMENT_STOP;
                }

                nfn_clear_queue();
                hardware_command_stop_light(1);
                timer = start_timer();
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                current_state = IDLE; 
                break;
        }
    }

    return 0;
}
