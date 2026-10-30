/*
 * ============================================================================
 * Course Outcome 3: Inter-Process Communication (IPC)
 * Program: shared_memory.c
 * Purpose: Demonstrates System V Shared Memory (shmget, shmat, shmdt, shmctl)
 *          allowing direct zero-copy RAM sharing between independent processes.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

struct SharedData {
    int counter;
    char message[256];
    int flag; // 0 = empty, 1 = written
};

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("     CO3: System V Shared Memory System Calls Demonstration    \n");
    printf("===============================================================\n\n");

    // 1. Create shared memory segment
    printf("1. Calling shmget(IPC_PRIVATE, %d, IPC_CREAT | 0666)...\n", SHM_SIZE);
    int shmid = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    printf("   Allocated Shared Memory Segment ID (shmid): %d\n", shmid);

    // 2. Attach shared memory in parent
    printf("2. Calling shmat() to attach segment into address space...\n");
    struct SharedData *shared = (struct SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
    printf("   Attached at virtual address: %p\n\n", (void *)shared);

    shared->counter = 0;
    shared->flag = 0;
    strcpy(shared->message, "Initial shared memory data");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process: Reader / Modifier
        printf("\033[1;32m[CHILD PID %d]\033[0m Attached to shared memory segment!\n", getpid());
        printf("  Read existing data: counter=%d, message=\"%s\"\n", shared->counter, shared->message);
        
        printf("  Modifying shared data directly in RAM...\n");
        shared->counter = 999;
        strncpy(shared->message, "Updated by Child Process over zero-copy RAM!", sizeof(shared->message));
        shared->flag = 1;

        printf("  Detaching with shmdt()...\n");
        shmdt(shared);
        exit(0);
    } else {
        // Parent Process: Verifier
        wait(NULL);

        printf("\n\033[1;35m[PARENT PID %d]\033[0m Examining shared memory after child exited...\n", getpid());
        printf("  shared->counter: \033[1;32m%d\033[0m\n", shared->counter);
        printf("  shared->message: \033[1;32m\"%s\"\033[0m\n", shared->message);
        printf("  shared->flag:    %d\n", shared->flag);

        printf("\n3. Detaching segment with shmdt(%p)...\n", (void *)shared);
        shmdt(shared);

        printf("4. Deallocating kernel segment with shmctl(shmid, IPC_RMID, NULL)...\n");
        shmctl(shmid, IPC_RMID, NULL);
        printf("\033[1;32m[SUCCESS]\033[0m Shared memory segment freed successfully.\n");
    }

    return 0;
}
