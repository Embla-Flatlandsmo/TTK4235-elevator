#include "ElevatorStateMachine.h"

State ElevatorState = INITIAL_STATE;
global HARDWARE_MOVEMENT DrivingDirection;
int nextFloor;
list 

list<FloorOrder> readOrders() {
    list<FloorOrder> FloorOrders;
    for (int i = 0; i =< HARDWARE_NUMBER_OF_FLOORS; i++) {
        if (hardware_read_order(i, HARDWARE_ORDER_DOWN)) {
            
        }
    }
}



switch(ElevatorState) {
    case INITIAL_CHECK:
        DrivingDirection = HARDWARE_MOVEMENT_STOP;
        hardware_command_movement(DrivingDirection);
        int floorSensorActive;
        for (int i = 1; i =< HARDWARE_NUMBER_OF_FLOORS; i++) {
            floorSensorActive += hardware_read_floor_sensor(i);
        }

        if (floorSensorActive) {
            ElevatorState = IDLE;
        } else {
            ElevatorState = INITIAL_DRIVE_DOWN;
        }
        break;

    case INITIAL_DRIVE_DOWN:
        DrivingDirection = HARDWARE_MOVEMENT_DOWN;
        hardware_command_movement(DrivingDirection);
        int pollingSensors = 1;
        while(pollingSensors) {
            for (int i = 0; i =< HARDWARE_NUMBER_OF_FLOORS; i++) {
                if (hardware_read_floor_sensor(i)) {
                    ElevatorState = IDLE;
                    pollingSensors = 0;
                }
            }
        }
        break;

    case IDLE:

        break;

    case DRIVE_DOWN:
        DrivingDirection = HARDWARE_MOVEMENT_DOWN;
        hardware_command_movement(DrivingDirection);
        int pollingSensors = 1;
        while(pollingSensors) {
            if (hardware_read_floor_sensor(nextFloor)) {
                ElevatorState = IDLE;
                pollingSensors = 0;
            }
        }
        break;

    case DRIVE_UP:
        break;
    
    case STOP:
        break;
}