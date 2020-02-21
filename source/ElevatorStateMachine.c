#include "ElevatorStateMachine.h"

ElevatorState = INITIAL_CHECK;

State stateMachine(State currentState) 
{
    switch(ElevatorState) {

        case INITIAL_CHECK:
            hardware_init(); //add bulletproofing???
            drivingDirection = HARDWARE_MOVEMENT_STOP;
            hardware_command_movement(drivingDirection);
            int floorSensorActive = checkAllFloorSensors();

            if (floorSensorActive) {
                ElevatorState = IDLE;
            } else {
                ElevatorState = INITIAL_DRIVE_DOWN;
            }
            break;

        case INITIAL_DRIVE_DOWN:
            drivingDirection = HARDWARE_MOVEMENT_DOWN;
            hardware_command_movement(drivingDirection);
            int pollingSensors = 1;
            while(pollingSensors) {
                currentFloor = checkAllFloorSensors();
                if (currentFloor) {
                    ElevatorState = IDLE;
                    hardware_command_floor_indicator_on(currentFloor);
                    pollingSensors = 0;
                }
            }
            break;

        case IDLE:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            int pollingSensors = 1;
            time_t start_time;
            int firstLoop = 1;

            if (hardware_read_floor_sensor(goalFloor)) {
                hardware_command_door_open(1);
                start_time = start_time();
            }

            while(pollingSensors){
                FloorOrders = readOrders();

                if (!isEmpty(&FloorOrders)) {    //If buttons are being pushed, put them in the correct queues
                    newFloorOrder(&FloorOrders);
                }
                goalFloor = getGoalFloor();
                if (hardware_read_stop_signal()) {
                    ElevatorState = STOP;
                    break;
                }
                
                if (timer_expired(start_time) == 0 && hardware_read_obstruction_signal()) {
                    start_time = start_time();
                }

                if (timer_expired(start_time) == 1) {
                    if (firstLoop) { //Checks if this is the first time that the code is being run through (to prevent deleteFirstNode from deleting the whole queue)
                        hardware_command_door_open(0);
                        if (drivingDirection == HARDWARE_MOVEMENT_UP) { //We turn off the order lights and delete the first order in the queue when the doors close
                            hardware_command_order_light(currentFloor, HARDWARE_ORDER_UP, 0);
                            hardware_command_order_light(currentFloor, HARDWARE_ORDER_INSIDE, 0);
                            deleteFirstNode(&QueueUp);
                        } else if (drivingDirection == HARDWARE_MOVEMENT_DOWN) {
                            hardware_command_order_light(currentFloor, HARDWARE_ORDER_DOWN, 0);
                            hardware_command_order_light(currentFloor, HARDWARE_ORDER_INSIDE, 0);
                            deleteFirstNode(&QueueDown);
                        }

                        goalFloor = getGoalFloor(); //Get new goal floor
                        firstLoop = 0;
                        }
                   


                    if (currentFloor > goalFloor && goalFloor != -1) {
                        ElevatorState = DRIVE_DOWN;
                        pollingSensors = 0;
                    }

                    else if (currentFloor < goalFloor && goalFloor != -1) {
                        ElevatorState = DRIVE_UP;
                        pollingSensors = 0;
                    }
                }
            }
            break;
            
        case DRIVE_DOWN:
            drivingDirection = HARDWARE_MOVEMENT_DOWN;
            hardware_command_movement(drivingDirection);
            int pollingSensors = 1;

            while(pollingSensors) {

                if (checkAllFloorSensors() != 0) { //update floor light and currentFloor
                    currentFloor = checkAllFloorSensors();
                    hardware_command_floor_indicator_on(currentFloor);
                }

                FloorOrders = readOrders(); //Check if new orders are being made
                if (!isEmpty(&FloorOrders)) { //If buttons are being pushed, put them in the right lists
                    newFloorOrder(&FloorOrders); 
                }

                goalFloor = getGoalFloor(); //Check which floor to go to next
                if (hardware_read_stop_signal()) {
                    ElevatorState = STOP;
                    break;
                }

                if (hardware_read_floor_sensor(goalFloor)) {
                    ElevatorState = IDLE;
                    pollingSensors = 0;
                }

            }
            break;

        case DRIVE_UP:
            drivingDirection = HARDWARE_MOVEMENT_UP;
            hardware_command_movement(drivingDirection);
            int pollingSensors = 1;
            while(pollingSensors) {

                if (checkAllFloorSensors() != 0) {
                    currentFloor = checkAllFloorSensors();
                    hardware_command_floor_indicator_on(currentFloor);
                }

                FloorOrders = readOrders();
                if (!isEmpty(&FloorOrders)) {
                    newFloorOrder(&FloorOrders);
                }
                goalFloor = getGoalFloor(); 
                if (hardware_read_stop_signal()) {
                    ElevatorState = STOP;
                    break;
                }

                if (hardware_read_floor_sensor(goalFloor)) {
                    ElevatorState = IDLE;
                    pollingSensors = 0;
                }
            }
            break;
        
        case STOP:
            hardware_command_stop_light(1);
            while (hardware_read_stop_signal());
            hardware_command_stop_light(0);
            ElevatorState = IDLE;
            break;
    }
}


struct Node* readOrders() {
    deleteList(&FloorOrders);
    free(FloorOrders)
    struct Node* head = NULL;
    for (int floor = 0; floor <= HARDWARE_NUMBER_OF_FLOORS; floor++) {
        for (int ordertype = 1; ordertype <= 3; ordertype++) {
            if (hardware_read_order(floor, ordertype)) {
                hardware_command_order_light(floor, ordertype, 1);
                FloorOrder f = {floor, ordertype};
                push(&head, newNode(&f));
            }
        }
    }
    return head;
}

time_t start_time() {
	return time(NULL); 
}

int timer_expired(time_t start_time){
	time_t end_time = time(NULL);
	if (end_time - start_time >= 3){ 
		return 1; 
	}
	return 0;
}

int checkAllFloorSensors() {
    for (int i = 1; i <= HARDWARE_NUMBER_OF_FLOORS; i++) {
        if (hardware_read_floor_sensor(i)) {
            return i;
        } else {
            return 0;
        }
    }
}