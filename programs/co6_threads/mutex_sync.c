/*
 * ============================================================================
 * Course Outcome 6: Thread Management and Synchronization
 * Program: mutex_sync.c
 * Purpose: Demonstrates Race Conditions on shared variables and how POSIX Mutex
 *          Locks (pthread_mutex_lock/unlock) ensure Mutual Exclusion.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define ITERATIONS 500000
#define NUM_THREADS 4

long unprotected_counter = 0;
long protected_counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *race_worker(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        unprotected_counter++; // Critical Section without protection: RACE CONDITION!
    }
    return NULL;
}

void *mutex_worker(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock);   // Enter Critical Section
        protected_counter++;
        pthread_mutex_unlock(&lock); // Leave Critical Section
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("   CO6: Race Conditions vs Mutex Synchronization Demo         \n");
    printf("===============================================================\n");
    printf("Configuration: %d threads, each incrementing counter %d times.\n", NUM_THREADS, ITERATIONS);
    printf("Expected Final Total: \033[1;33m%ld\033[0m\n\n", (long)NUM_THREADS * ITERATIONS);

    // TEST 1: WITHOUT MUTEX (RACE CONDITION)
    printf("\033[1;31m[TEST 1: WITHOUT MUTEX (Race Condition Activated)]\033[0m\n");
    pthread_t threads[NUM_THREADS];
    unprotected_counter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, race_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Expected: %ld\n", (long)NUM_THREADS * ITERATIONS);
    printf("  Actual:   \033[1;31m%ld\033[0m\n", unprotected_counter);
    long lost_updates = ((long)NUM_THREADS * ITERATIONS) - unprotected_counter;
    printf("  Lost Updates Due to Race Condition: \033[1;31m%ld\033[0m\n", lost_updates);
    printf("  Reason: Non-atomic Read-Modify-Write cycle on shared memory register.\n\n");

    // TEST 2: WITH MUTEX LOCKS
    printf("\033[1;32m[TEST 2: WITH MUTEX LOCKS (pthread_mutex_lock/unlock)]\033[0m\n");
    protected_counter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, mutex_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("  Expected: %ld\n", (long)NUM_THREADS * ITERATIONS);
    printf("  Actual:   \033[1;32m%ld\033[0m\n", protected_counter);
    printf("  Lost Updates: \033[1;32m0 (Exact match)\033[0m\n\n");

    pthread_mutex_destroy(&lock);
    printf("\033[1;32m[CONCLUSION]\033[0m Mutual exclusion guarantees atomic memory transactions.\n");
    return 0;
}
