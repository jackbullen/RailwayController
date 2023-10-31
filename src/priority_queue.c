#include "priority_queue.h"
#include <stdlib.h>
#include <stdio.h>

PriorityQueue* create_priority_queue() {
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    pq->list = create_linked_list();
    return pq;
}

void enqueue(PriorityQueue *pq, void *data, comparator_fn comparator) {
    Node *node = create_node(data);
    if(!pq->list->head) {
        pq->list->head = node;
        pq->list->size++;
        return;
    }

    Node *temp = pq->list->head, *prev = NULL;
    while(temp) {
        // Here's where the logic was flipped. 
        // The new data should be inserted before the temp node if comparator returns a value less than 0.
        if (comparator(data, temp->data) > 0) {
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    if (!prev) {
        node->next = pq->list->head;
        pq->list->head = node;
    } else {
        node->next = prev->next;
        prev->next = node;
    }

    pq->list->size++;
}



Train* dequeue(PriorityQueue *pq) {
    return (Train *)delete_beginning((pq->list));
}

void free_priority_queue(PriorityQueue *pq) {
    free_node(pq->list->head);
    free(pq);
}