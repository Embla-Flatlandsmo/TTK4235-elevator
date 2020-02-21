#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "hardware.h"
#include "ElevatorStateMachine.h"

struct Node* QueueUp;
struct Node* QueueDown;
struct Node* NextQueueUp;
struct Node* NextQueueDown;
/**
 * @brief Figures out which floor to go to next, also uses checkifTurn to decide if the elevator has to turn or not.
 * @return -1 if there are no floor orders, otherwise it returns the next floor.
 */
int getGoalFloor();

/**
 * @brief Checks if the Elevator has to turn. Decides when to use @p turnElevator
 */
void checkifTurn();

/**
 * @brief Sets driving direction and moves the items from NextQueueUp/Down to QueueUp/Down
 * @param upOrDown decides whether to start driving up or down
 */
void turnElevator(HardwareMovement upOrDown);

/**
 * @brief Clears all the queues (QueueUp, QueueDown, NextQueueUp, NextQueueDown). Used for STOP state.
 */
void clearAllQueues();


/**
 * @brief Takes the current floor orders from the state machine and puts them in the correct queues.
 * @param head_ref pointer to the head of the current floor orders being made.
 */
void newFloorOrder(struct Node** head_ref);

