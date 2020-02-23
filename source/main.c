#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "NextFloorNegotiator.h"
#include "ElevatorStateMachine.h"

static State ElevatorState;


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
    ElevatorState = INITIAL_CHECK;
    *goalFloor = -1;
    *currentFloor = 0;
    *drivingDirection = HARDWARE_MOVEMENT_STOP;

    FloorOrders = NULL;
    QueueUp = NULL;
    QueueDown = NULL;
    NextQueueUp = NULL;
    NextQueueDown = NULL;

    while(1) {
        stateMachine(ElevatorState);
    }

    return 0;
}
