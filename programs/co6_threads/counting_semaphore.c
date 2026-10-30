/*
 * ============================================================================
 * Course Outcome 6: Thread Management and Synchronization
 * Program: counting_semaphore.c
 * Purpose: Demonstrates POSIX Counting Semaphores (sem_init, sem_wait, sem_post)
 *          controlling a finite resource pool (3 available slots for 6 threads).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define RESOURCE_COUNT 3
#define THREAD_COUNT 6

sem_t resource_pool;

void *worker(void *arg) {
    int id = *(int *)arg;

    printf("  \033[1;37m[Thread %d]\033[0m Waiting for available resource slot (sem_wait)...\n", id);
    sem_wait(&resource_pool); // Decrements semaphore; blocks if 0

    int current_val;
    sem_getvalue(&resource_pool, &current_val);
    printf("  \033[1;32m[Thread %d] >> ENTERED CRITICAL SECTION\033[0m | Remaining free slots: %d\n", id, current_val);

    // Simulating resource usage
    usleep(150000); // 150ms

    printf("  \033[1;33m[Thread %d] << RELEASING RESOURCE\033[0m (sem_post)...\n", id);
    sem_post(&resource_pool); // Increments semaphore; wakes up a waiting thread

    return NULL;
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("     CO6: Counting Semaphores Resource Pool Demonstration      \n");
    printf("===============================================================\n");
    printf("Total Resource Slots: %d | Competing Worker Threads: %d\n\n", RESOURCE_COUNT, THREAD_COUNT);

    // Initialize semaphore with initial count = 3 (0 for thread-shared)
    sem_init(&resource_pool, 0, RESOURCE_COUNT);

    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, worker, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&resource_pool);
    printf("\n\033[1;32m[SUCCESS]\033[0m All 6 threads mediated safely through the 3-resource pool.\n");
    return 0;
}
