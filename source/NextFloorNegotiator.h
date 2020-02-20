#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "ElevatorStateMachine.h"

struct Node* QueueUp;
struct Node* QueueDown;
/**
 * @brief Figures out which floor to go to next
 * @return -1 if there are no floor orders, otherwise it returns the next floor.
 */
int getGoalFloor();

/**
 * @brief Takes the current floor orders from the state machine and puts them in the correct queues.
 * @param head_ref pointer to the head of the current floor orders being made.
 */
void newFloorOrder(struct Node** head_ref);

