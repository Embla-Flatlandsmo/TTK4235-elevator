#include "ElevatorStateMachine.h"

ElevatorState = INITIAL_CHECK;


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
        currentFloor = goalFloor;
        hardware_command_floor_indicator_on(currentFloor);

        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        int pollingSensors = 1;
        time_t start_time;


        if (hardware_read_floor_sensor(goalFloor)) {
            hardware_command_door_open(1);
            start_time = start_time();
        }

        while(pollingSensors){
            FloorOrders = readOrders();

            if (hardware_read_stop_signal()) {
                ElevatorState = STOP;
                break;
            }
            
            if (timer_expired(start_time) == 0 && hardware_read_obstruction_signal()) {
                start_time = start_time();
            }

            if (timer_expired(start_time) == 1) {
                hardware_command_door_open(0);
                if (currentFloor < goalFloor) {
                    ElevatorState = DRIVE_DOWN;
                    break;
                }

                if (currentFloor > goalFloor) {
                    ElevatorState = DRIVE_UP;
                    break;
                }
            }
        }

        break;
        
    case DRIVE_DOWN:
        drivingDirection = HARDWARE_MOVEMENT_DOWN;
        hardware_command_movement(drivingDirection);
        int pollingSensors = 1;

        while(pollingSensors) {

            if (checkAllFloorSensors() != 0) {
                currentFloor = checkAllFloorSensors();
                hardware_command_floor_indicator_on(currentFloor);
            }

            FloorOrders = readOrders();

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
            FloorOrders = readOrders();
            
            if (checkAllFloorSensors() != 0) {
                currentFloor = checkAllFloorSensors();
                hardware_command_floor_indicator_on(currentFloor);
            }

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



//please change list and names maybe
list<FloorOrder> readOrders() {
    list<FloorOrder> F_O;
    for (int i = 0; i =< HARDWARE_NUMBER_OF_FLOORS; i++) {
        for (int j = 1, j =< 3; j++) {
            if (hardware_read_order(i, j)) {
                FloorOrder f = {i,j};
                F_O.append(f);
            }
        }
    }
    return F_O;
}

time_t start_time(){
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
    for (int i = 1; i =< HARDWARE_NUMBER_OF_FLOORS; i++) {
        if hardware_read_floor_sensor(i) {
            return i;
        } else {
            return 0;
        }
    }
}