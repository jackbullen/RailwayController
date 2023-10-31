#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "train.h"

typedef struct Node {
    void *data;
    struct Node *next;
} Node;
Node* create_node(void *data);
void free_node(Node *head);

typedef struct LinkedList {
    Node *head;
    int size;
} LinkedList;
LinkedList* create_linked_list();
void insert_end(LinkedList *list, void *data);
void insert_front(LinkedList *list, void *data);
void* delete_beginning(LinkedList *list);
void free_list(LinkedList *list);

#endif