#ifndef RAILWAY_CONTROLLER_H
#define RAILWAY_CONTROLLER_H

#include "train.h"
#include "priority_queue.h"

typedef struct {
    pthread_mutex_t track_mutex;
    PriorityQueue *westbound_queue;
    PriorityQueue *eastbound_queue;
    pthread_cond_t westbound_convar;
    pthread_cond_t eastbound_convar;
    pthread_cond_t track_available;
    pthread_cond_t ctrl_convar;
    pthread_cond_t track_convar;
    pthread_cond_t multiple_direction_convar;
    int direction_count;
    int last_direction;
    int is_occupied;
} RailwayController;

RailwayController* create_railway_controller();
void request_access_to_track(Train *train, RailwayController *controller);
void release_track(Train *train, RailwayController *controller);
void free_railway_controller(RailwayController *controller);

#endif