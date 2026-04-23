#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int balance = 0;

void* deposit(void* arg) {
    for (int i = 0; i < 100000; i++) {
        balance = balance + 1;
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, deposit, NULL);
    pthread_create(&thread2, NULL, deposit, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Final balance spected: 2000000\n");
    printf("Final balance obtained: %d\n");

    return 0;
}