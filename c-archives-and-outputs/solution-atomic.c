#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

/**
 * SOLUTION 5: ATOMIC OPERATIONS (TEST-AND-SET)
 * * Concept:
 * Uses hardware-level instructions (like XCHG in x86) provided via C's
 * stdatomic library. The reading of the lock and setting it to true
 * happens in a single, uninterruptible clock cycle (atomic).
 * * Why it works:
 * It perfectly prevents race conditions because no context switch can
 * happen *during* the evaluation of the lock. However, it still uses
 * busy waiting, consuming CPU cycles.
 */

// Global variables shared between threads
int balance = 0; // Shared balance variable to be incremented by both threads
atomic_flag lock = ATOMIC_FLAG_INIT; // Atomic flag used as a lock; initialized to false (unlocked)
const int ITERATIONS = 1000000; // Number of iterations each thread will perform

// Thread function for depositing (incrementing balance)
void *deposit(void *arg)
{
    // Loop for the specified number of iterations
    for (int i = 0; i < ITERATIONS; i++)
    {

        // Atomically test and set the lock: returns previous value and sets to true.
        // Busy wait (spin) until the lock is acquired (previous value was false).
        while (atomic_flag_test_and_set(&lock));

        // Critical Section: safely increment the shared balance
        balance = balance + 1;

        // Release the lock by clearing the atomic flag
        atomic_flag_clear(&lock);

    }
    
    return NULL;

}

int main()
{

    // Declare thread identifiers
    pthread_t thread1, thread2;

    printf("Starting Solution 5: Atomic Operations (Test-and-Set)\n");

    // Create the first thread
    if (pthread_create(&thread1, NULL, deposit, NULL) != 0) return 1;
    // Create the second thread
    if (pthread_create(&thread2, NULL, deposit, NULL) != 0) return 2;

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Print expected and actual balance
    printf("Expected Balance: %d\n", ITERATIONS * 2);
    printf("Final Balance:    %d\n", balance);

    return 0;
    
}