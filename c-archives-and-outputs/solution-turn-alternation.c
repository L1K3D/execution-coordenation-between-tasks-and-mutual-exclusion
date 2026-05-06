#include <stdio.h>
#include <pthread.h>

/**
 * SOLUTION 3: STRICT ALTERNATION (TURN)
 * * Concept:
 * Uses a 'turn' variable to dictate which thread is allowed to enter
 * the critical section. Thread 0 can only enter if turn == 0, and vice versa.
 * * Why it is flawed:
 * It guarantees mutual exclusion, but violates the "Independence" principle.
 * If Thread 0 is much faster or doesn't need to enter the critical section
 * anymore, Thread 1 will be blocked forever waiting for 'turn' to become 1.
 */

// Global variables shared between threads
int balance = 0; // Shared balance variable to be incremented by both threads
int turn = 0; // Turn variable: 0 for Thread 0, 1 for Thread 1
const int ITERATIONS = 1000000; // Number of iterations each thread will perform

// Thread function for depositing (incrementing balance)
void *deposit(void *arg)
{
    // Extract thread ID from argument (0 or 1)
    long id = (long)arg;

    // Loop for the specified number of iterations
    for (int i = 0; i < ITERATIONS; i++)
    {
        
        // Busy waiting: spin until it's this thread's turn
        while (turn != id);

        // Critical Section: safely increment the shared balance
        balance = balance + 1;

        // Pass the turn to the other thread
        turn = 1 - id;

    }
    
    return NULL;
}

int main()
{

    // Declare thread identifiers
    pthread_t thread0, thread1;

    printf("Starting Solution 3: Strict Alternation (Turn)\n");

    // Create thread 0 with ID 0
    if (pthread_create(&thread0, NULL, deposit, (void *)0) != 0) return 1;
    // Create thread 1 with ID 1
    if (pthread_create(&thread1, NULL, deposit, (void *)1) != 0) return 2;

    // Wait for both threads to finish
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);

    // Print expected and actual balance
    printf("Expected balance: %d\n", ITERATIONS * 2);
    printf("Final balance: %d\n", balance);

    return 0;

}