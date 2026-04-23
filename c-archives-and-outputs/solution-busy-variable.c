#include <stdio.h>
#include <pthread.h>

/**
 * SOLUTION 2: BUSY VARIABLE (BUSY WAITING)
 * * * Concept:
 * Uses a global 'busy' flag as a software lock. Threads check this flag
 * before entering the critical section.
 * * * Why it fails:
 * The "test and set" operation is not atomic. A thread might check 'busy == 0',
 * get interrupted, and a second thread might also see 'busy == 0'.
 * Both will then set 'busy = 1' and enter the critical section simultaneously.
 * * * CPU Waste:
 * The 'while(busy)' loop consumes CPU cycles doing nothing but checking the flag,
 * which is called "Busy Waiting".
 */

int balance = 0;
int busy = 0; // 0 = free, 1 = occupied
const int ITERATIONS = 1000000;

void *deposit(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {

        // --- STEP 1: Busy Waiting (CPU Waste) ---
        // A thread stays here looping while busy is 1.
        while (busy)
            ;

        // --- STEP 2: The Vulnerability ---
        // A race condition occurs right here. If a context switch happens
        // after the 'while' check but before the next line, both threads enter.
        busy = 1;

        // Critical Section
        balance = balance + 1;

        // Release the lock
        busy = 0;
    }
    return NULL;
}

int main()
{
    pthread_t thread1, thread2;

    printf("Starting Solution 2: Busy Variable (Software Lock)\n");

    if (pthread_create(&thread1, NULL, deposit, NULL) != 0)
        return 1;
    if (pthread_create(&thread2, NULL, deposit, NULL) != 0)
        return 2;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Expected Balance: %d\n", ITERATIONS * 2);
    printf("Final Balance:    %d\n", balance);

    if (balance != ITERATIONS * 2)
    {
        printf("Analysis: Mutual exclusion FAILED. Race condition occurred between check and set.\n");
    }

    return 0;
}