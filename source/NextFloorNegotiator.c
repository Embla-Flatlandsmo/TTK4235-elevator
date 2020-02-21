#include "NextFloorNegotiator.h"


int getGoalFloor() {
    if (drivingDirection == HARDWARE_MOVEMENT_UP) {
        if (!isEmpty(&QueueUp)) {                                   //There are more orders in the upwards queue
            return *(int*)readFirstNode(&QueueUp);
        } else if (isEmpty(&QueueUp) && !isEmpty(&QueueDown)){      //We are at the end of the upwards queue and there are orders in the downwards queue
            drivingDirection = HARDWARE_MOVEMENT_DOWN;              //Change direction and start reading from the other queue
            return *(int*)readFirstNode(&QueueDown);
        }
    } else if (drivingDirection == HARDWARE_MOVEMENT_DOWN) {
        if (!isEmpty(&QueueDown)) {                                 //There are more orders in the downward direction
           return *(int*)readFirstNode(&QueueDown);            
        } else if (isEmpty(&QueueDown) && !isEmpty(&QueueUp)) {     //we are at the end of the queue
            drivingDirection = HARDWARE_MOVEMENT_UP;                //Change direction and start reading from the other queue
            return *(int*)readFirstNode(&QueueUp);
        }
    } else {
        return -1;
    }
}

void newFloorOrder(struct Node** head_ref) {
    //get the front of the list from readOrders()
    FloorOrder tempOrder = *(FloorOrder*)pop(head_ref);
    struct Node* currentNode = newNode(tempOrder.floor);
    
    //sort the entry into its own list according to what type of order it is
    if (tempOrder.orderType == HARDWARE_ORDER_DOWN) {
        descendingInsert(&QueueDown, currentNode);
    }
    else if (tempOrder.orderType == HARDWARE_ORDER_UP) {
        ascendingInsert(&QueueUp, currentNode);
    }
    else if (tempOrder.orderType == HARDWARE_ORDER_INSIDE) {
        if (currentFloor < tempOrder.floor) {
            ascendingInsert(&QueueUp, currentNode);
        }
        else if (currentFloor > tempOrder.floor) {
            descendingInsert(&QueueDown, currentNode);
        }
    }
}