#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "train.h"
#include "priority_queue.h"
#include "railway_controller.h"
#include <pthread.h>
#include <time.h>

extern pthread_mutex_t global_mutex;
extern int waiting_trains;
extern pthread_cond_t all_trains_ready;
extern int count;
extern RailwayController *controller;
extern LinkedList *train_list;
extern struct timespec sim_time;

void format_time_output(double time_in_seconds, char* buffer) {
    int hours = (int)(time_in_seconds / 3600);
    time_in_seconds -= hours * 3600;

    int minutes = (int)(time_in_seconds / 60);
    time_in_seconds -= minutes * 60;

    int seconds = (int)time_in_seconds;
    time_in_seconds -= seconds;

    int tenths = (int)(time_in_seconds * 10);

    sprintf(buffer, "%02d:%02d:%02d.%d", hours, minutes, seconds, tenths);
}

const char* get_full_direction(char direction) {
    switch (direction) {
        case 'E':
        case 'e':
            return "East";
        case 'W':
        case 'w':
            return "West";
        default:
            return "Unknown";
    }
}

Train* create_train(int number, char direction, int priority, float loading_time, float crossing_time) {
    Train *new_train = (Train *)malloc(sizeof(Train));
    if (!new_train) {
        fprintf(stderr, "Memory allocation error: Failed to allocate memory for new train.\n");
        exit(EXIT_FAILURE);
    }

    new_train->number = number;
    new_train->direction = direction;
    new_train->priority = priority;
    new_train->loading_time = loading_time;
    new_train->crossing_time = crossing_time;

    pthread_cond_init(&(new_train->loaded_convar), NULL);
    pthread_cond_init(&(new_train->self_convar), NULL);  // Initialize the train's own condition variable

    return new_train;
}


void* train_behaviour(void *arg) {
    // initialize
    Train *train = (Train *)arg;
    pthread_mutex_lock(&global_mutex);
    char time_buffer[12];
    
    // increment waiting trains and signal if all trains are ready
    waiting_trains++;
    if (waiting_trains == train_list->size) {
        // printf("Train %d is signalling ctrl that all trains are ready\n", train->number);
        pthread_cond_signal(&controller->ctrl_convar);
    }

    // wait for all trains to be ready
    while (waiting_trains < train_list->size) {
        pthread_cond_wait(&all_trains_ready, &global_mutex);
    }
    
    pthread_mutex_unlock(&global_mutex);

    // printf("Train %d is ready to begin loading %s\n", train->number, get_full_direction(train->direction));
    struct timespec curr_time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &curr_time);

    // loading the train
    usleep(train->loading_time * 1000000);
    // train loaded, request access to track now

    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    format_time_output(timespec_to_seconds(&curr_time) - timespec_to_seconds(&sim_time), time_buffer);
    printf("%s Train %d is ready to go %s\n", time_buffer, train->number, get_full_direction(train->direction));

    // finished loading, getting in queue
    request_access_to_track(train, controller);
    // acquired main track

    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    format_time_output(timespec_to_seconds(&curr_time) - timespec_to_seconds(&sim_time), time_buffer);
    printf("%s Train %d is ON the main track going %s\n", time_buffer, train->number, get_full_direction(train->direction));

    // crossing main track
    usleep(train->crossing_time * 1000000);
    // finished crossing main track, releasing

    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    format_time_output(timespec_to_seconds(&curr_time) - timespec_to_seconds(&sim_time), time_buffer);
    printf("%s Train %d is OFF the main track after going %s\n", time_buffer, train->number, get_full_direction(train->direction));

    release_track(train, controller);
    
    return NULL;
}

int train_comparator(void* a, void*b) {
    Train* trainA = (Train*) a;
    Train* trainB = (Train*) b;

    // Debugging prints
    // printf("Comparing train %d and train %d\n", trainA->number, trainB->number);
    // printf("Train %d priority: %d\n", trainA->number, trainA->priority);
    // printf("Train %d priority: %d\n", trainB->number, trainB->priority);

    if (trainA->priority == trainB->priority) {
        // if priority is the same, compare train numbers
        // note that the train with higher priority will have a lower number
        // hence the subtraction is reversed
        return  trainB->number - trainA->number;
    }
    return trainA->priority - trainB->priority;
}

void free_train(Train *train) {
    pthread_cond_destroy(&(train->loaded_convar));
    pthread_cond_destroy(&(train->self_convar));  // Destroy the train's own condition variable
    free(train);
}
