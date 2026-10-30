/*
 * ============================================================================
 * Course Outcome 1: Operating System Architecture
 * Program: mini_shell.c
 * Purpose: Demonstrates how the Linux Shell runs entirely as a User-Space
 *          program, using fork(), execvp(), and waitpid() to execute commands.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void execute_command(char *cmd_line) {
    char *tokens[32];
    int token_count = 0;

    char *token = strtok(cmd_line, " \t\r\n");
    while (token != NULL && token_count < 31) {
        tokens[token_count++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    tokens[token_count] = NULL;

    if (token_count == 0) return;

    printf("\n\033[1;36m[SHELL LOOP]\033[0m Processing command: '%s'\n", tokens[0]);
    printf(" 1. Shell (PID %d) invokes fork() system call to create child...\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child Process
        printf(" 2. [Child Process PID %d] Created successfully!\n", getpid());
        printf(" 3. [Child Process PID %d] Invoking execvp('%s')...\n", getpid(), tokens[0]);
        printf("    (Replacing memory address space with target binary image)\n");
        printf("------------------ Command Output Begin ------------------\n");
        execvp(tokens[0], tokens);

        // If execvp returns, an error occurred
        perror("execvp failed");
        exit(127);
    } else {
        // Parent Process (Shell)
        printf(" 4. [Parent Shell PID %d] Calling waitpid(%d, &status, 0)...\n", getpid(), pid);
        int status;
        waitpid(pid, &status, 0);
        printf("------------------ Command Output End --------------------\n");
        if (WIFEXITED(status)) {
            printf(" 5. [Parent Shell PID %d] Child %d terminated normally with exit code: %d\n",
                   getpid(), pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf(" 5. [Parent Shell PID %d] Child %d killed by signal: %d\n",
                   getpid(), pid, WTERMSIG(status));
        }
    }
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("       CO1: Shell as a User-Space Program Demonstration        \n");
    printf("===============================================================\n");
    printf("Shell architecture: An infinite loop doing Read-Eval-Print (REPL)\n");
    printf("Built with standard POSIX system calls: fork(), execvp(), waitpid().\n");

    if (argc > 1) {
        // Run command specified via arguments
        char buffer[256] = {0};
        for (int i = 1; i < argc; i++) {
            strcat(buffer, argv[i]);
            if (i < argc - 1) strcat(buffer, " ");
        }
        execute_command(buffer);
    } else {
        // Run demonstration batch
        char cmd1[] = "uname -r";
        char cmd2[] = "whoami";
        char cmd3[] = "date";

        execute_command(cmd1);
        execute_command(cmd2);
        execute_command(cmd3);
    }

    printf("\n\033[1;32m[SHELL EXIT]\033[0m All commands executed via user-space fork-exec flow.\n");
    return 0;
}
