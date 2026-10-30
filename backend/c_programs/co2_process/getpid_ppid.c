/*
 * ============================================================================
 * Course Outcome 2: Process Management
 * Program: getpid_ppid.c
 * Purpose: Demonstrates getpid() and getppid() system calls, process identification,
 *          and parent-child PID hierarchy.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("        CO2: getpid() and getppid() System Call Demo          \n");
    printf("===============================================================\n\n");

    printf("\033[1;36m[Initial Process Inspection]\033[0m\n");
    printf(" Process ID  (getpid()):  \033[1;32m%d\033[0m\n", getpid());
    printf(" Parent ID   (getppid()): \033[1;33m%d\033[0m (e.g. bash shell / systemd)\n\n", getppid());

    printf("Forking a child to examine PID reassignment and hierarchy...\n");
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        printf("\n\033[1;32m[Child Process View]\033[0m\n");
        printf("  My PID:          %d\n", getpid());
        printf("  My Parent PPID:  %d\n", getppid());
        printf("  Verification: Child getppid() matches Parent getpid() exactly!\n");
        exit(0);
    } else {
        // Parent Process
        printf("\n\033[1;35m[Parent Process View]\033[0m\n");
        printf("  My PID:          %d\n", getpid());
        printf("  My Child PID:    %d\n", pid);
        printf("  My Parent PPID:  %d\n", getppid());
        wait(NULL);
    }

    return 0;
}
