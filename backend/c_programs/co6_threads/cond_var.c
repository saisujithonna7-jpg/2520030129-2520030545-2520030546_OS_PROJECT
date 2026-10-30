/*
 * ============================================================================
 * Course Outcome 6: Thread Management and Synchronization
 * Program: cond_var.c
 * Purpose: Demonstrates POSIX Condition Variables (pthread_cond_wait, signal)
 *          with a Producer-Consumer Bounded Buffer synchronization pattern.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    for (int i = 1; i <= TOTAL_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            printf("  \033[1;33m[PRODUCER]\033[0m Buffer FULL (%d/%d). Sleeping on cond_not_full...\n", count, BUFFER_SIZE);
            pthread_cond_wait(&cond_not_full, &mutex);
        }

        buffer[in] = i * 10;
        printf("  \033[1;32m[PRODUCER]\033[0m Produced item: %d at slot %d (Buffer count: %d)\n", buffer[in], in, count + 1);
        in = (in + 1) % BUFFER_SIZE;
        count++;

        // Signal consumer that buffer is not empty
        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&mutex);

        usleep(80000); // 80ms
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 1; i <= TOTAL_ITEMS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            printf("  \033[1;36m[CONSUMER]\033[0m Buffer EMPTY (0/%d). Sleeping on cond_not_empty...\n", BUFFER_SIZE);
            pthread_cond_wait(&cond_not_empty, &mutex);
        }

        int item = buffer[out];
        printf("  \033[1;36m[CONSUMER]\033[0m Consumed item: %d from slot %d (Buffer count: %d)\n", item, out, count - 1);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        // Signal producer that buffer is not full
        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&mutex);

        usleep(120000); // 120ms
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("   CO6: Condition Variables (Producer-Consumer Bounded Buffer) \n");
    printf("===============================================================\n\n");

    pthread_t prod_thread, cons_thread;

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_not_full);
    pthread_cond_destroy(&cond_not_empty);

    printf("\n\033[1;32m[SUCCESS]\033[0m Producer-Consumer bounded queue safely completed with zero overflows/underflows.\n");
    return 0;
}
