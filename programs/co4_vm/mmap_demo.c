/*
 * ============================================================================
 * Course Outcome 4: Virtual Memory Management
 * Program: mmap_demo.c
 * Purpose: Demonstrates mmap() and munmap() system calls: mapping virtual memory
 *          pages directly into user address space, modifying memory, and unmapping.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MMAP_FILE "/tmp/os_mmap_demo.dat"
#define PAGE_SIZE 4096

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("       CO4: mmap() and munmap() System Calls Demonstration     \n");
    printf("===============================================================\n\n");

    // 1. Anonymous memory mapping (allocating virtual memory pages directly from kernel)
    printf("\033[1;36m[TEST 1: Anonymous Memory Allocation via mmap]\033[0m\n");
    printf("Calling mmap(NULL, %d, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)...\n", PAGE_SIZE);

    char *anon_mem = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (anon_mem == MAP_FAILED) {
        perror("mmap anonymous failed");
        exit(1);
    }

    printf("  Allocated Page Virtual Address: \033[1;32m%p\033[0m\n", (void *)anon_mem);
    strcpy(anon_mem, "Direct zero-syscall memory write into mmap'd virtual page!");
    printf("  Wrote content to page: \"%s\"\n", anon_mem);

    printf("  Calling munmap(%p, %d) to return page to kernel...\n", (void *)anon_mem, PAGE_SIZE);
    munmap(anon_mem, PAGE_SIZE);
    printf("  Page successfully unmapped from process page tables.\n\n");

    // 2. File-backed memory mapping
    printf("\033[1;33m[TEST 2: File-Backed Memory Mapping]\033[0m\n");
    int fd = open(MMAP_FILE, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    // Set file size to 1 page (4096 bytes)
    ftruncate(fd, PAGE_SIZE);
    write(fd, "LINUX-VIRTUAL-MEMORY-SYSTEM", 27);

    printf("Mapping file '%s' into address space...\n", MMAP_FILE);
    char *file_mem = (char *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_mem == MAP_FAILED) {
        perror("mmap file failed");
        close(fd);
        exit(1);
    }

    printf("  File mapped at memory address: \033[1;32m%p\033[0m\n", (void *)file_mem);
    printf("  Read directly via memory pointer: \"%.27s\"\n", file_mem);

    printf("  Overwriting first 5 bytes via direct pointer assignment: 'UBUNT'...\n");
    file_mem[0] = 'U';
    file_mem[1] = 'B';
    file_mem[2] = 'U';
    file_mem[3] = 'N';
    file_mem[4] = 'T';

    printf("  Calling msync(MS_SYNC) to flush dirty page cache to disk...\n");
    msync(file_mem, PAGE_SIZE, MS_SYNC);

    munmap(file_mem, PAGE_SIZE);
    close(fd);

    // Verify disk file was updated without write() syscall!
    fd = open(MMAP_FILE, O_RDONLY);
    char verify_buf[32] = {0};
    read(fd, verify_buf, 27);
    close(fd);
    unlink(MMAP_FILE);

    printf("  Content read back from file: \033[1;32m\"%s\"\033[0m\n", verify_buf);
    printf("\n\033[1;32m[DONE]\033[0m High performance memory-mapped I/O verified.\n");

    return 0;
}
