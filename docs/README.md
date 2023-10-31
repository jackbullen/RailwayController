# OS Scheduling Simulator

The goal is to simulate an OS scheduler. The simulation has a **shared resource** (main track), **two consumers of the resource with high and low priority requests** (trains), and the **scheduling unit** (railway controller). The trains request access to the track based on priorities and other rules while the controller manages these requests and grants safe access to the resource.

## Train Structure (Train):
**Fields:**
- `number` An integer representing a unique identifier for the train.
- `direction` A character indicating the direction of the train (e.g., eastbound or westbound).
- `priority` An integer representing the priority of the train (e.g., high or low).
- `loading_time` A float indicating the time it takes to load the train.
- `crossing_time` A float representing the time it takes for the train to cross the track.
- `thread` A pthread_t type representing the thread associated with the train.
- `loaded_convar` A condition variable (pthread_cond_t) signaling when the train has finished loading.
- `self_convar` Another condition variable for the train's internal synchronization.

**Functions:**
- `train_comparator(void* a, void* b)` A comparator function to compare two trains, potentially used in sorting or priority queues.
- `create_train(...)` A function to create and initialize a train with given attributes.
- `timespec_to_seconds(struct timespec *ts)` A utility function to convert a timespec structure to seconds.
- `train_behaviour(void *arg)` A function representing the behavior or routine of a train thread.
- `free_train(Train *train)` A function to free the memory and resources associated with a train.

## Railway Controller Structure (RailwayController):
**Fields:**
- `track_mutex` A mutex (pthread_mutex_t) for synchronizing access to the track.
- `westbound_queue` & `eastbound_queue` Priority queues representing the queues of trains waiting to go westbound and eastbound respectively.
- `westbound_convar` & `eastbound_convar` Condition variables for synchronizing westbound and eastbound trains.
- `track_available` A condition variable signaling when the track is available for use.
- `ctrl_convar` A condition variable for internal synchronization of the controller.
- `track_convar` Another condition variable related to track availability.
- `multiple_direction_convar` A condition variable for managing trains from different directions.
- `direction_count` An integer counter for the direction of trains.
- `last_direction` An integer representing the last direction in which a train was allowed.
- `is_occupied` An integer (likely used as a boolean) indicating if the track is currently occupied.

**Functions:**
- `create_railway_controller()` A function to create and initialize a railway controller.
- `request_access_to_track(Train *train, RailwayController *controller)` A function where a train requests access to the track, and the controller manages this request.
- `release_track(Train *train, RailwayController *controller)` A function where a train releases the track after crossing, signaling its availability.
- `free_railway_controller(RailwayController *controller)` A function to free the memory and resources associated with a railway controller.

## Node Structure (Node):
**Fields:**
- `data` A pointer to the data stored in the node. This is a generic pointer, meaning the linked list can store any type of data.
- `next` A pointer to the next node in the list.

**Functions**:
- `create_node(void *data)` Creates a new node with the given data.
- `free_node(Node *head)` Frees the memory associated with the given node.

## Linked List Structure (LinkedList):
**Fields:**
- `head` A pointer to the head node of the list.
- `size` An integer representing the number of elements in the list.

**Functions:**
- `create_linked_list()` Creates a new linked list.
- `insert_end(LinkedList *list, void *data)` Inserts a new node with the given data at the end of the list.
- `insert_front(LinkedList *list, void *data)` Inserts a new node with the given data at the beginning of the list.
- `delete_beginning(LinkedList *list)` Deletes the first node of the list and returns its data.
- `free_list(LinkedList *list)` Frees the memory associated with the linked list and all its nodes.


## Priority Queue Structure (PriorityQueue):

**list:** A pointer to a LinkedList that stores the elements of the priority queue.
**comparator_fn:** A function pointer type representing a comparator function. This function takes two generic pointers and returns an integer. It's used to determine the priority of elements when inserting into the priority queue.

**Functions:**

- `create_priority_queue()` Creates a new priority queue.

- `enqueue(PriorityQueue *pq, void *data, comparator_fn comparator)` Inserts a new element with the given data into the priority queue based on its priority determined by the comparator function.

- `dequeue(PriorityQueue *pq)` Removes and returns the highest-priority element from the priority queue.

- `free_priority_queue(PriorityQueue *pq)` Frees the memory associated with the priority queue and its elements.

## Train.c 

Implementation of the behavior and operations associated with a train.

**Utility Functions:**
format_time_output(double time_in_seconds, char* buffer):

Converts time in seconds into a formatted string (HH:MM:SS.T).
Stores the formatted time in the provided buffer.
get_full_direction(char direction):

Returns the full string representation ("East", "West", or "Unknown") of a train's direction based on its character representation ('e', 'w', etc.).
Core Functions:

- `create_train(...)`

Creates a new Train object with the provided attributes.
Initializes condition variables associated with the train.
Returns a pointer to the new train.

- `train_behaviour(void *arg)`

Represents the behavior of a train thread.
The train goes through various stages, such as:
Loading and preparing to cross.
Requesting access to the main track.
Crossing the main track.
Releasing the track after crossing.
The function outputs timestamps and status messages for the train at different stages.
train_comparator(void* a, void* b):

Comparator function for comparing two trains based on their priority and number.
Used for ordering trains in priority queues or other data structures.

- `free_train(Train *train)`

Frees the memory and resources associated with a train.
Destroys condition variables associated with the train.

## main.c 
Entry point of the railway simulator program.

**Includes:**
The file includes standard libraries, pthread for threading, and user-defined headers (txt_reader.h, railway_controller.h, train.h, priority_queue.h, linked_list.h).

**Global Variables:**

`sim_time` Global simulation time.

`NS_CONVERSION` A constant for converting nanoseconds.

`start_timer` A mutex for starting the simulation timer.

`global_mutex` A global mutex for general synchronization.

`waiting_trains` A counter for trains that are waiting.

`all_trains_ready` A condition variable for ensuring all trains are ready.

## Txt_reader.c 

Implementation for reading data from an input file and storing it in a linked list.

**Function:** `read_input_file(const char *filename, LinkedList *list)`
Purpose: Reads train data from the specified input file and stores each entry as a Train object in the provided linked list.

**Implementation:**

Opens the specified file for reading.
Checks for errors in opening the file.
Initializes variables to store the train's attributes: number, priority, direction, loading_time, and crossing_time.
Uses a loop to read each line of the file, extracting the train's direction, loading time, and crossing time.
Determines the train's priority based on its direction (e or w for low priority, other values for high priority).
Creates a new Train object with the extracted data.
Inserts the Train object at the end of the linked list.
Increments the train number for the next iteration.
Closes the file after reading all entries.
This function provides a straightforward way to read train data from a file and store it in a data structure for further processing.

## Railway Controller and Trains:

`train_list:` A linked list storing all the trains.

`controller`: An instance of the railway controller.