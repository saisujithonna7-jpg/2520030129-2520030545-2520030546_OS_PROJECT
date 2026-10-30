/*
 * ============================================================================
 * Course Outcome 4: Virtual Memory Management
 * Program: paging_translation.c
 * Purpose: Demonstrates Paging Architecture, Address Translation, and TLB:
 *          Extracting Virtual Page Number (VPN) and Page Offset from addresses,
 *          simulating Page Tables and calculating physical memory locations.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define PAGE_SIZE 4096 // 4 KB (12 bits offset)
#define OFFSET_MASK 0xFFF // 12 bits: 0000 1111 1111 1111
#define TLB_CAPACITY 4

typedef struct {
    uint64_t vpn;
    uint64_t pfn;
    int valid;
} TLBEntry;

TLBEntry tlb[TLB_CAPACITY];
int tlb_next_replace = 0;

void init_tlb() {
    for (int i = 0; i < TLB_CAPACITY; i++) {
        tlb[i].valid = 0;
    }
}

// Simulates TLB Cache lookup
int lookup_tlb(uint64_t vpn, uint64_t *pfn_out) {
    for (int i = 0; i < TLB_CAPACITY; i++) {
        if (tlb[i].valid && tlb[i].vpn == vpn) {
            *pfn_out = tlb[i].pfn;
            return 1; // TLB HIT!
        }
    }
    return 0; // TLB MISS!
}

void insert_tlb(uint64_t vpn, uint64_t pfn) {
    tlb[tlb_next_replace].vpn = vpn;
    tlb[tlb_next_replace].pfn = pfn;
    tlb[tlb_next_replace].valid = 1;
    tlb_next_replace = (tlb_next_replace + 1) % TLB_CAPACITY;
}

// Simulates Page Table Lookup (Page Table maps VPN to PFN)
uint64_t page_table_lookup(uint64_t vpn) {
    // Deterministic mock physical frame generator based on VPN
    return (vpn * 7 + 0x1A2) & 0xFFFFF;
}

void translate_address(uint64_t virtual_address) {
    uint64_t offset = virtual_address & OFFSET_MASK;
    uint64_t vpn = virtual_address >> 12;

    printf("\n---------------------------------------------------------------\n");
    printf(" Virtual Address: \033[1;36m0x%016lx\033[0m (%lu)\n", virtual_address, virtual_address);
    printf("---------------------------------------------------------------\n");
    printf(" 1. Offset (lower 12 bits):       0x%03lx (%lu bytes into 4KB page)\n", offset, offset);
    printf(" 2. Virtual Page Number (VPN):     0x%lx (%lu)\n", vpn, vpn);

    uint64_t pfn;
    int tlb_hit = lookup_tlb(vpn, &pfn);

    if (tlb_hit) {
        printf(" 3. TLB Lookup:                    \033[1;32m[TLB HIT!]\033[0m Found in CPU L1 TLB cache!\n");
        printf("    Physical Frame Number (PFN):   0x%lx\n", pfn);
    } else {
        printf(" 3. TLB Lookup:                    \033[1;31m[TLB MISS!]\033[0m Walking Multi-level Page Table (CR3)...\n");
        pfn = page_table_lookup(vpn);
        printf("    Page Table Entry (PTE) found! PFN = 0x%lx\n", pfn);
        insert_tlb(vpn, pfn);
        printf("    Populated TLB cache entry for VPN 0x%lx.\n", vpn);
    }

    uint64_t physical_address = (pfn << 12) | offset;
    printf(" 4. Physical Address Formula:      (PFN << 12) | Offset\n");
    printf("    -> \033[1;32mCalculated Physical RAM Address: 0x%016lx\033[0m\n", physical_address);
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("  CO4: Paging Architecture, Address Translation & TLB Model    \n");
    printf("===============================================================\n");
    printf("Architecture: Standard 4 KB Paging (12-bit offset, 52-bit VPN)\n");

    init_tlb();

    if (argc > 1) {
        uint64_t custom_addr = strtoull(argv[1], NULL, 0);
        translate_address(custom_addr);
    } else {
        // Run sample sequence to demonstrate both TLB Miss and TLB Hit
        int sample_var = 42;
        uint64_t sample_addr = (uint64_t)&sample_var;

        printf("Translating actual runtime stack variable address (%p):\n", (void *)&sample_var);
        translate_address(sample_addr);

        printf("\nRepeating query on same page to observe TLB Hit acceleration:\n");
        translate_address(sample_addr + 64); // Same page, different offset

        printf("\nQuerying a different heap/code page:\n");
        translate_address(0x7fff5fbff820ULL);
    }

    return 0;
}
