/*
 * ============================================================================
 * Course Outcome 5: File Management
 * Program: stat_inode.c
 * Purpose: Demonstrates stat() and fstat() system calls: querying file metadata,
 *          inode data structures, permission bitmasks, and timestamps.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void print_file_stats(const char *path) {
    struct stat file_stat;

    if (stat(path, &file_stat) < 0) {
        perror("stat failed");
        return;
    }

    printf("Metadata for file: \033[1;36m%s\033[0m\n", path);
    printf("-----------------------------------------------------------\n");
    printf("  Inode Number (st_ino):        \033[1;32m%lu\033[0m\n", (unsigned long)file_stat.st_ino);
    printf("  Device ID (st_dev):           %lu\n", (unsigned long)file_stat.st_dev);
    printf("  Hard Link Count (st_nlink):   %lu\n", (unsigned long)file_stat.st_nlink);
    printf("  Owner UID:                    %u\n", file_stat.st_uid);
    printf("  Group GID:                    %u\n", file_stat.st_gid);
    printf("  Total Size (st_size):         \033[1;33m%ld bytes\033[0m\n", (long)file_stat.st_size);
    printf("  Block Size (st_blksize):      %ld bytes\n", (long)file_stat.st_blksize);
    printf("  Blocks Allocated (st_blocks): %ld\n", (long)file_stat.st_blocks);

    printf("  File Type:                    ");
    if (S_ISREG(file_stat.st_mode))  printf("Regular File (-)\n");
    else if (S_ISDIR(file_stat.st_mode)) printf("Directory (d)\n");
    else if (S_ISLNK(file_stat.st_mode)) printf("Symbolic Link (l)\n");
    else if (S_ISFIFO(file_stat.st_mode)) printf("Named Pipe (FIFO) (p)\n");
    else if (S_ISSOCK(file_stat.st_mode)) printf("Socket (s)\n");
    else printf("Other\n");

    printf("  Permissions (Octal):          0%o\n", file_stat.st_mode & 0777);
    printf("  Permissions (rwxrwxrwx):      ");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    char time_buf[64];
    struct tm *tm_info = localtime(&file_stat.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("  Last Modified (st_mtime):     %s\n", time_buf);
    printf("-----------------------------------------------------------\n");
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("         CO5: stat() / fstat() Inode Query Demonstration       \n");
    printf("===============================================================\n\n");

    const char *target = (argc > 1) ? argv[1] : "/etc/passwd";
    print_file_stats(target);

    return 0;
}
