/*==============================================================================
 Author:       =        Tim Siwula <tcsiwula@gmail.com>
 Liscense:     =        GPLv2
 File:         =        bankers_algorithm.c
 Version:      =        0.01
 Created:      =        03/06/2017
==============================================================================
Compile:      =        gcc -g -Wall -o b_a bankers_algorithm.c
Run:          =        ./b_a 10 5 7 -lpthread
Debug:        =        gcc -DDEBUG -g -Wall -o b_a bankers_algorithm.c
==============================================================================
Input:        =        Integer list representing resource type and amount.
                       (i.e.) [./b_a 10 5 7 -lpthread] represents 3 resources.
                       Resource #1 whose amount is 10
                       Resource #2 whose amount is 5
                       Resource #3 whose amount is 7
Output:       =        -1 if a deadlock would occur.
==============================================================================
About:        =         A deadlock avoidance algorithm.
==============================================================================
Note:         =
==============================================================================
............................................................................ */

/* ========================================================================== */
/*                          External libaries                                 */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/* .......................................................................... */

/* ========================================================================== */
/*                          Shared-Global variables                           */
/* ========================================================================== */
pthread_mutex_t mutex;
pthread_cond_t ok_to_proceed;
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3
#define FALSE -1
#define TRUE 0
#define NO_DEADLOCK 0
#define DEADLOCK_FOUND -1

// the available amount of each resource
int available[NUM_RESOURCES];
// the maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];
// the amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];
// the remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];
/* .......................................................................... */

/* ========================================================================== */
/*                          Custom function definitions                       */
/* ========================================================================== */
int request_resources(int my_rank, int request[]);

int release_resources(int my_rank, int release[]);

int safety_algo(int my_rank, int request[]);

void thread_pool_loop(int my_rank);

void generate_req(int request[]);
/* .......................................................................... */


/* ========================================================================== */
/*                          Main( )                                           */
/* ========================================================================== */
int main(int argc, char *argv[]) {
    printf("\n bankers_algorithm.main() \n");
    int thread;
    pthread_t *thread_handles = malloc(NUM_CUSTOMERS * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&ok_to_proceed, NULL);

    // spawn all threads into the banker function
    for (thread = 0; thread < NUM_CUSTOMERS; thread++) {
        pthread_create(&thread_handles[thread], (pthread_attr_t *) NULL, thread_pool_loop, thread);
    }

    // terminate all treads
    for (thread = 0; thread < NUM_CUSTOMERS; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    // all done, relinquish system resources
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&ok_to_proceed);
    free(thread_handles);
    free(available);
    free(maximum);
    free(allocation);
    free(need);
    return 0;
}  /* main */
/* .......................................................................... */


void thread_pool_loop(int my_rank) {

    // lock with pthread mutex lock
    pthread_mutex_lock(&mutex);
    printf("\n thread_pool_loop() my_rank = %d \n", my_rank);

    // generate a random request list
    int request[NUM_RESOURCES];
    generate_req(request);


    if (safety_algo(my_rank, request) == NO_DEADLOCK) {
        request_resources(my_rank, request);
    } else {
        // generate a random release list
        int release[NUM_RESOURCES];
        generate_req(release);
        release_resources(my_rank, release);
    }

    // unlock with pthread mutex lock
    pthread_mutex_unlock(&mutex);
}

void generate_req(int request[]) {
    srand(time(NULL));

    int i;
    for (i = 0; i < NUM_RESOURCES; i++) {
        int random_number = ((double) rand() / (double) NUM_RESOURCES);
        request[i] = random_number;
        printf("\n request[%d] = %d \n", i, random_number);
    }
}

int safety_algo(int my_rank, int request[]) {

    printf("\n safety_algo() my_rank = %d \n", my_rank);

    // step 1
    int work[NUM_RESOURCES];
    int finish[NUM_CUSTOMERS];
    int i = 0;

    for (i = 0; i < NUM_CUSTOMERS; i++) {
        work[i] = available[i];
        finish[i] = FALSE; // FALSE == -1
    }

    // step 2
    do {
        for (i = 0; i < NUM_RESOURCES; i++) {

            //printf("\n finish[i] = %d \n", finish[i]);
            printf("need[i] = %d \n", need[i]);
            printf("work = %d \n", work);


            if (finish[i] = 0 && need[i] <= work) {

                // step 3
                work[i] = work[i] + allocation[i];
                finish[i] = TRUE; // TRUE == 1

                // go to step 2?
                break;
            }
        }
    } while (finish[i] = 0 && need[i] <= work);

    // step 4
    int allTrue = 1;
    for (i = 0; i < NUM_RESOURCES; i++) {
        if (finish[i] != 1) {
            allTrue = 0;
        }
    }

    if (allTrue == 1) {
        return NO_DEADLOCK;
    } else {
        return DEADLOCK_FOUND;
    }
}


int request_resources(int customer_num, int request[]) {
    //// Customer # 2 requesting 3 0 0 Available = 10  5  7

    printf("Customer # %d requesting %d %d %d Available = %d %d %d", customer_num),
            request[0], request[1], request[2], available[0], available[1], available[2];

    int successful = 0;
    int unsuccessful = -1;

    if (customer_num == successful) {
        return successful;
    } else {
        return unsuccessful;
    }
}

int release_resources(int customer_num, int release[]) {

// Customer # 4 releasing 2 1 2 Available = 7  5  7  Allocated = [0  0  0  ]

    printf("Customer # %d releasing %d %d %d Available = %d %d %d", customer_num),
            release[0], release[1], release[2], available[0], available[1], available[2];


    int successful = 0;
    int unsuccessful = -1;

    if (customer_num == successful) {
        return successful;
    } else {
        return unsuccessful;
    }
}

/* ========================================================================== */
/*                          THE CUSTOMERS                                         */
/* ========================================================================== */
/*
 *
 *      CREATE N CUSTOMER THREADS
 *      EACH REQUEST AND RELEASE RESOURCES FROM THE BANK
 *      BANKER WILL GRANT REQUEST IF SAFTY ALGO IS SUCCESSFUL
 *      ELSE DENY REQUEST
 *
 *      Here is what I used for the max array
        7,5,3
        3,2,2
        9,0,2
        2,2,2
        4,3,3
 *
 */



//The output of the program should be the allocation, need and available arrays as the processes run and show that the result does not deadlock or get in an unsafe state.  For testing I would run it for a while, for the output you turn it, several pages of tables will be fine.
//
// Customer # 2 requesting 3 0 0 Available = 10  5  7
// Customer # 4 requesting 2 1 2 Available = 7  5  7
// Customer # 4 releasing 2 1 2 Available = 7  5  7  Allocated = [0  0  0  ]
// Customer # 0 requesting 3 0 2 Available = 7  5  7
// Customer # 0 releasing 3 0 2 Available = 7  5  7  Allocated = [0  0  0  ]
// Customer # 2 releasing 3 0 0 Available = 10  5  7  Allocated = [0  0  0  ]
// Customer # 2 requesting 6 0 2 Available = 10  5  7
// Customer # 4 requesting 3 1 1 Available = 4  5  5
// Customer # 3 requesting 1 1 0 Available = 1  4  4
// Customer # 3 requesting 0 0 1 Available = 1  4  4
// Customer # 0 requesting 2 0 1 Available = 1  4  3  INSUFFICIENT RESOURCES
//
// Customer # 2 releasing 6 0 2 Available = 7  4  5  Allocated = [0  0  0  ]
// Customer # 1 requesting 0 2 0 Available = 7  4  5
// Customer # 0 requesting 6 2 1 Available = 7  2  5
// Customer # 2 requesting 4 0 2 Available = 7  2  5
// Customer # 1 releasing 0 2 0 Available = 3  4  3  Allocated = [0  0  0  ]
// Customer # 0 requesting 0 2 0 Available = 3  4  3
// Customer # 0 releasing 0 2 0 Available = 3  4  3  Allocated = [0  0  0  ]
// Customer # 4 releasing 3 1 1 Available = 6  5  4  Allocated = [0  0  0  ]
// Customer # 3 releasing 0 0 1 Available = 6  5  5  Allocated = [0  0  0  ]
// Customer # 2 releasing 4 0 2 Available = 10  5  7  Allocated = [0  0  0  ]
// Customer # 0 requesting 7 5 2 Available = 10  5  7
// Customer # 2 requesting 8 0 2 Available = 3  0  5  INSUFFICIENT RESOURCES
//
// Customer # 4 requesting INSUFFICIENT RESOURCES
//2 2 2 Available = 3  0  5
// Customer # 1 requesting 1 0 1 Available = 3  0  5
// Customer # 3 requesting 0 INSUFFICIENT RESOURCES
//2 0 Available = 2  0  4
// Customer # 3 requesting 1 2 2 Available = 2  0  4  INSUFFICIENT RESOURCES
//
// Customer # 2 requesting 3 INSUFFICIENT RESOURCES
//0 1 Available = 2  0  4
// Customer # 4 requesting 4 0 2 Available = 2  0  4
// Customer # 2 requesting 8 0 0 Available = 2  0  4  INSUFFICIENT RESOURCES
//INSUFFICIENT RESOURCES
//
// Customer # 1 releasing 1 0 1 Available = 3  0  5  Allocated = [0  0  0  ]
// Customer # 1 requesting 3 0 2 Available = 3  0  5
// Customer # 1 releasing 3 0 2 Available = 3  0  5  Allocated = [0  0  0  ]
// Customer # 0 releasing 7 5 2 Available = 10  5  7  Allocated = [0  0  0  ]
// Customer # 3 requesting 2 0 1 Available = 10  5  7
// Customer # 0 requesting 7 0 1 Available = 8  5  6
// Customer # 1 requesting 1 1 1 Available = 1  5  5
// Customer # 4 requesting 2 3 3 Available = 0  4  4  INSUFFICIENT RESOURCES
//
// Customer # 2 requesting 3 0 1 Available = 0  4  4  INSUFFICIENT RESOURCES
//
// Customer # 0 releasing 7 0 1 Available = 7  4  5  Allocated = [0  0  0  ]
// Customer # 3 releasing 2 0 1 Available = 9  4  6  Allocated = [0  0  0  ]
//.
//.
//.
//(until you stop it)
