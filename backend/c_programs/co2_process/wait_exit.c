/*
 * ============================================================================
 * Course Outcome 2: Process Management
 * Program: wait_exit.c
 * Purpose: Demonstrates wait(), waitpid(), exit() system calls, exit status
 *          inspection macros, and zombie process cleanup (reaping).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int requested_exit_code = 42;
    if (argc > 1) {
        requested_exit_code = atoi(argv[1]);
    }

    printf("===============================================================\n");
    printf("        CO2: wait() and exit() System Call Demonstration      \n");
    printf("===============================================================\n\n");

    printf("\033[1;34m[PARENT]\033[0m PID %d creating child to demonstrate exit code transmission...\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        printf("\033[1;32m[CHILD]\033[0m PID %d starting work...\n", getpid());
        sleep(1);
        printf("\033[1;32m[CHILD]\033[0m Calling exit(%d)...\n", requested_exit_code);
        printf("        Kernel will close open file descriptors and release memory pages.\n");
        printf("        Process enters ZOMBIE state until parent calls wait().\n");
        exit(requested_exit_code);
    } else {
        // Parent Process
        printf("\033[1;35m[PARENT]\033[0m Child PID is %d. Calling waitpid()...\n", pid);
        int status;
        pid_t terminated_pid = waitpid(pid, &status, 0);

        printf("\033[1;35m[PARENT]\033[0m Reaped Child PID: %d\n", terminated_pid);

        if (WIFEXITED(status)) {
            printf("  -> Normal Termination (WIFEXITED is true)\n");
            printf("  -> Exit Status Code (WEXITSTATUS): \033[1;32m%d\033[0m\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("  -> Terminated by Signal (WIFSIGNALED is true)\n");
            printf("  -> Signal Number (WTERMSIG): \033[1;31m%d\033[0m\n", WTERMSIG(status));
        }

        printf("\nZombie process successfully cleared from the OS Process Table!\n");
    }

    return 0;
}
