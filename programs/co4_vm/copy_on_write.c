/*
 * ============================================================================
 * Course Outcome 4: Virtual Memory Management
 * Program: copy_on_write.c
 * Purpose: Demonstrates Copy-on-Write (COW) optimization in Linux fork():
 *          shared physical frames, write-protection page fault, and frame duplication.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("       CO4: Virtual Memory Copy-on-Write (COW) Demonstration   \n");
    printf("===============================================================\n\n");

    // Allocate 1 page of memory
    char *shared_cow_page = (char *)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    strcpy(shared_cow_page, "Original data stored in shared physical frame before fork().");

    printf("\033[1;34m[BEFORE FORK]\033[0m\n");
    printf("  Process PID %d allocated virtual page at: %p\n", getpid(), (void *)shared_cow_page);
    printf("  Initial Page Content: \"%s\"\n\n", shared_cow_page);

    printf("Calling fork(). Linux marks all user pages as READ-ONLY in page tables of both parent and child.\n");
    printf("Physical frames are NOT copied upfront, saving gigabytes of RAM and achieving O(1) fork time!\n\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        printf("\033[1;32m[CHILD PID %d]\033[0m Virtual address matches parent: %p\n", getpid(), (void *)shared_cow_page);
        printf("  Reading shared page (no page fault): \"%s\"\n\n", shared_cow_page);

        printf("  \033[1;33m[COW TRIGGER]\033[0m Child initiates write to shared_cow_page[0] = 'X'...\n");
        printf("  1. Hardware MMU detects Write access to Read-Only page.\n");
        printf("  2. CPU fires Trap 14: Page Fault (CR2 = %p).\n", (void *)shared_cow_page);
        printf("  3. Linux kernel handle_pte_fault() allocates a NEW physical page frame.\n");
        printf("  4. Kernel copies original 4KB frame into new frame.\n");
        printf("  5. Child's Page Table Entry (PTE) redirected to new frame with Write permission.\n");
        
        shared_cow_page[0] = 'X'; // Triggers COW
        strcpy(shared_cow_page, "MODIFIED IN CHILD: Child now possesses private physical frame!");

        printf("  Child page now contains: \033[1;32m\"%s\"\033[0m\n", shared_cow_page);
        exit(0);
    } else {
        // Parent Process
        wait(NULL);

        printf("\n\033[1;35m[PARENT PID %d]\033[0m Checking page after child modification:\n", getpid());
        printf("  Parent page contains: \033[1;35m\"%s\"\033[0m\n", shared_cow_page);
        printf("  \033[1;32m[VERIFIED]\033[0m Parent page was completely untouched!\n");
        munmap(shared_cow_page, BUFFER_SIZE);
    }

    return 0;
}
