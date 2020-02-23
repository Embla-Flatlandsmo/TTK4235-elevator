#include "LinkedList.h"




//A utility function to create a new node */
struct Node *newNode(FloorOrder new_data) 
{ 
    //allocate node
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
  
    //put in the data
    new_node->data  = new_data; 
    new_node->next =  NULL; 
  
    return new_node; 
} 


int isEmpty(struct Node** head_ref) {
    struct Node* head = *head_ref;
    if (head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void deleteList(struct Node** head_ref) {

    //Deref head_ref to get the real head
    struct Node* current = *head_ref;
    struct Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    //Deref head ref to affect the real head back to the caller(?)
    *head_ref = NULL;
}

void push(struct Node** head_ref, FloorOrder new_data) {

    //Allocate the new Node in the heap and set its data
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    //put in the data
    new_node->data = new_data;

    //link old list off the new node
    new_node->next = (*head_ref);

    //move head to point to the new node
    (*head_ref) = new_node;

}

FloorOrder readFirstNode(struct Node** head_ref) {
    return (*head_ref)->data;
}

void deleteFirstNode(struct Node** head_ref) {
    if (*head_ref == NULL) {
        return;
    }

    struct Node* head = *head_ref;
    (*head_ref) = (*head_ref)->next;
    free(head);
    return;
}

FloorOrder pop(struct Node** head_ref) {
    //underflow condition
    if (*head_ref == NULL) {
        FloorOrder f = {-1,1}
        return f;
    }

    struct Node* head = *head_ref;
    FloorOrder result = head->data; //pull out data before node is deleted

    (*head_ref) = (*head_ref)->next; //unlink head node for the caller

    free(head); //free the head node

    return result;

}

void ascendingInsert(struct Node** head_ref, struct Node* new_node) 
{
    struct Node* current;
    //Special cases for head (insert new node before head, or if this is the first insert)
    if (*head_ref == NULL || (*head_ref)->data.floor > new_node->data.floor) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    } else {
        //Locate node before point of insertion
        current = *head_ref;
        while (current->next != NULL && current->next->data.floor < new_node->data.floor) {
            current = current->next;
        }
        if (current->next != NULL && current->data.floor == current->next->data.floor) {  //This means that there is already an order like it in the list
            return;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

void descendingInsert(struct Node** head_ref, struct Node* new_node) 
{
    struct Node* current;
    //Special cases for head (insert new node before head, or if this is the first insert)
    if (*head_ref == NULL || (*head_ref)->data.floor < new_node->data.floor) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    } else {
        //Locate node before point of insertion
        current = *head_ref;
        while (current->next != NULL && current->next->data.floor > new_node->data.floor) {
            current = current->next;
        }
        if (current->next != NULL && current->data.floor == current->next->data.floor) {
            return;
        }

        new_node->next = current->next;
        current->next = new_node;
    }
}
