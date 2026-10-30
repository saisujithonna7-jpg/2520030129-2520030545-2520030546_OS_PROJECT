/*
 * ============================================================================
 * Course Outcome 5: File Management
 * Program: file_crud.c
 * Purpose: Demonstrates fundamental low-level file I/O system calls:
 *          open(), write(), read(), and close() with file descriptors.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEMO_FILE "/tmp/os_demo_file.txt"

int main(int argc, char *argv[]) {
    const char *data_to_write = "Operating System System Calls in Linux: open, write, read, close.\n"
                                "Direct interaction with the Virtual File System (VFS).\n";

    if (argc > 1) {
        data_to_write = argv[1];
    }

    printf("===============================================================\n");
    printf("   CO5: File Management: open(), write(), read(), close()      \n");
    printf("===============================================================\n\n");

    // 1. open() with O_CREAT | O_WRONLY | O_TRUNC
    printf("1. Calling open('%s', O_CREAT | O_WRONLY | O_TRUNC, 0644)...\n", DEMO_FILE);
    int fd = open(DEMO_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open for write failed");
        exit(1);
    }
    printf("   File opened. Allocated File Descriptor index: \033[1;32m%d\033[0m\n", fd);

    // 2. write()
    size_t len = strlen(data_to_write);
    printf("2. Calling write(%d, buffer, %zu bytes)...\n", fd, len);
    ssize_t bytes_written = write(fd, data_to_write, len);
    if (bytes_written < 0) {
        perror("write failed");
        close(fd);
        exit(1);
    }
    printf("   Successfully flushed \033[1;32m%zd\033[0m bytes to Linux page cache.\n", bytes_written);

    // 3. close()
    printf("3. Calling close(%d) to release file descriptor...\n", fd);
    if (close(fd) < 0) {
        perror("close failed");
    }
    printf("   File descriptor %d closed.\n\n", fd);

    // 4. open() for reading
    printf("4. Calling open('%s', O_RDONLY)...\n", DEMO_FILE);
    fd = open(DEMO_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open for read failed");
        exit(1);
    }
    printf("   Re-opened for read. New File Descriptor: \033[1;32m%d\033[0m\n", fd);

    // 5. read()
    char read_buf[512];
    printf("5. Calling read(%d, buffer, %zu)...\n", fd, sizeof(read_buf) - 1);
    ssize_t bytes_read = read(fd, read_buf, sizeof(read_buf) - 1);
    if (bytes_read < 0) {
        perror("read failed");
        close(fd);
        exit(1);
    }
    read_buf[bytes_read] = '\0';
    printf("   Bytes read: %zd\n", bytes_read);
    printf("---------------- Content Read from File ----------------\n");
    printf("\033[1;36m%s\033[0m", read_buf);
    printf("--------------------------------------------------------\n\n");

    // 6. Final close()
    close(fd);
    printf("6. Closed file descriptor %d.\n", fd);
    printf("\033[1;32m[DONE]\033[0m File open, write, read, and close cycle completed.\n");

    return 0;
}
