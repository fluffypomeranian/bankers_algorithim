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
Output:       =        True or false if a deadlock would occur.
==============================================================================
Note:         =        1.  If the list is randomly generated, the keys are in
              =            the range 1 -- KEY_MAX.
              =         2.  The size of the list, n, should be a power of 2:
              =         the program doesn't check that this is the case.
............................................................................ */

/* ========================================================================== */
/*                          External libaries                                 */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/* .......................................................................... */

/* ========================================================================== */
/*                          Constants                                         */
/* ========================================================================== */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

// the available amount of each resource
int available[NUMBER_OF_RESOURCES];

// the maximum demand of each customer
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// the amount currently allocated to each customer
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// the remaining need of each customer
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* .......................................................................... */

/* ========================================================================== */
/*                          Custom function definitions                       */
/* ========================================================================== */
int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);
//int safety_algo(int my_rank, int need[], int alloc[][], int max[][], int avail[]);
/* .......................................................................... */

/* ========================================================================== */
/*                          Main( )                                           */
/* ========================================================================== */
int main(int argc, char* argv[])
{
      printf("\n bankers_algorithm.main() \n");
//    while(true) // request and release random resources loop
//    {
//
//         if ( safety_algo(my_rank, customers_request) )
//         {
//            // lock with pthread mutex lock
//            request_resources(my_rank, req);
//            // unlock with pthread mutex lock
//         }
//         else
//         {
//            // lock with pthread mutex lock
//            release_resources(my_rank, release);
//            //un lock with pthread mutex lock
//         }
//
//    }
//
    return 0;
}  /* main */
/* .......................................................................... */


 /* ============================================================================
 Function:     =        Get_args
 Purpose:      =        Get the command line arguments.
 ==============================================================================
 Input arg:  =        1. argc: Flag for list creation type.
             =        2. *argv[]: A pointer to an array.
             =        5. list_size: The size of the processors array.
             =        6. number_of_processors: The size of the processors array.
 ==============================================================================
 Out args:   =        1. list_size_p: A pointer an integer.
             =        2. generate_list_p: pointer to an i or g for list type.
 =========================================================================== */
void Get_args(int argc, char* argv[], int* list_size_p, char* generate_list_p,
                int* list_size, int number_of_processors)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <n> <'i'|'g'>\n", argv[0]);
        fprintf(stderr, "n = number of elements in the list (a power of 2)\n");
        fprintf(stderr, "'i':  user will enter list (no quotes)\n");
        fprintf(stderr, "'g':  program should generate list (no quotes)\n");
        exit(0);
    }

    *list_size_p = strtol(argv[1], NULL, 10);
    *generate_list_p = argv[2][0];
    *list_size = *list_size_p/number_of_processors;
}  /* Get_args */
/* .......................................................................... */
