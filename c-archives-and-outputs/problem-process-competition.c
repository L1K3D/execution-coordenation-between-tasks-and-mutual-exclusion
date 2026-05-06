#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/* Global variable shared across all threads */
int balance = 0;

/* Function to be executed by each thread */
void *deposit(void *arg)
{
    for (int i = 0; i < 100000; i++)
    {
        /*
         * Critical Section: Multiple threads attempt to read, increment,
         * and write back the 'balance' value simultaneously.
         * This leads to a "Race Condition".
         */
        balance = balance + 1;
    }
    return NULL;
}

int main()
{
    /* Thread identifiers */
    pthread_t thread1, thread2;

    /* Create the first thread to run the 'deposit' function */
    pthread_create(&thread1, NULL, deposit, NULL);

    /* Create the second thread to run the 'deposit' function */
    pthread_create(&thread2, NULL, deposit, NULL);

    /* Wait for thread1 to finish execution */
    pthread_join(thread1, NULL);

    /* Wait for thread2 to finish execution */
    pthread_join(thread2, NULL);

    /* Display the expected result vs the actual result */
    printf("Final balance expected: 200000\n");
    printf("Final balance obtained: %d\n", balance);

    return 0;
}