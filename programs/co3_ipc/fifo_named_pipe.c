/*
 * ============================================================================
 * Course Outcome 3: Inter-Process Communication (IPC)
 * Program: fifo_named_pipe.c
 * Purpose: Demonstrates Named Pipes (FIFO) using mkfifo() system call,
 *          persistent filesystem presence, and inter-process communication.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define FIFO_PATH "/tmp/demo_named_pipe"

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("        CO3: Named Pipe (FIFO) System Call Demonstration       \n");
    printf("===============================================================\n\n");

    // Remove old FIFO if exists
    unlink(FIFO_PATH);

    printf("1. Creating Named Pipe in filesystem via mkfifo('%s', 0666)...\n", FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo error");
            exit(1);
        }
    }
    printf("   FIFO special node created. File type in stat: 'p' (named pipe).\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process: Reader
        printf("\033[1;32m[READER CHILD PID %d]\033[0m Opening FIFO for reading (O_RDONLY)...\n", getpid());
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) {
            perror("Child open error");
            exit(1);
        }
        printf("\033[1;32m[READER CHILD]\033[0m FIFO opened successfully (blocking wait unblocked)!\n");

        char read_buffer[256];
        ssize_t bytes_read = read(fd, read_buffer, sizeof(read_buffer) - 1);
        if (bytes_read > 0) {
            read_buffer[bytes_read] = '\0';
            printf("\033[1;32m[READER CHILD RECEIVED]:\033[0m \"%s\"\n", read_buffer);
        }
        close(fd);
        exit(0);
    } else {
        // Parent Process: Writer
        sleep(1); // Give reader a moment to block on open
        printf("\033[1;35m[WRITER PARENT PID %d]\033[0m Opening FIFO for writing (O_WRONLY)...\n", getpid());
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd < 0) {
            perror("Parent open error");
            exit(1);
        }

        const char *message = "Data packet transmitted through POSIX FIFO filesystem node.";
        printf("\033[1;35m[WRITER PARENT]\033[0m Sending: \"%s\"\n", message);
        write(fd, message, strlen(message));
        close(fd);

        wait(NULL);

        printf("\nUnlinking FIFO file '%s' from filesystem...\n", FIFO_PATH);
        unlink(FIFO_PATH);
        printf("\033[1;32m[DONE]\033[0m Named pipe demo completed and cleaned up.\n");
    }

    return 0;
}
