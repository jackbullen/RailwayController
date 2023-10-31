#include <stdio.h>
#include <stdlib.h>
#include "txt_reader.h"

void read_input_file(const char *filename, LinkedList *list) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    int number = 0;
    int priority;
    char direction;
    float loading_time, crossing_time;

    while(fscanf(file, "%c %f %f\n", &direction, &loading_time, &crossing_time) == 3) {
        if (direction == 'e' || direction == 'w') {
            priority = 0;
        } else {
            priority = 1;
        }
        Train *train = create_train(number, direction, priority, loading_time/10, crossing_time/10);
        insert_end(list, train);
        number++;
    }
    fclose(file);
    return;
}