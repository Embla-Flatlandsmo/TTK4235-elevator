/**
 * @file
 * @brief Data structure for linked lists.
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <stdio.h>
#include "hardware.h"


/**
 * @brief Struct that gives which floor is being ordered, and which type of order it is.
 */
typedef struct {
    int floor;
    HardwareOrder orderType;
} FloorOrder;

//Linked list node
struct Node 
{ 
    FloorOrder data; 
    struct Node* next; 
}; 

/**
 * @brief Utility function for creating a new node.
 * @param new_data Data to be contained in node.
 * @return Pointer to the new node.
 */
struct Node *newNode(FloorOrder new_data);


/**
 * @brief Checks if the linked list is empty.
 * @param head_ref Pointer to the head pointer.
 * @return 1 if list is empty, 0 otherwise.
 */
int isEmpty(struct Node** head_ref);

/**
 * @brief Deletes the entire list and frees up the memory.
 * @param head_ref Pointer to the head pointer.
 * @warning Does not delete the head pointer. This needs to be deleted manually.
 */
void deleteList(struct Node** head_ref);

/**
 * @brief Pushes a new node to the front of the list.
 * @param head_ref Pointer to the head pointer.
 * @param new_data data to be contained in the new node.
 */
void push(struct Node** head_ref, FloorOrder new_data);

/**
 * @brief Reads the data in the head of the list.
 * @param head_ref Pointer to the head pointer.
 * @return Value of the data in the head. -1 if the list is empty.
 * @warning Does not delete the first node.
 */
FloorOrder readFirstNode(struct Node** head_ref);


/**
 * @brief Deletes the head node and sets the predecessor to 
 * @param head_ref Pointer to the head pointer.
 */
void deleteFirstNode(struct Node** head_ref);


/**
 * @brief Retrieves the node at the front of the list and deletes it.
 * @param head_ref Pointer to the head pointer.
 * @return -1 if list is empty, otherwise data contained in the node at the front of the list.
 */
FloorOrder pop(struct Node** head_ref);

/**
 * @brief Inserts new node in ascending order (ex. 1->3->8)
 * @param head_ref Pointer to the head pointer.
 * @param new_node New node to be inserted into the list.
 * @warning Only use nodes with data types that can be sorted such as floats, ints or char.
 */
void ascendingInsert(struct Node** head_ref, struct Node* new_node); 

/**
 * @brief Inserts new node in descending order (ex. 8->3->1)
 * @param head_ref Pointer to the head pointer.
 * @param new_node New node to be inserted into the list.
 * @warning Only use nodes with data types that can be sorted such as floats, ints or char.
 */
void descendingInsert(struct Node** head_ref, struct Node* new_node); 

#endif // LINKEDLIST_H