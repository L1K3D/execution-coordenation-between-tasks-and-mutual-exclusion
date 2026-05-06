#include <stdio.h>
#include <pthread.h>

/**
 * SOLUTION 4: PETERSON'S ALGORITHM
 * * Concept:
 * Combines the ideas of the 'busy' flag and the 'turn' variable.
 * Each thread indicates its intention to enter the critical section (flag),
 * but politely yields the priority to the other thread (turn).
 * * Why it works:
 * It guarantees mutual exclusion, avoids deadlocks, and respects independence.
 * It does not require special hardware instructions, making it a pure
 * software solution for 2 threads.
 */

// Global variables shared between threads
int balance = 0; // Shared balance variable to be incremented by both threads
int flag[2] = {0, 0}; // Flag array: flag[i] indicates if thread i wants to enter critical section
int turn = 0; // Turn variable to resolve conflicts when both threads want to enter
const int ITERATIONS = 100000; // Number of iterations each thread will perform

// Thread function for depositing (incrementing balance)
void *deposit(void *arg)
{
    
    // Extract thread ID from argument (0 or 1)
    long id = (long)arg;
    // Calculate the other thread's ID
    long other = 1 - id;

    // Loop for the specified number of iterations
    for (int i = 0; i < ITERATIONS; i++)
    {

        // Indicate intention to enter critical section
        flag[id] = 1;

        // Politely yield turn to the other thread
        turn = other;

        // Busy wait: wait until the other thread doesn't want to enter or it's not their turn
        while (flag[other] && turn == other);

        // Critical Section: safely increment the shared balance
        balance = balance + 1;

        // Indicate exit from critical section
        flag[id] = 0;
    
    }

    return NULL;

}

int main()
{

    // Declare thread identifiers
    pthread_t thread0, thread1;

    printf("Starting Solution 4: Peterson's Algorithm\n");

    // Create thread 0 with ID 0
    if (pthread_create(&thread0, NULL, deposit, (void *)0) != 0) return 1;
    // Create thread 1 with ID 1
    if (pthread_create(&thread1, NULL, deposit, (void *)1) != 0) return 2;

    // Wait for both threads to finish
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);

    // Print expected and actual balance
    printf("Expected Balance: %d\n", ITERATIONS * 2);
    printf("Final Balance: %d\n", balance);

    return 0;

}