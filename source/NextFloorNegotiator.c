#include "NextFloorNegotiator.h"




int getGoalFloor() {
    if (drivingDirection == HARDWARE_MOVEMENT_UP && !isEmpty(&QueueUp)) {
        return *(int*)readFirstNode(&QueueUp);
    } else if (drivingDirection == HARDWARE_MOVEMENT_DOWN && !isEmpty(&QueueDown)) {
        return *(int*)readFirstNode(&QueueDown);
    } else {
        return -1;
    }
}

void floorReached() {
    if (drivingDirection == HARDWARE_MOVEMENT_UP && !isEmpty(&QueueUp)) {
        deleteFirstNode(&QueueUp);
    } else if (drivingDirection == HARDWARE_MOVEMENT_DOWN && !isEmpty(&QueueDown)) {
        deleteFirstNode(&QueueDown);
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
        } else { //What if we are already in the floor that the passenger is requesting?
            if (drivingDirection == HARDWARE_MOVEMENT_DOWN) {
                push(&QueueDown, currentNode);
            } else if (drivingDirection == HARDWARE_MOVEMENT_UP) {
                push(&QueueUp, currentNode);
            } else {
                push(&QueueDown, currentNode);
            }
        }
    }
}