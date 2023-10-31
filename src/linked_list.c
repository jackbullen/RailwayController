#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

LinkedList* create_linked_list() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (!list) return NULL;

    list->head = NULL;
    list->size = 0;
    return list;
}

void free_list(LinkedList *list) {
    if (!list) return;
    free_node(list->head);
    free(list);
}

Node* create_node(void *data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) return NULL;

    new_node->data = data;
    new_node->next = NULL;
    
    return new_node;
}

void insert_end(LinkedList *list, void *data) {
    // if no head exists, create one
    if(!list->head) {
        list->head = create_node(data);
    } else {
        Node *curr = list->head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = create_node(data);
    }
    list->size++;
}

void insert_front(LinkedList *list, void *data) {
    Node *new_head = create_node(data);
    new_head->next = list->head;
    list->head = new_head;
    list->size++;
}

void* delete_beginning(LinkedList *list) {
    if (!list->head) return NULL;

    Node *temp = list->head;
    void *data = temp->data;
    list->head = list->head->next;
    free(temp);
    list->size--;
    return data;
}

void free_node(Node *head) {
    while(head) {
        Node *temp = head;
        head = head->next;
        free(temp->data);
        free(temp);
    }
}