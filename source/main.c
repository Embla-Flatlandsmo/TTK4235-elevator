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

int poll_floor_indicator(int current_floor) {
    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (hardware_read_floor_sensor(f)) {
            return f;
        }
    }
    return current_floor;
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
    int next_floor;

    // Initial move down
    HardwareMovement driving_direction = HARDWARE_MOVEMENT_DOWN;
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    int current_floor = -1;
    while (current_floor == -1) {
        current_floor = poll_floor_indicator(current_floor);
    }
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    current_state = IDLE;

    while (1) {
        hardware_command_stop_light(0);
        next_floor_negotiator_poll_order_sensors();
        current_floor = poll_floor_indicator(current_floor); 
        next_floor = next_floor_negotiator_get_next_floor(driving_direction);


        if (hardware_read_stop_signal()) {
            current_state = STOP;
        }

        switch (current_state) {
            case IDLE:
                int order_above = next_floor_negotiator_order_above(current_floor, driving_direction);
              
                if (next_floor_negotiator_at_next_floor(next_floor)) {
                    hardware_command_door_open(1);
                    next_floor_negotiator_remove_order(next_floor, driving_direction);
                    next_floor = next_floor_negotiator_get_next_floor(driving_direction);
                    timer = start_timer();
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
                    current_state = IDLE;
                }

                break;
            
            case DRIVE_UP:
                if (next_floor_negotiator_at_next_floor(next_floor)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case DRIVE_DOWN:
                if (next_floor_negotiator_at_next_floor(next_floor)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case STOP:
                next_floor_negotiator_clear_queues();
                hardware_command_stop_light(1);
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                current_state = IDLE; //getting out of STOP
                break;
        }
    }

    return 0;
}
