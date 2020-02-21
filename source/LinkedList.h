/**
 * @file
 * @brief Data structure for linked lists.
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Utility function for creating a new node.
 * @param new_data Data to be contained in node.
 * @return Pointer to the new node.
 */
struct Node *newNode(void* new_data);

/**
 * @brief Deletes the entire list and frees up the memory.
 * @param head_ref Pointer to the head pointer.
 */
void deleteList(struct Node** head_ref);

/**
 * @brief Checks if the linked list is empty.
 * @param head_ref Pointer to the head pointer.
 * @return 1 if list is empty, 0 otherwise.
 */
int isEmpty(struct Node** head_ref);

/**
 * @brief Pushes a new node to the front of the list.
 * @param head_ref Pointer to the head pointer.
 * @param new_data data to be contained in the new node.
 */
void push(struct Node** head_ref, void* new_data);

/**
 * @brief Retrieves the node at the front of the list and deletes it.
 * @param head_ref Pointer to the head pointer.
 * @return -1 if list is empty, otherwise data contained in the node at the front of the list.
 */
void* pop(struct Node** head_ref);

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