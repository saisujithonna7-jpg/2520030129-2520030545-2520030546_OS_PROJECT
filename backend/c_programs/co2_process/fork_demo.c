/*
 * ============================================================================
 * Course Outcome 2: Process Management
 * Program: fork_demo.c
 * Purpose: Demonstrates the fork() system call, process duplication, separate
 *          address spaces, and concurrent execution of parent and child.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_var = 100; // Lives in Data segment

int main(int argc, char *argv[]) {
    int local_var = 50; // Lives on Stack

    printf("===============================================================\n");
    printf("            CO2: fork() System Call Demonstration             \n");
    printf("===============================================================\n\n");

    printf("\033[1;34m[BEFORE FORK]\033[0m Initial State in Original Process:\n");
    printf("  PID: %d | PPID: %d\n", getpid(), getppid());
    printf("  global_var = %d (address: %p)\n", global_var, (void *)&global_var);
    printf("  local_var  = %d (address: %p)\n\n", local_var, (void *)&local_var);

    printf("--> Calling fork() system call...\n\n");
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child Process
        printf("\033[1;32m[CHILD PROCESS]\033[0m fork() returned 0!\n");
        printf("  My PID: %d | Parent PID: %d\n", getpid(), getppid());
        printf("  Modifying variables in Child...\n");
        global_var += 25;
        local_var += 25;
        printf("  In Child: global_var = %d | local_var = %d\n", global_var, local_var);
        printf("  Note: Child has an exact duplicate of parent's address space at fork time,\n");
        printf("        but modifications are isolated due to Copy-on-Write (COW).\n");
        sleep(1);
        printf("\033[1;32m[CHILD PROCESS]\033[0m Exiting cleanly with code 0.\n\n");
        exit(0);
    } else {
        // Parent Process
        printf("\033[1;35m[PARENT PROCESS]\033[0m fork() returned Child PID = %d\n", pid);
        printf("  My PID: %d | My Parent PID: %d\n", getpid(), getppid());
        printf("  Waiting for child to finish...\n");
        
        int status;
        wait(&status); // Wait for child state change
        
        printf("\033[1;35m[PARENT PROCESS]\033[0m Child finished.\n");
        printf("  In Parent: global_var = %d | local_var = %d\n", global_var, local_var);
        printf("  Proof of Isolation: Parent variables remain UNCHANGED (%d, %d)!\n", global_var, local_var);
    }

    return 0;
}
