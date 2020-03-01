/**
 * @file
 * @brief File containing the state machine for the elevator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "hardware.h"
#include "ordermanager.h"

#define OPEN 1
#define CLOSED 0
#define UNDEFINED -1



/**
 * @brief State type used in the state machine.
 */
typedef enum {
    DRIVE_DOWN,
    IDLE,
    DOOR_OPEN,
    DRIVE_UP,
    STOP
} State;


/**
 * @brief Function for starting the timer.
 * @return Starting time.
 */
time_t start_timer() {
	return time(NULL); 
}

/**
 * @brief Function for checking if the timer has expired.
 * @param[in] startTime Starting time
 * @return 1 if 3 seconds have passed since @p startTime, 0 otherwise.
 */
int timer_expired(time_t startTime){
	time_t end_time = time(NULL);
	if (end_time - startTime >= 3){ 
		return 1; 
	}
	return 0;
}
/**
 * @brief Polls all the floor indicators and updates the 
 * @param[in, out]  current_floor Variable for storing which floor was last registered.
 * @param[in, out]  between_floors Variable for storing whether we are above (1), below (-1) or at (0) a floor.
 * @param[in]   driving_direction Elevator's driving direction
 * @param[in, out]  stopped_between Variable for remembering whether the stop-button has been pushed between a floor (edge-case scenario)
 */
void poll_floor_indicators(int* current_floor, int* between_floors, HardwareMovement driving_direction, int* stopped_between) {

    for (int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++) {
        if (hardware_read_floor_sensor(f)) {
            hardware_command_floor_indicator_on(f);
            *between_floors = ATFLOOR;
            *current_floor = f;
            *stopped_between = 0;
            return;
        }
    }
    if(*stopped_between){
        return;
    } else if (driving_direction == HARDWARE_MOVEMENT_DOWN) {
        *between_floors = BELOW;
    } else if (driving_direction == HARDWARE_MOVEMENT_UP) {
        *between_floors = ABOVE;
    }
}

/**
 * @brief Function for handling interrupts, results in program termination.
 */
static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

State current_state;
time_t timer;
HardwareMovement driving_direction;
 /** @brief  Variable for storing the last registered floor */
static int current_floor;
/** @brief Variable for storing whether we are above, below or at a floor */
static int between_floors;
/** @brief Variable for remembering whether or not the door has been opened. */
static int door_open;
/** @brief Variable for storing if we have an order above, below or no order. */
static int order_above;
/** @brief Variable for remembering whether the elevator has been stopped between floors. */
static int stopped_between;

/**
 * @brief This is where shit gets real. Contains the state machine :)
 */
int main(){
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);


    driving_direction = HARDWARE_MOVEMENT_DOWN;
    current_floor = UNDEFINED;
    door_open = CLOSED;
    stopped_between = 0;

    // Initial move down
    ordermanager_clear_queue();
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

    while (current_floor == UNDEFINED) {
        poll_floor_indicators(&current_floor, &between_floors, driving_direction, &stopped_between);
    }
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    current_state = IDLE;

    while (1) {
        hardware_command_stop_light(0);
        ordermanager_poll_order_sensors();
        poll_floor_indicators(&current_floor, &between_floors, driving_direction, &stopped_between);        
        
        if (door_open) {
            current_state = DOOR_OPEN;
        }

        if (hardware_read_stop_signal()) {
            current_state = STOP;
        }



        switch (current_state) {
            case IDLE:

                order_above = ordermanager_order_above_or_below(current_floor, between_floors, driving_direction);

                if (ordermanager_at_ordered_floor(current_floor, driving_direction) && between_floors == ATFLOOR){
                    hardware_command_door_open(1);
                    ordermanager_remove_order(current_floor, driving_direction);
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
                if (ordermanager_at_ordered_floor(current_floor, driving_direction) && (between_floors == ATFLOOR)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case DRIVE_DOWN:
                if (ordermanager_at_ordered_floor(current_floor, driving_direction) && (between_floors == ATFLOOR)) {
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    current_state = IDLE;
                    break;
                }
                break;

            case STOP:
                if (between_floors != ATFLOOR) {
                    driving_direction = HARDWARE_MOVEMENT_STOP;
                    stopped_between = 1;
                }
                
                ordermanager_clear_queues();
                hardware_command_stop_light(1);
                timer = start_timer();
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                current_state = IDLE; 
                break;
        }
    }

    return 0;
}
