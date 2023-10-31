#include <stdlib.h>
#include <stdio.h>
#include "railway_controller.h"
#include <unistd.h>


RailwayController* create_railway_controller() {
    RailwayController *controller = (RailwayController *)malloc(sizeof(RailwayController));
    if (!controller) return NULL;

    controller->direction_count = 1;
    controller->last_direction = -1;

    pthread_mutex_init(&(controller->track_mutex), NULL);

    pthread_cond_init(&controller->ctrl_convar, NULL);
    pthread_cond_init(&(controller->track_available), NULL);
    pthread_cond_init(&(controller->track_convar), NULL);
    pthread_cond_init(&controller->westbound_convar, NULL);
    pthread_cond_init(&controller->eastbound_convar, NULL);
    pthread_cond_init(&controller->multiple_direction_convar, NULL);

    controller->westbound_queue = create_priority_queue();
    controller->eastbound_queue = create_priority_queue();

    return controller;
}

Train* get_next_train_to_signal(RailwayController *controller) {
    if (!controller->westbound_queue->list->head && !controller->eastbound_queue->list->head) {
        // No train in either queue
        return NULL;
    }

    // If only one queue has trains, return its head
    if (!controller->westbound_queue->list->head) {
        return (Train*)controller->eastbound_queue->list->head->data;
    }
    if (!controller->eastbound_queue->list->head) {
        return (Train*)controller->westbound_queue->list->head->data;
    }

    // Both queues have trains, decide based on priority and other logic
    Train* westbound_head = (Train*)controller->westbound_queue->list->head->data;
    Train* eastbound_head = (Train*)controller->eastbound_queue->list->head->data;


    // Prioritize by direction if three trains have gone in the same direction
    if (controller->direction_count >= 3) {
        printf("direction count: %d\n", controller->direction_count);
        if (controller->last_direction == 'E' || controller->last_direction == 'e') {
            return westbound_head;
        } else {
            return eastbound_head;
        }
    }

    if (westbound_head->priority == eastbound_head->priority) {
        // printf("equal\n");
        // Prioritize by direction if priorities are equal
        if (controller->last_direction == 'E' || controller->last_direction == 'e') {
            return westbound_head;
        } else {
            return eastbound_head;
        }
    }

    // Prioritize by train priority using train comparator
    if (train_comparator(westbound_head, eastbound_head) > 0) {
        // printf("westbound head: %d\n", westbound_head->number);
        return westbound_head;
    } else if (train_comparator(westbound_head, eastbound_head) < 0) {
        // printf("eastbound head: %d\n", eastbound_head->number);
        return eastbound_head;
    } else {
        // printf("equal\n");
        // Prioritize by direction if priorities are equal
        if (controller->last_direction == 'E' || controller->last_direction == 'e') {

            return westbound_head;
        } else {
            return eastbound_head;
        }
    }

    // // If direction count is less than 3, prioritize by train priority
    // if (westbound_head->priority < eastbound_head->priority) {
    //     return westbound_head;
    // } else if (westbound_head->priority > eastbound_head->priority) {
    //     return eastbound_head;
    // } else if (westbound_head->priority == eastbound_head->priority) {
    //     // Prioritize by direction if priorities are equal
    //     if (controller->last_direction == 'E' || controller->last_direction == 'e') {
    //         return westbound_head;
    //     } else {
    //         return eastbound_head;
    //     }
    // } else {
    //     // If priorities are equal, return the train in the opposite direction of the last train
    //     if (controller->last_direction == 'E' || controller->last_direction == 'e') {
    //         return westbound_head;
    //     } else {
    //         return eastbound_head;
    //     }
    // }
}


void request_access_to_track(Train *train, RailwayController *controller) {
    pthread_mutex_lock(&controller->track_mutex);

    PriorityQueue *this_direction_queue = train->direction == 'W' || train->direction == 'w' ? controller->westbound_queue : controller->eastbound_queue;

    enqueue(this_direction_queue, train, train_comparator);

    while (controller->is_occupied || train != (Train *)this_direction_queue->list->head->data) {
        pthread_cond_wait(&(train->self_convar), &controller->track_mutex);
    }

    if (controller->last_direction == train->direction) {
        controller->direction_count++;
    } else {
        controller->last_direction = train->direction;
        controller->direction_count = 1;
    }

    controller->is_occupied = 1;
    dequeue(this_direction_queue);
    pthread_mutex_unlock(&controller->track_mutex);
}





void release_track(Train *train, RailwayController *controller) {
    pthread_mutex_lock(&controller->track_mutex);
    controller->is_occupied = 0;

    Train *next_train = get_next_train_to_signal(controller);  // Assuming this function is defined
    if (next_train) {
        pthread_cond_signal(&(next_train->self_convar));
    }

    // Debugging print: current queues
    // printf("Westbound queue: ");
    Node *curr = controller->westbound_queue->list->head;
    while (curr) {
        // printf("%d ", ((Train *)curr->data)->number);
        curr = curr->next;
    }
    // printf("\n");

    // printf("Eastbound queue: ");
    curr = controller->eastbound_queue->list->head;
    while (curr) {
        // printf("%d ", ((Train *)curr->data)->number);
        curr = curr->next;
    }
    // printf("\n");

    pthread_mutex_unlock(&controller->track_mutex);
}


void free_railway_controller(RailwayController *controller) {
    pthread_mutex_destroy(&controller->track_mutex);
    pthread_cond_destroy(&controller->westbound_convar);
    pthread_cond_destroy(&controller->eastbound_convar);
    free_priority_queue(controller->westbound_queue);
    free_priority_queue(controller->eastbound_queue);
    free(controller);
}