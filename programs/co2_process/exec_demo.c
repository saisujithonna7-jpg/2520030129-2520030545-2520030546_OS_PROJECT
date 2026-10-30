/*
 * ============================================================================
 * Course Outcome 2: Process Management
 * Program: exec_demo.c
 * Purpose: Demonstrates the exec() family of system calls (execvp / execlp).
 *          Shows how exec completely replaces code, stack, and heap segments.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("         CO2: exec() System Call Family Demonstration          \n");
    printf("===============================================================\n\n");

    printf("\033[1;34m[PARENT PROCESS]\033[0m PID: %d\n", getpid());
    printf("exec() does NOT create a new process; it transforms the current process\n");
    printf("into a new program binary specified in the arguments.\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        printf("\033[1;32m[CHILD PROCESS BEFORE EXEC]\033[0m PID: %d, PPID: %d\n", getpid(), getppid());
        printf("Preparing to execute '/bin/ls -l --color=auto' via execvp()...\n");
        printf("Notice: Any instructions following execvp() will NEVER execute\n");
        printf("        unless the execvp call encounters an error!\n");
        printf("-------------------- Output of /bin/ls --------------------\n");
        fflush(stdout);

        char *args[] = {"ls", "-lh", "/tmp", NULL};
        execvp(args[0], args);

        // This line only reached if execvp failed
        perror("\033[1;31mexecvp failed\033[0m");
        exit(1);
    } else {
        // Parent Process
        int status;
        waitpid(pid, &status, 0);
        printf("-----------------------------------------------------------\n");
        printf("\033[1;35m[PARENT PROCESS AFTER EXEC]\033[0m Child PID %d executed and returned.\n", pid);
        if (WIFEXITED(status)) {
            printf("Child program exited with status: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
