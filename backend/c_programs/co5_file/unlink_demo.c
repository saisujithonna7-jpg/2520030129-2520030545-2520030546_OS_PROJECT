/*
 * ============================================================================
 * Course Outcome 5: File Management
 * Program: unlink_demo.c
 * Purpose: Demonstrates unlink() system call: hard link count decrement,
 *          directory entry removal, and deferred inode deallocation.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define UNLINK_TARGET "/tmp/os_unlink_test.txt"

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("           CO5: unlink() System Call Demonstration            \n");
    printf("===============================================================\n\n");

    // 1. Create a file and hold an open file descriptor
    printf("1. Creating temporary file '%s'...\n", UNLINK_TARGET);
    int fd = open(UNLINK_TARGET, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    const char *data = "This data will remain readable even after unlink() while fd is open!\n";
    write(fd, data, strlen(data));

    struct stat st;
    stat(UNLINK_TARGET, &st);
    printf("   Initial state before unlink: Inode=%lu, Hard links (st_nlink)=%lu\n\n",
           (unsigned long)st.st_ino, (unsigned long)st.st_nlink);

    // 2. Call unlink() while the file descriptor is still open!
    printf("2. Calling unlink('%s') while file descriptor (fd=%d) is STILL OPEN...\n", UNLINK_TARGET, fd);
    if (unlink(UNLINK_TARGET) < 0) {
        perror("unlink failed");
        close(fd);
        exit(1);
    }
    printf("   unlink() returned 0 (Success)!\n");
    printf("   Directory entry removed. Trying to access via filename now:\n");

    int test_fd = open(UNLINK_TARGET, O_RDONLY);
    if (test_fd < 0) {
        printf("   \033[1;31m[CONFIRMED]\033[0m Path '%s' no longer exists in directory structure!\n\n", UNLINK_TARGET);
    }

    // 3. Prove that process can still read and write to the open descriptor
    printf("3. Demonstrating that kernel retains data blocks for open descriptor %d:\n", fd);
    lseek(fd, 0, SEEK_SET);
    char buf[128];
    ssize_t bytes = read(fd, buf, sizeof(buf) - 1);
    if (bytes > 0) {
        buf[bytes] = '\0';
        printf("   Read %zd bytes via existing fd: \033[1;32m\"%s\"\033[0m", bytes, buf);
    }

    // 4. Closing the file descriptor
    printf("4. Calling close(fd=%d)...\n", fd);
    close(fd);
    printf("   Now that link count == 0 AND open reference count == 0,\n");
    printf("   the Linux Kernel VFS frees the inode and deallocates data blocks!\n\n");

    printf("\033[1;32m[SUCCESS]\033[0m Linux POSIX unlinking semantics demonstrated.\n");
    return 0;
}
