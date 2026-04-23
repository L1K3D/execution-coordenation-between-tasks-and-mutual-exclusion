#include <stdio.h>
#include <pthread.h>

/**
 * SOLUTION 1: INTERRUPT INHIBITION (SIMULATED)
 * * Concept:
 * In single-processor systems, the simplest way to achieve mutual exclusion
 * is to disable interrupts during the execution of the critical section.
 * This prevents the OS scheduler from switching the context to another
 * thread until the current thread finishes its task.
 * * Why it is impractical into nowadays operating systems:
 * 1. Multi-core systems: Disabling interrupts on one CPU core does not
 * prevent a thread on another core from accessing the shared memory.
 * 2. System risk: If a thread enters an infinite loop with interrupts
 * disabled, the entire operating system will freeze.
 */

// Shared global variable (Critical Region)
int balance = 0;
const int ITERATIONS = 1000000;

/**
 * Simulated function to disable hardware interrupts.
 * In a real Kernel-level scenario, this would involve assembly
 * instructions like 'cli' (Clear Interrupt Flag).
 */
void disable_interrupts()
{
    // Simulation: In a real system, this would tell the CPU
    // to ignore any external signals that trigger context switching.
}

/**
 * Simulated function to enable hardware interrupts.
 * In a real Kernel-level scenario, this would involve assembly
 * instructions like 'sti' (Set Interrupt Flag).
 */
void enable_interrupts()
{
    // Simulation: Restores the CPU's ability to switch tasks.
}

/**
 * Function executed by each thread.
 */
void *deposit(void *arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        // --- START OF CRITICAL SECTION PROTECTION ---
        disable_interrupts();

        balance = balance + 1;

        enable_interrupts();
        // --- END OF CRITICAL SECTION PROTECTION ---
    }
    return NULL;
}

int main()
{
    pthread_t thread1, thread2;

    printf("Starting Solution 1: Interrupt Inhibition (Simulation)\n");

    // Creating two concurrent threads
    if (pthread_create(&thread1, NULL, deposit, NULL) != 0)
        return 1;
    if (pthread_create(&thread2, NULL, deposit, NULL) != 0)
        return 2;

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Results
    printf("Expected Balance: %d\n", ITERATIONS * 2);
    printf("Final Balance:    %d\n", balance);

    if (balance != ITERATIONS * 2)
    {
        printf("Note: As expected, in a modern multi-core Windows environment, "
               "simple interrupt inhibition simulation does not prevent race conditions.\n");
    }

    return 0;
}