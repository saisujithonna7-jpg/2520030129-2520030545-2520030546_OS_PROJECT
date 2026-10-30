/*
 * ============================================================================
 * Course Outcome 1: Operating System Architecture
 * Program: user_kernel_mode.c
 * Purpose: Demonstrates User Mode (Ring 3) vs Kernel Mode (Ring 0), CPU
 *          privilege boundaries, and transition via the system call interface.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf env;

void segfault_handler(int sig) {
    printf("\033[1;31m[KERNEL TRAP]\033[0m CPU Exception 14: Page Fault / General Protection Fault caught!\n");
    printf("              Signal %d (SIGSEGV) delivered by kernel to User Process.\n", sig);
    printf("              Reason: User mode (Ring 3) cannot directly read/write kernel memory.\n");
    longjmp(env, 1);
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("   CO1: User Mode vs Kernel Mode & Trap Mechanism Demo        \n");
    printf("===============================================================\n\n");

    printf("\033[1;36m[STEP 1: User Space Execution (Ring 3)]\033[0m\n");
    printf(" Current Process ID: %d (Running in Ring 3 - User Mode)\n", getpid());
    printf(" CPU execution is constrained: cannot execute privileged instructions (cli, hlt, lgdt)\n");
    printf(" Cannot directly access physical I/O ports or kernel address space.\n\n");

    printf("\033[1;36m[STEP 2: Controlled Transition to Kernel Space (Ring 0)]\033[0m\n");
    printf(" Invoking system call 'getuid' via glibc wrapper...\n");
    uid_t uid = getuid();
    printf(" -> Returned User ID: %d\n", uid);
    printf(" Mechanism: The C library triggers a trap ('syscall' instruction in x86_64).\n");
    printf("            CPU switches Ring 3 -> Ring 0, loads kernel stack, invokes sys_getuid().\n");
    printf("            Kernel returns result in %%rax and switches back Ring 0 -> Ring 3.\n\n");

    printf("\033[1;36m[STEP 3: Raw Syscall Invocation via syscall()]\033[0m\n");
    long sys_pid = syscall(SYS_getpid);
    printf(" Directly invoking kernel system call SYS_getpid (%ld) without wrapper: %ld\n\n", (long)SYS_getpid, sys_pid);

    printf("\033[1;36m[STEP 4: Testing CPU Protection Boundary]\033[0m\n");
    printf(" Registering signal handler for SIGSEGV...\n");
    signal(SIGSEGV, segfault_handler);

    if (setjmp(env) == 0) {
        printf(" Attempting unauthorized read from higher kernel memory address (0xFFFFFFFFFF600000)...\n");
        volatile char *kernel_ptr = (volatile char *)0xFFFFFFFFFF600000;
        char val = *kernel_ptr; // Will trigger hardware trap!
        printf(" Value read: %d (Unexpected!)\n", val);
    } else {
        printf(" Successfully caught violation! Process safely recovered.\n");
    }

    printf("\n\033[1;32m[SUMMARY]\033[0m\n");
    printf(" User applications MUST use system calls as the sole gateway to OS kernel services.\n");
    printf(" Direct hardware access or memory tampering is prohibited by CPU hardware privilege levels.\n");
    return 0;
}
