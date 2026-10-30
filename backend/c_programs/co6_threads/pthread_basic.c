/*
 * ============================================================================
 * Course Outcome 6: Thread Management and Synchronization
 * Program: pthread_basic.c
 * Purpose: Demonstrates POSIX Threads (pthread_create, pthread_join, pthread_self),
 *          passing structures to threads, and retrieving return values.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct {
    int thread_id;
    int start_num;
    int count;
    long result_sum;
} ThreadData;

void *worker_function(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    pthread_t sys_tid = pthread_self();

    printf("  \033[1;36m[Thread %d]\033[0m Started! Native pthread ID: %lu\n", data->thread_id, (unsigned long)sys_tid);

    long sum = 0;
    for (int i = 0; i < data->count; i++) {
        sum += (data->start_num + i);
    }
    data->result_sum = sum;

    usleep(100000); // 100ms work simulation
    printf("  \033[1;36m[Thread %d]\033[0m Completed sum = %ld. Exiting.\n", data->thread_id, sum);

    pthread_exit((void *)(long)data->result_sum);
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("         CO6: POSIX Threads (pthread_create & pthread_join)    \n");
    printf("===============================================================\n\n");

    pthread_t threads[NUM_THREADS];
    ThreadData thread_args[NUM_THREADS];

    printf("Main thread (PID %d) spawning %d POSIX worker threads...\n\n", getpid(), NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].thread_id = i + 1;
        thread_args[i].start_num = i * 100 + 1;
        thread_args[i].count = 100;
        thread_args[i].result_sum = 0;

        printf("Calling pthread_create for Thread %d...\n", i + 1);
        if (pthread_create(&threads[i], NULL, worker_function, (void *)&thread_args[i]) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }

    printf("\nMain thread waiting for all workers to join (pthread_join)...\n\n");
    long grand_total = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        void *status;
        pthread_join(threads[i], &status);
        long ret_val = (long)status;
        printf("Main thread: Joined Thread %d | Return value: %ld\n", i + 1, ret_val);
        grand_total += ret_val;
    }

    printf("\n\033[1;32m[RESULT]\033[0m All threads joined. Grand total sum: %ld\n", grand_total);
    return 0;
}
