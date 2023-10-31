#ifndef TRAIN_H
#define TRAIN_H

#include <pthread.h>

typedef struct {
    int number;
    char direction;
    int priority;
    float loading_time;
    float crossing_time;
    pthread_t thread;
    pthread_cond_t loaded_convar;
    pthread_cond_t self_convar;
} Train;

int train_comparator(void* a, void* b);

Train* create_train(int number, char direction, int priority, float loading_time, float crossing_time);
double timespec_to_seconds(struct timespec *ts);
void * train_behaviour(void *arg);
void free_train(Train *train);

#endif