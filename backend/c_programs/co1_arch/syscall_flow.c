/*
 * ============================================================================
 * Course Outcome 1: Operating System Architecture
 * Program: syscall_flow.c
 * Purpose: Demonstrates System Call Execution Flow: from user space registers,
 *          through the 'syscall' machine instruction, to kernel dispatcher.
 * ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("    CO1: System Call Execution Flow & Register Dispatch       \n");
    printf("===============================================================\n\n");

    const char *msg = ">>> [DEMO] Message written via raw x86_64 inline assembly syscall! <<<\n";
    size_t len = strlen(msg);

    printf("\033[1;33m[Execution Path Step-by-Step]\033[0m\n");
    printf(" 1. User Application prepares parameters for 'write' system call:\n");
    printf("    - Syscall Number: %ld (__NR_write)\n", (long)SYS_write);
    printf("    - Argument 1 (File Descriptor): 1 (STDOUT_FILENO)\n");
    printf("    - Argument 2 (Buffer Pointer):  %p\n", (void *)msg);
    printf("    - Argument 3 (Byte Count):      %zu bytes\n\n", len);

    printf(" 2. Loading into x86-64 hardware registers according to Linux ABI:\n");
    printf("    - rax = %ld (Syscall opcode)\n", (long)SYS_write);
    printf("    - rdi = 1   (Arg 1: fd)\n");
    printf("    - rsi = %p  (Arg 2: buf)\n", (void *)msg);
    printf("    - rdx = %zu (Arg 3: count)\n\n", len);

    printf(" 3. Triggering CPU instruction: 'syscall'\n");
    printf("    - CPU saves RIP to RCX, RFLAGS to R11\n");
    printf("    - CPU loads Kernel entry point from MSR_LSTAR into RIP\n");
    printf("    - Switches CPL (Current Privilege Level) from 3 to 0\n\n");

    long ret;
#if defined(__x86_64__)
    __asm__ volatile (
        "movq %1, %%rax\n\t"  // Syscall number SYS_write (1)
        "movq %2, %%rdi\n\t"  // fd = 1 (stdout)
        "movq %3, %%rsi\n\t"  // buffer pointer
        "movq %4, %%rdx\n\t"  // buffer length
        "syscall\n\t"         // Trap into Linux Kernel Ring 0
        "movq %%rax, %0\n\t"  // Store return value
        : "=r" (ret)
        : "i" (SYS_write), "r" ((long)1), "r" (msg), "r" ((long)len)
        : "%rax", "%rdi", "%rsi", "%rdx", "%rcx", "%r11", "memory"
    );
#else
    ret = syscall(SYS_write, 1, msg, len);
#endif

    printf(" 4. Kernel return value stored in rax: %ld bytes written\n\n", ret);
    printf("\033[1;32m[RESULT]\033[0m Raw assembly syscall succeeded! Bytes returned: %ld\n", ret);
    return 0;
}
