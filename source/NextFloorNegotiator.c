#include "NextFloorNegotiator.h"

void turnElevator(HardwareMovement upOrDown) {
    if (upOrDown == HARDWARE_MOVEMENT_UP) {
        *drivingDirection = HARDWARE_MOVEMENT_UP;
        while (!isEmpty(&NextQueueUp)) {
            ascendingInsert(&QueueUp, pop(&NextQueueUp));
        }
    } else if (upOrDown == HARDWARE_MOVEMENT_DOWN) {
        *drivingDirection = HARDWARE_MOVEMENT_DOWN;
        while (!isEmpty(&NextQueueDown)) {
            descendingInsert(&QueueDown, pop(&NextQueueDown));
        }
    }
}

void checkifTurn() {
    if (*drivingDirection == HARDWARE_MOVEMENT_UP) {
        if (isEmpty(&QueueUp) && !isEmpty(&QueueDown)) {
            turnElevator(HARDWARE_MOVEMENT_DOWN);
    		return;
        }
        else if (isEmpty(&QueueUp) && isEmpty(&QueueDown) && !isEmpty(&NextQueueUp)) {
            turnElevator(HARDWARE_MOVEMENT_UP);
            return;
        }
    } else if (*drivingDirection == HARDWARE_MOVEMENT_DOWN) {
        if (isEmpty(&QueueDown) && !isEmpty(&QueueUp)){
            turnElevator(HARDWARE_MOVEMENT_DOWN);
        } else if (isEmpty(&QueueDown) && isEmpty(&QueueUp) && !isEmpty(&NextQueueDown)) {
            turnElevator(HARDWARE_MOVEMENT_DOWN);
        }
    }
}

int getGoalFloor() {
    checkifTurn();
    if (*drivingDirection == HARDWARE_MOVEMENT_UP) {
        if (!isEmpty(&QueueUp)) {                                   //There are more orders in the upwards queue
            return readFirstNode(&QueueUp).floor;
        } else if (isEmpty(&QueueUp) && !isEmpty(&QueueDown)){      //We are at the end of the upwards queue and there are orders in the downwards queue
            return readFirstNode(&QueueDown).floor;
        }
    } else if (*drivingDirection == HARDWARE_MOVEMENT_DOWN) {
        if (!isEmpty(&QueueDown)) {                                 //There are more orders in the downward direction
           return readFirstNode(&QueueDown).floor;            
        } else if (isEmpty(&QueueDown) && !isEmpty(&QueueUp)) {     //we are at the end of the queue
            *drivingDirection = HARDWARE_MOVEMENT_UP;                //Change direction and start reading from the other queue
            return readFirstNode(&QueueUp).floor;
        }
    }
    
    return -1;
}

void newFloorOrder(struct Node** head_ref) {

    if ((*head_ref) == NULL) {
        return;
    }
    //get the front of the list from readOrders()
    FloorOrder tempOrder = pop(head_ref);
    struct Node* currentNode = newNode(tempOrder);
    
    //sort the entry into its own list according to what type of order it is
    if (tempOrder.orderType == HARDWARE_ORDER_DOWN) {
        if (tempOrder.floor > *currentFloor) {
            descendingInsert(&NextQueueDown, currentNode); //if someone wants to go down but is above the current floor, we put it in a different queue
        } else {
            descendingInsert(&QueueDown, currentNode);
        }

    }
    else if (tempOrder.orderType == HARDWARE_ORDER_UP) {
        if (tempOrder.floor < *currentFloor && *drivingDirection == HARDWARE_MOVEMENT_UP) {
            ascendingInsert(&NextQueueUp, currentNode);
        } else {
            ascendingInsert(&QueueUp, currentNode);            
        }

    }
    else if (tempOrder.orderType == HARDWARE_ORDER_INSIDE) {
        if (*drivingDirection == HARDWARE_MOVEMENT_UP) {
            if (tempOrder.floor > *currentFloor) {
                ascendingInsert(&QueueUp, currentNode);                
            } else {
                descendingInsert(&QueueDown, currentNode);
            }
        } else if (*drivingDirection == HARDWARE_MOVEMENT_DOWN) {
            if (tempOrder.floor < *currentFloor) {
                descendingInsert(&QueueDown, currentNode);
            } else {
                ascendingInsert(&QueueUp, currentNode);
            }
            descendingInsert(&QueueDown, currentNode);
        }
    }
}

void clearAllQueues() {
    deleteList(&QueueUp);
    deleteList(&NextQueueUp);
    deleteList(&QueueDown);
    deleteList(&NextQueueDown);
}