/*
 * ============================================================================
 * Course Outcome 5: File Management
 * Program: lseek_demo.c
 * Purpose: Demonstrates lseek() system call: manipulating file offset pointers,
 *          random access reading, and calculating file sizes.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LSEEK_FILE "/tmp/os_lseek_demo.txt"

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("           CO5: lseek() System Call Demonstration             \n");
    printf("===============================================================\n\n");

    // Create a known sample file: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    int fd = open(LSEEK_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    write(fd, alphabet, strlen(alphabet));
    printf("Created test file '%s' with %zu bytes: \"%s\"\n\n", LSEEK_FILE, strlen(alphabet), alphabet);

    // 1. SEEK_SET
    off_t offset = lseek(fd, 10, SEEK_SET);
    printf("1. lseek(fd, 10, SEEK_SET) -> New Offset: %ld\n", (long)offset);
    char buf[6] = {0};
    read(fd, buf, 5);
    printf("   Read 5 bytes starting at offset 10: \033[1;32m\"%s\"\033[0m (Expected: KLMNO)\n\n", buf);

    // 2. SEEK_CUR
    offset = lseek(fd, 5, SEEK_CUR);
    printf("2. lseek(fd, 5, SEEK_CUR) -> Advanced 5 bytes ahead. New Offset: %ld\n", (long)offset);
    memset(buf, 0, sizeof(buf));
    read(fd, buf, 5);
    printf("   Read 5 bytes: \033[1;32m\"%s\"\033[0m (Expected: UVWXY)\n\n", buf);

    // 3. SEEK_END
    offset = lseek(fd, -10, SEEK_END);
    printf("3. lseek(fd, -10, SEEK_END) -> Rewound 10 bytes from end. New Offset: %ld\n", (long)offset);
    memset(buf, 0, sizeof(buf));
    read(fd, buf, 10);
    printf("   Read last 10 characters: \033[1;32m\"%s\"\033[0m (Expected: 0123456789)\n\n", buf);

    // 4. Determining total file size via lseek
    off_t file_size = lseek(fd, 0, SEEK_END);
    printf("4. Determining file size via lseek(fd, 0, SEEK_END): \033[1;33m%ld bytes\033[0m\n", (long)file_size);

    close(fd);
    unlink(LSEEK_FILE);
    printf("\n\033[1;32m[DONE]\033[0m Random access file positioning with lseek() demonstrated.\n");

    return 0;
}
