#include "ElevatorStateMachine.h"

State ElevatorState = INITIAL_STATE;
global HARDWARE_MOVEMENT DrivingDirection;
int nextFloor;
list 
list<FloorOrder> FloorOrders;
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
        DrivingDirection = HARDWARE_MOVEMENT_STOP;
        hardware_command_movement(DrivingDirection);
        int pollingSensors = 1;
        while(pollingSensors){
            FloorOrders = readOrders();
            if(hardware_read_floor_sensor(nextFloor))
        }
        break;
        

    case DRIVE_DOWN:
        DrivingDirection = HARDWARE_MOVEMENT_DOWN;
        hardware_command_movement(DrivingDirection);
        int pollingSensors = 1;
        while(pollingSensors) {
            FloorOrders = readOrders();
            if (hardware_read_floor_sensor(nextFloor)) {
                ElevatorState = IDLE;
                pollingSensors = 0;
            }
        }
        break;

    case DRIVE_UP:
            DrivingDirection = HARDWARE_MOVEMENT_UP;
        hardware_command_movement(DrivingDirection);
        int pollingSensors = 1;
        while(pollingSensors) {
            FloorOrders = readOrders();
            if (hardware_read_floor_sensor(nextFloor)) {
                ElevatorState = IDLE;
                pollingSensors = 0;
            }
        }
        break;
    
    case STOP:
        break;
}