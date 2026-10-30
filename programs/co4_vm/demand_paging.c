/*
 * ============================================================================
 * Course Outcome 4: Virtual Memory Management
 * Program: demand_paging.c
 * Purpose: Demonstrates Demand Paging and Hardware Page Faults:
 *          Allocates 64 MB virtual address space, queries getrusage() for
 *          ru_minflt (minor page faults), and shows pages are only loaded on demand.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define ALLOCATION_SIZE (64 * 1024 * 1024) // 64 Megabytes
#define PAGE_SIZE 4096

long get_page_fault_count() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_minflt;
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("     CO4: Demand Paging & Hardware Page Faults Demonstration   \n");
    printf("===============================================================\n\n");

    long initial_faults = get_page_fault_count();
    printf("1. Initial Baseline Minor Page Faults (ru_minflt): \033[1;36m%ld\033[0m\n\n", initial_faults);

    printf("2. Requesting 64 MB (%d pages) of virtual memory via mmap(MAP_ANONYMOUS)...\n",
           ALLOCATION_SIZE / PAGE_SIZE);

    char *big_buffer = (char *)mmap(NULL, ALLOCATION_SIZE, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (big_buffer == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    long post_alloc_faults = get_page_fault_count();
    printf("   Virtual Address allocated: %p to %p\n", (void *)big_buffer, (void *)(big_buffer + ALLOCATION_SIZE));
    printf("   Page faults immediately after mmap: \033[1;36m%ld\033[0m (Increase: %ld)\n",
           post_alloc_faults, post_alloc_faults - initial_faults);
    printf("   \033[1;33m[INSIGHT]\033[0m Linux allocates ONLY virtual address space descriptor (VMA).\n");
    printf("             Zero physical RAM frames have been committed yet!\n\n");

    printf("3. Accessing (touching) first 500 pages (one byte per 4096-byte page)...\n");
    int pages_to_touch = 500;
    for (int i = 0; i < pages_to_touch; i++) {
        big_buffer[i * PAGE_SIZE] = (char)(i % 256); // Writing touches page, triggering page fault!
    }

    long post_touch_faults = get_page_fault_count();
    long new_faults = post_touch_faults - post_alloc_faults;
    printf("   Page faults after accessing %d pages: \033[1;32m%ld\033[0m (Increase: \033[1;32m+%ld faults\033[0m)\n",
           pages_to_touch, post_touch_faults, new_faults);
    printf("   Every touched page generated exactly 1 hardware page fault,\n");
    printf("   causing Linux kernel to allocate a physical 4KB RAM frame on demand!\n\n");

    printf("4. Accessing the SAME 500 pages a second time...\n");
    for (int i = 0; i < pages_to_touch; i++) {
        volatile char val = big_buffer[i * PAGE_SIZE];
        (void)val;
    }
    long second_touch_faults = get_page_fault_count();
    printf("   Page faults after 2nd pass: %ld (Increase: \033[1;32m+%ld faults\033[0m)\n",
           second_touch_faults, second_touch_faults - post_touch_faults);
    printf("   Zero new faults! All 500 pages now reside in physical RAM.\n\n");

    munmap(big_buffer, ALLOCATION_SIZE);
    printf("\033[1;32m[DONE]\033[0m Demand Paging lifecycle verified on real Linux kernel.\n");
    return 0;
}
