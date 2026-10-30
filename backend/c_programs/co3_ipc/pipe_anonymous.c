/*
 * ============================================================================
 * Course Outcome 3: Inter-Process Communication (IPC)
 * Program: pipe_anonymous.c
 * Purpose: Demonstrates Anonymous Pipes (pipe() system call), unidirectional
 *          kernel data buffer, file descriptor management, and IPC.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2]; // pipefd[0] = Read end, pipefd[1] = Write end
    char buffer[256];
    const char *custom_msg = "Hello from Parent! IPC Pipe is working on Ubuntu!";

    if (argc > 1) {
        custom_msg = argv[1];
    }

    printf("===============================================================\n");
    printf("         CO3: Anonymous Pipe System Call Demonstration         \n");
    printf("===============================================================\n\n");

    printf("1. Invoking pipe(pipefd) system call...\n");
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    printf("   Pipe created in Linux kernel buffer (typically 64 KB capacity).\n");
    printf("   Read File Descriptor  [pipefd[0]]: %d\n", pipefd[0]);
    printf("   Write File Descriptor [pipefd[1]]: %d\n\n", pipefd[1]);

    printf("2. Calling fork() to create communicating processes...\n");
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child Process: Reader
        printf("\033[1;32m[CHILD - READER]\033[0m PID %d started.\n", getpid());
        printf("  Closing unused write end (fd %d)...\n", pipefd[1]);
        close(pipefd[1]); // Best practice: close unused write end

        printf("  Waiting to read data from kernel pipe buffer...\n");
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("  \033[1;32m[CHILD RECEIVED %zd BYTES]:\033[0m \"%s\"\n", bytes_read, buffer);
        } else {
            printf("  Read EOF or error.\n");
        }

        close(pipefd[0]);
        printf("  Reader closed fd %d. Exiting.\n", pipefd[0]);
        exit(0);
    } else {
        // Parent Process: Writer
        printf("\033[1;35m[PARENT - WRITER]\033[0m PID %d started.\n", getpid());
        printf("  Closing unused read end (fd %d)...\n", pipefd[0]);
        close(pipefd[0]); // Best practice: close unused read end

        printf("  Writing message to pipe: \"%s\"\n", custom_msg);
        ssize_t bytes_written = write(pipefd[1], custom_msg, strlen(custom_msg));
        printf("  \033[1;35m[PARENT TRANSMITTED %zd BYTES]\033[0m\n", bytes_written);

        close(pipefd[1]); // Closing write end sends EOF to reader
        wait(NULL);
        printf("\n\033[1;32m[SUCCESS]\033[0m Pipe communication completed synchronously.\n");
    }

    return 0;
}
