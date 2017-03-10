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
#include <time.h>
/* .......................................................................... */

/* ========================================================================== */
/*                          Shared-Global variables                           */
/* ========================================================================== */
pthread_mutex_t mutex;
pthread_cond_t ok_to_proceed;
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3
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
void *thread_pool_loop(int my_rank);
void generate_test_alloc(int request[], int proc);
void update_avail();
void sleep();
void print_proc_resources(int proc);
/* .......................................................................... */


/* ========================================================================== */
/*                          Main( )                                           */
/* ========================================================================== */
int main(int argc, char *argv[])
{
	printf("\n bankers_algorithm.main() \n");
	int thread;
	pthread_t *thread_handles = malloc(NUM_CUSTOMERS * sizeof(pthread_t));
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&ok_to_proceed, NULL);

//	      Here is what I used for the max array
//        7,5,3
//        3,2,2
//        9,0,2
//        2,2,2
//        4,3,3
	maximum[0][0] = 7;
	maximum[0][1] = 5;
	maximum[0][2] = 3;
	maximum[1][0] = 3;
	maximum[1][1] = 2;
	maximum[1][2] = 2;
	maximum[2][0] = 9;
	maximum[2][1] = 0;
	maximum[2][2] = 2;
	maximum[3][0] = 2;
	maximum[3][1] = 2;
	maximum[3][2] = 2;
	maximum[4][0] = 4;
	maximum[4][1] = 3;
	maximum[4][2] = 3;
	int i;
	printf("\n    Max			Alloc		Need		Avail\n");
	for (i = 0; i < NUM_CUSTOMERS; i++)
	{
		print_proc_resources(i);
		update_avail();
		//update_need_avail(i);
	}
	printf("\n");

	// spawn all threads into the banker function
	for (thread = 0; thread < NUM_CUSTOMERS; thread++)
	{
		pthread_create(&thread_handles[thread], (pthread_attr_t *) NULL, thread_pool_loop, thread);
	}

	// terminate all treads
	for (thread = 0; thread < NUM_CUSTOMERS; thread++)
	{
		pthread_join(thread_handles[thread], NULL);
	}

	// all done, relinquish system resources
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&ok_to_proceed);
	free(thread_handles);
	return 0;
}  /* main */
/* .......................................................................... */


/* ========================================================================== */
/*                          thread_pool_loop( )                               */
/* ========================================================================== */
void *thread_pool_loop(int my_rank)
{
	//#1 a process makes a request
	int my_rank_request[NUM_RESOURCES];
	int i;
	for (i = 0; i < NUM_RESOURCES; i++)
	{
		int random_number = (rand() / (int) need[i]) % NUM_RESOURCES;
		my_rank_request[i] = random_number;
		allocation[my_rank][i] = my_rank_request[i];
	}
	//#2 make a test_alloc and test_avail
	int test_available[NUM_RESOURCES];
	int test_allocation[NUM_CUSTOMERS][NUM_RESOURCES];
	int ran[NUM_CUSTOMERS];
	ran[0] = 0;
	ran[1] = 0;
	ran[2] = 0;
	ran[3] = 0;
	ran[4] = 0;

	while (1)
	{
				pthread_mutex_lock(&mutex);

		printf("\n    Max			Alloc		Need		Avail\n");
	for (i = 0; i < NUM_CUSTOMERS; i++)
	{
		print_proc_resources(i);
		update_avail();
		//update_need_avail(i);
	}
	printf("\n");
			pthread_mutex_unlock(&mutex);


		if (ran[my_rank] == 0) // retry with prev deadlock proc
		{
			pthread_mutex_lock(&mutex);

			// copy real values into test data structures for this proc only
			int i, j;
			for (i = 0; i < NUM_RESOURCES; i++)
			{
				//test_available[i] = my_rank_request[i];

				for (j = 0; j < NUM_RESOURCES; j++)
				{
					test_allocation[my_rank][j] = allocation[my_rank][j];
				}
			}

			// update_avail need for this proc only
//			for (i = 0; i < NUM_RESOURCES; i++)
//			{
			for (j = 0; j < NUM_RESOURCES; j++)
			{
				need[my_rank][j] = maximum[my_rank][j] - test_allocation[my_rank][j];
			}
			//}

			// calculate
			for (i = 0; i < NUM_RESOURCES; i++)
			{
				for (j = 0; j < NUM_RESOURCES; j++)
				{
					// need = max - alloc
					if (need[my_rank][j] < test_available[my_rank])
					{
						test_available[my_rank] = test_allocation[my_rank][j];
						//available[my_rank] = test_allocation[my_rank][j];
						ran[my_rank] = 1; // no deadlock found
						printf(" Allocated = [%d %d %d] \n", test_allocation[my_rank][j], test_allocation[my_rank][j],
							   test_allocation[my_rank][j]);
					} else
					{
						ran[my_rank] = 0; // deadlock found
						//printf("  INSUFFICIENT RESOURCES \n");
					}
				}
			}

			if (ran[0] == 1 && ran[1] == 1 && ran[2] == 1 && ran[3] == 1 && ran[4] == 1)
			{
				break;
			}
			pthread_mutex_unlock(&mutex);

		} else
		{
			// Customer # 4 releasing 2 1 2 Available = 7  5  7  Allocated = [0  0  0  ]
			printf("Customer # %d releasing %d %d %d. Available = %d %d %d.\n", my_rank,
				   my_rank_request[0], my_rank_request[1], my_rank_request[2], available[0], available[1],
				   available[2]);
			// take a nap
			sleep();
		}
	}
}/* thread_pool_loop */
/* .......................................................................... */

void sleep()
{
	time_t current_time;
	/* Obtain current time. */
	current_time = time(NULL);
	srand(time(NULL));
	int wait_time = (rand() % 5);
	time_t end_time = current_time + wait_time;

	// wait a random number of seconds fro 0 to 4
	while (end_time > current_time)
	{
		current_time = time(NULL);
	}
}
void generate_test_alloc(int request[NUM_RESOURCES], int proc)
{
	int i;
	for (i = 0; i < NUM_RESOURCES; i++)
	{
		int random_number = (rand() / (int) need[i]) % NUM_RESOURCES;
		request[i] = random_number;
		allocation[proc][i] = request[i];
	}
}
void update_avail()
{
	int i, j;

	for (i = 0; i < NUM_CUSTOMERS; i++)
	{
		int max_sum_col = 0;
		int alloc_sum_col = 0;

		// generate a random request list
		int request[NUM_RESOURCES];
		generate_test_alloc(request, i);

		for (j = 0; j < NUM_RESOURCES; j++)
		{
			need[i][j] = maximum[i][j] - allocation[i][j];
			max_sum_col = max_sum_col + maximum[i][j];
			alloc_sum_col = alloc_sum_col + allocation[i][j];
			//allocation[i][j] = maximum[i][j];
		}
		available[i] = max_sum_col - alloc_sum_col;
	}
}
void print_proc_resources(int proc)
{
	int i;
	printf("P%d  ", proc);

	for (i = 0; i < NUM_RESOURCES; i++)
	{
		printf("%d ", maximum[proc][i]);

	}

	printf("		");

	for (i = 0; i < NUM_RESOURCES; i++)
	{
		printf("%d ", allocation[proc][i]);
	}
	printf("		");


		for (i = 0; i < NUM_RESOURCES; i++)
		{
			printf("%d ", need[proc][i]);
		}
		printf("		");

		for (i = 0; i < NUM_RESOURCES; i++)
		{
			printf("%d ", available[i]);
		}
		printf("		");


	printf("\n");
}
