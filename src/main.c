#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "txt_reader.h"
#include "railway_controller.h"
#include "train.h"
#include "priority_queue.h"
#include "linked_list.h"

// Global simulation time
#define NS_CONVERSION 1e9
pthread_mutex_t start_timer;
struct timespec sim_time;

// Global railway controller and train list
LinkedList *train_list;
RailwayController *controller;
int count = 0;

// Global mutex
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global loading synchronization
int waiting_trains = 0;
pthread_cond_t all_trains_ready = PTHREAD_COND_INITIALIZER;

double timespec_to_seconds(struct timespec *ts) {
	return ((double) ts->tv_sec) + (((double) ts->tv_nsec) / NS_CONVERSION);
}

void initialize_simulation() {
    sim_time.tv_sec = 0;
    sim_time.tv_nsec = 0;
    controller = create_railway_controller();
    if (!controller) {
        fprintf(stderr, "Error initializing railway controller.\n");
        exit(EXIT_FAILURE);
    }
}

void cleanup_simulation() {
    free_railway_controller(controller);
}

int main(int argc, char **argv) {
    // pid_t pid = fork();
    // if (pid < 0) {
    //     fprintf(stderr, "Fork Failed");
    // } else if (pid == 0) {
    //     char *args[] = {"afplay", "./trains.mp3", NULL};
    //     execvp(args[0], args);
    //     perror("execvp");
    // }
    // system("./starter.sh");
    initialize_simulation();

    // create the link list from input
    train_list = create_linked_list();
    read_input_file("input.txt", train_list);

    // create the threads
    Node *current_node = train_list->head;
    for (int i = 0; i < train_list->size; i++) {
        Train *train = (Train *)current_node->data;
        pthread_create(&train->thread, NULL, train_behaviour, train);
        current_node = current_node->next;
    }

    // synchronize loading times
    pthread_mutex_lock(&global_mutex);
    while (waiting_trains < train_list->size) {
        pthread_cond_wait(&controller->ctrl_convar, &global_mutex);
    }

    // printf("All trains ready to go, starting simulation and signalling threads.\n");
    clock_gettime(CLOCK_MONOTONIC, &sim_time);

    pthread_cond_broadcast(&all_trains_ready);
    pthread_mutex_unlock(&global_mutex);

   // join the threads to ensure they all finish if the main terminates first.
    Node *curr = train_list->head;
    for (int j=0; j<train_list->size; j++) {
        Train *train = (Train *)curr->data;
        pthread_join(train->thread, NULL);
        curr = curr->next;
    }

    free_list(train_list);
    cleanup_simulation();
    // kill(pid, SIGKILL);
    return 0;
}