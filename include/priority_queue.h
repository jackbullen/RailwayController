#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "linked_list.h"
#include "train.h"

typedef struct PriorityQueue {
    LinkedList *list;
} PriorityQueue;
typedef int (*comparator_fn)(void*, void*);
PriorityQueue* create_priority_queue();
void enqueue(PriorityQueue *pq, void *data, comparator_fn comparator);
Train* dequeue(PriorityQueue *pq);
void free_priority_queue(PriorityQueue *pq);

#endif