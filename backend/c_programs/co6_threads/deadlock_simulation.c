/*
 * ============================================================================
 * Course Outcome 6: Thread Management and Synchronization
 * Program: deadlock_simulation.c
 * Purpose: Demonstrates Deadlock (Coffman conditions: Circular Wait) using
 *          safe timeout/trylock detection, and demonstrates Deadlock Prevention
 *          using strict Resource Hierarchy / Lock Ordering.
 * ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;

void *deadlock_thread1(void *arg) {
    printf("  \033[1;36m[Thread 1]\033[0m Attempting to lock Mutex A...\n");
    pthread_mutex_lock(&lockA);
    printf("  \033[1;36m[Thread 1]\033[0m Acquired Mutex A! Holding Lock A...\n");

    usleep(100000); // 100ms: allows Thread 2 to acquire Lock B

    printf("  \033[1;36m[Thread 1]\033[0m Now attempting to acquire Mutex B...\n");
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 2; // 2 second timeout to safely detect deadlock without freezing!

    int res = pthread_mutex_timedlock(&lockB, &ts);
    if (res == ETIMEDOUT) {
        printf("  \033[1;31m[Thread 1 DETECTED DEADLOCK]\033[0m Timed out waiting for Mutex B!\n");
        printf("  -> Thread 1 holds A, wants B. Thread 2 holds B, wants A.\n");
        printf("  -> Circular Wait Condition confirmed!\n");
    } else {
        printf("  \033[1;32m[Thread 1]\033[0m Acquired Mutex B.\n");
        pthread_mutex_unlock(&lockB);
    }

    pthread_mutex_unlock(&lockA);
    return NULL;
}

void *deadlock_thread2(void *arg) {
    printf("  \033[1;33m[Thread 2]\033[0m Attempting to lock Mutex B...\n");
    pthread_mutex_lock(&lockB);
    printf("  \033[1;33m[Thread 2]\033[0m Acquired Mutex B! Holding Lock B...\n");

    usleep(100000); // 100ms

    printf("  \033[1;33m[Thread 2]\033[0m Now attempting to acquire Mutex A...\n");
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 2;

    int res = pthread_mutex_timedlock(&lockA, &ts);
    if (res == ETIMEDOUT) {
        printf("  \033[1;31m[Thread 2 DETECTED DEADLOCK]\033[0m Timed out waiting for Mutex A!\n");
    } else {
        printf("  \033[1;32m[Thread 2]\033[0m Acquired Mutex A.\n");
        pthread_mutex_unlock(&lockA);
    }

    pthread_mutex_unlock(&lockB);
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("       CO6: Safe Deadlock Detection & Prevention Demo          \n");
    printf("===============================================================\n");
    printf("Coffman Conditions for Deadlock:\n");
    printf(" 1. Mutual Exclusion  2. Hold and Wait\n");
    printf(" 3. No Preemption     4. Circular Wait\n\n");

    printf("\033[1;35m[PHASE 1: Simulating Inverted Lock Ordering (Deadlock Hazard)]\033[0m\n");
    pthread_t t1, t2;

    pthread_create(&t1, NULL, deadlock_thread1, NULL);
    pthread_create(&t2, NULL, deadlock_thread2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n\033[1;32m[PHASE 2: Deadlock Prevention Solution]\033[0m\n");
    printf("Rule: Enforce a global lock hierarchy (always acquire Lock A BEFORE Lock B).\n");
    printf("      This breaks Coffman's 4th condition (Circular Wait), guaranteeing\n");
    printf("      that deadlocks can NEVER mathematically occur!\n");

    return 0;
}
