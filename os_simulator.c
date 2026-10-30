/*
 * ============================================================================
 * Project: Interactive Operating System System Calls Simulator (Ubuntu Linux)
 * Language: Pure C (POSIX.1-2008 Standard)
 * Author: SaiSujith (2520030129-2520030545-2520030546)
 * Description: Master interactive terminal application for executing, tracing,
 *              and demonstrating OS concepts (CO1 to CO6) with GCC and strace.
 * ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdint.h>
#include <ctype.h>
#include <libgen.h>

/* ANSI Terminal Color Palettes */
#define C_RESET   "\033[0m"
#define C_BOLD    "\033[1m"
#define C_DIM     "\033[2m"
#define C_CYAN    "\033[1;36m"
#define C_GREEN   "\033[1;32m"
#define C_YELLOW  "\033[1;33m"
#define C_RED     "\033[1;31m"
#define C_PURPLE  "\033[1;35m"
#define C_BLUE    "\033[1;34m"
#define C_WHITE   "\033[1;37m"

/* Data Structures */
typedef struct {
    const char *q;
    const char *a;
    const char *tip;
} VivaQuestion;

typedef struct {
    int step;
    const char *actor;
    const char *action;
} FlowStep;

typedef struct {
    const char *id;
    const char *title;
    const char *file;
    const char *syscalls;
    const char *difficulty;
    const char *summary;
    const char *explanation;
    FlowStep flow[5];
    int flow_count;
    VivaQuestion viva[3];
    int viva_count;
} Demo;

typedef struct {
    const char *code;
    const char *title;
    const char *summary;
    Demo demos[5];
    int demo_count;
} Module;

/* Comprehensive Curriculum Catalog (CO1 to CO6) */
static const Module MODULES[] = {
    /* ------------------------------ CO1 ------------------------------ */
    {
        .code = "CO1",
        .title = "Operating System Architecture",
        .summary = "Kernel vs User Mode (Ring 3/0), CPU Traps, Syscall Flow, and Shell as a User Program",
        .demo_count = 3,
        .demos = {
            {
                .id = "user_kernel_mode",
                .title = "Kernel Mode vs User Mode & CPU Protection",
                .file = "programs/co1_arch/user_kernel_mode.c",
                .syscalls = "syscall(), getuid(), getpid()",
                .difficulty = "Intermediate",
                .summary = "Demonstrates CPU rings (Ring 3 vs Ring 0), hardware traps, and memory page violations.",
                .explanation = "Modern CPUs implement Dual-Mode Operation (x86 Ring 3 User vs Ring 0 Kernel). In User Mode, applications are barred from executing privileged instructions (cli, hlt, lgdt) or reading kernel memory. System calls safely trigger software traps into Ring 0 kernel entry points. Direct unauthorized memory access triggers CPU Exception 14 (Page Fault / SIGSEGV).",
                .flow_count = 4,
                .flow = {
                    {1, "User Space (Ring 3)", "Application calls getuid() glibc wrapper."},
                    {2, "CPU Trap", "CPU executes 'syscall' instruction, switching CPL=0."},
                    {3, "Kernel (Ring 0)", "sys_call_table directs call to sys_getuid() in kernel memory."},
                    {4, "Return to User", "sysret restores Ring 3 and places UID result in %rax register."}
                },
                .viva_count = 2,
                .viva = {
                    {"What is the difference between User Mode and Kernel Mode?", "User Mode (Ring 3) has restricted CPU instructions and isolated memory. Kernel Mode (Ring 0) has unrestricted hardware and RAM access.", "Mention Current Privilege Level (CPL)."},
                    {"What happens if a user process accesses kernel space directly?", "The MMU detects a page privilege violation, raises CPU Exception 14, and Linux delivers SIGSEGV terminating the process.", "Explain that user page tables flag kernel pages as supervisor-only."}
                }
            },
            {
                .id = "syscall_flow",
                .title = "Low-Level System Call Register Flow",
                .file = "programs/co1_arch/syscall_flow.c",
                .syscalls = "syscall(), write()",
                .difficulty = "Advanced",
                .summary = "Executes raw inline assembly syscall loading arguments into rax, rdi, rsi, rdx registers.",
                .explanation = "In x86-64 System V ABI, syscalls use CPU registers rather than stack memory: %rax holds syscall opcode (1 for sys_write), %rdi=fd, %rsi=buffer, %rdx=count. The 'syscall' opcode transitions CPU execution to the address in MSR_LSTAR register.",
                .flow_count = 4,
                .flow = {
                    {1, "Register Setup", "rax=1 (SYS_write), rdi=1 (stdout), rsi=buf, rdx=len."},
                    {2, "Hardware Opcode", "'syscall' machine instruction triggers CPU mode switch."},
                    {3, "Kernel Dispatcher", "entry_SYSCALL_64 saves registers on kernel stack."},
                    {4, "Return Value", "Kernel returns written byte count in %rax."}
                },
                .viva_count = 2,
                .viva = {
                    {"Which register holds the syscall opcode in 64-bit Linux?", "%rax holds the opcode (e.g. 1 for sys_write, 60 for sys_exit).", "In 32-bit Linux it was %eax with 'int 0x80'."},
                    {"What is the purpose of the System Call Interface?", "SCI provides standard POSIX C wrappers that safely transition user programs into the kernel.", "Mention glibc."}
                }
            },
            {
                .id = "mini_shell",
                .title = "Shell as a User-Space Program",
                .file = "programs/co1_arch/mini_shell.c",
                .syscalls = "fork(), execvp(), waitpid()",
                .difficulty = "Intermediate",
                .summary = "Demonstrates how command-line shells parse commands, fork children, and wait for completion.",
                .explanation = "A shell is not part of the kernel; it is a regular user-space program running in Ring 3. It runs a Read-Eval-Print Loop (REPL): reads command, forks child via fork(), replaces child memory with target binary via execvp(), and sleeps via waitpid().",
                .flow_count = 4,
                .flow = {
                    {1, "Shell Read", "Reads command string from standard input."},
                    {2, "Fork Process", "fork() creates child process with isolated address space."},
                    {3, "Execute Binary", "Child invokes execvp() to load executable into its memory."},
                    {4, "Parent Synchronization", "Parent invokes waitpid() until child terminates."}
                },
                .viva_count = 2,
                .viva = {
                    {"Is the bash shell part of the Linux kernel?", "No, bash is a regular user-space application that interfaces with the kernel through system calls.", "Shell uses fork/exec/wait."},
                    {"Which system calls are fundamental to shell command execution?", "fork() to spawn process, execvp() to load binary, waitpid() to synchronize.", "pipe() and dup2() for I/O redirection."}
                }
            }
        }
    },

    /* ------------------------------ CO2 ------------------------------ */
    {
        .code = "CO2",
        .title = "Process Management",
        .summary = "fork(), execvp(), wait(), exit(), getpid(), getppid(), and Process Trees",
        .demo_count = 5,
        .demos = {
            {
                .id = "fork_demo",
                .title = "Process Creation with fork()",
                .file = "programs/co2_process/fork_demo.c",
                .syscalls = "fork(), getpid(), getppid(), wait()",
                .difficulty = "Beginner",
                .summary = "Demonstrates fork() return values (0 in child, PID in parent) and memory address isolation.",
                .explanation = "fork() duplicates the calling process. It returns twice: 0 to child, and child's PID to parent. Memory is isolated via Copy-on-Write (COW). Variable modifications in child do not alter parent variables.",
                .flow_count = 4,
                .flow = {
                    {1, "Parent fork()", "Parent calls fork(). Kernel clones task_struct."},
                    {2, "COW Setup", "Memory pages marked Read-Only in both processes."},
                    {3, "Child Resumes", "Child receives return value 0."},
                    {4, "Parent Resumes", "Parent receives return value = Child PID."}
                },
                .viva_count = 2,
                .viva = {
                    {"Why does fork() return twice?", "fork() duplicates the CPU execution state. Both processes resume at the same instruction, but kernel sets %rax=0 for child and %rax=child_pid for parent.", "Return -1 on error."},
                    {"Do parent and child share variables after fork()?", "No, they have isolated address spaces. Changes in child are invisible to parent.", "File descriptors are shared."}
                }
            },
            {
                .id = "exec_demo",
                .title = "Program Replacement with exec()",
                .file = "programs/co2_process/exec_demo.c",
                .syscalls = "execvp(), fork(), waitpid()",
                .difficulty = "Beginner",
                .summary = "Demonstrates execvp() replacing current process code, stack, and heap with a new binary.",
                .explanation = "exec() replaces text, data, heap, and stack segments with a new ELF executable. The PID is preserved. Subsequent instructions never execute unless exec() fails.",
                .flow_count = 4,
                .flow = {
                    {1, "Child execvp()", "Child invokes execvp('/bin/ls', args)."},
                    {2, "Memory Wiped", "Kernel deallocates old address space VMAs."},
                    {3, "ELF Loaded", "Kernel maps new code and initializes stack with argv/envp."},
                    {4, "Entry Point", "Instruction pointer jumps to binary _start entry point."}
                },
                .viva_count = 2,
                .viva = {
                    {"Does exec() assign a new Process ID?", "No, PID remains identical. exec() overwrites process memory without creating a new process.", "Differentiate fork vs exec."},
                    {"When does exec() return to the caller?", "Only when it fails (returns -1 with errno like ENOENT). On success it never returns.", "Always add error checking after exec."}
                }
            },
            {
                .id = "wait_exit",
                .title = "Process Synchronization with wait() & exit()",
                .file = "programs/co2_process/wait_exit.c",
                .syscalls = "wait(), waitpid(), exit()",
                .difficulty = "Intermediate",
                .summary = "Demonstrates exit status transmission, WIFEXITED/WEXITSTATUS macros, and zombie process reaping.",
                .explanation = "When a process calls exit(status), kernel frees its memory but retains its task_struct (Zombie state) until parent reads status via wait(). wait() reaps the child PCB.",
                .flow_count = 4,
                .flow = {
                    {1, "Child exit(42)", "Child terminates; transitions to EXIT_ZOMBIE."},
                    {2, "SIGCHLD", "Kernel sends SIGCHLD signal to parent."},
                    {3, "Parent waitpid()", "Parent reaps child exit status."},
                    {4, "Cleanup", "Kernel completely deallocates child task_struct."}
                },
                .viva_count = 2,
                .viva = {
                    {"What is a Zombie Process?", "A terminated process whose exit status has not yet been read by parent via wait(). It occupies an entry in the PID table.", "Reaped with wait()."},
                    {"What is an Orphan Process?", "A running child whose parent terminated early. Adopted by PID 1 (init/systemd).", "Init reaps orphans automatically."}
                }
            },
            {
                .id = "getpid_ppid",
                .title = "Process Identification: getpid() & getppid()",
                .file = "programs/co2_process/getpid_ppid.c",
                .syscalls = "getpid(), getppid()",
                .difficulty = "Beginner",
                .summary = "Queries process identification numbers and parent hierarchy.",
                .explanation = "Linux tracks processes in kernel task_struct. getpid() returns current->pid; getppid() returns current->real_parent->pid.",
                .flow_count = 3,
                .flow = {
                    {1, "getpid()", "Kernel reads task_struct->pid."},
                    {2, "getppid()", "Kernel traverses real_parent pointer to read parent PID."},
                    {3, "Return", "Integer IDs returned to user space."}
                },
                .viva_count = 1,
                .viva = {
                    {"What is PID 1 in Linux?", "The init / systemd process booted directly by kernel. Ancestor of all user processes.", "PID 0 is the idle swapper."}
                }
            },
            {
                .id = "process_tree",
                .title = "Hierarchical Process Tree Simulation",
                .file = "programs/co2_process/process_tree.c",
                .syscalls = "fork(), waitpid(), getpid()",
                .difficulty = "Intermediate",
                .summary = "Demonstrates 3-tier Grandparent -> Parent -> Child hierarchy with multi-level reaping.",
                .explanation = "Linux processes form an acyclic directed tree rooted at PID 1. Each parent is strictly responsible for waiting on its direct children.",
                .flow_count = 3,
                .flow = {
                    {1, "Tier 1 Forks Tier 2", "Grandparent forks Parent."},
                    {2, "Tier 2 Forks Tier 3", "Parent forks Grandchild."},
                    {3, "Bottom-Up Reaping", "Grandchild reaped by Parent; Parent reaped by Grandparent."}
                },
                .viva_count = 1,
                .viva = {
                    {"Can a parent process directly wait() on its grandchild?", "No, wait() only accepts direct children. Grandparent cannot wait on grandchildren unless adoption occurs.", "POSIX hierarchy constraint."}
                }
            }
        }
    },

    /* ------------------------------ CO3 ------------------------------ */
    {
        .code = "CO3",
        .title = "Inter-Process Communication (IPC)",
        .summary = "Anonymous Pipes, Named Pipes (FIFO), Unix Domain Sockets, Signals, Shared Memory",
        .demo_count = 5,
        .demos = {
            {
                .id = "pipe_anonymous",
                .title = "Anonymous Pipes (pipe)",
                .file = "programs/co3_ipc/pipe_anonymous.c",
                .syscalls = "pipe(), read(), write(), close()",
                .difficulty = "Beginner",
                .summary = "Demonstrates unidirectional in-memory kernel ring buffer (pipefd[0]=read, pipefd[1]=write).",
                .explanation = "pipe() creates a unidirectional FIFO buffer in kernel RAM (64 KB capacity). Unused descriptors must be closed so EOF is detected when writer terminates.",
                .flow_count = 4,
                .flow = {
                    {1, "pipe(fd)", "Kernel creates in-memory pipe buffer and two descriptors."},
                    {2, "fork()", "Child inherits both open descriptors."},
                    {3, "Parent write()", "Copies message into kernel ring buffer."},
                    {4, "Child read()", "Reads message out of kernel buffer."}
                },
                .viva_count = 2,
                .viva = {
                    {"Why close unused pipe ends?", "If read end does not close its inherited write end, read() blocks forever because a write descriptor remains open.", "Closing write end sends EOF."},
                    {"Can unrelated processes communicate via anonymous pipes?", "No, anonymous pipes require common ancestry across fork(). Use Named Pipes for unrelated processes.", "FIFOs have filesystem names."}
                }
            },
            {
                .id = "fifo_named_pipe",
                .title = "Named Pipes (FIFO / mkfifo)",
                .file = "programs/co3_ipc/fifo_named_pipe.c",
                .syscalls = "mkfifo(), open(), read(), write(), unlink()",
                .difficulty = "Intermediate",
                .summary = "Demonstrates persistent filesystem FIFO special node connecting independent processes.",
                .explanation = "mkfifo() creates a special node in the directory tree (type 'p'). Data flows through kernel buffer without touching disk. Opening blocks until both reader and writer connect.",
                .flow_count = 4,
                .flow = {
                    {1, "mkfifo()", "Creates FIFO inode on filesystem."},
                    {2, "Reader open()", "open(O_RDONLY) blocks until writer opens."},
                    {3, "Writer open()", "open(O_WRONLY) connects; both unblock."},
                    {4, "Transfer & unlink", "Data transmitted; unlink() cleans up filesystem node."}
                },
                .viva_count = 1,
                .viva = {
                    {"Difference between Anonymous Pipe and FIFO?", "Anonymous pipe has no filesystem name and connects related processes. FIFO has a path (e.g. /tmp/fifo) and connects unrelated processes.", "Both use kernel RAM buffer."}
                }
            },
            {
                .id = "unix_sockets",
                .title = "Unix Domain Sockets (AF_UNIX)",
                .file = "programs/co3_ipc/unix_sockets.c",
                .syscalls = "socket(), bind(), listen(), accept(), connect()",
                .difficulty = "Advanced",
                .summary = "Full-duplex bidirectional stream sockets operating locally without network stack overhead.",
                .explanation = "AF_UNIX sockets use standard socket API but bypass TCP/IP protocol headers, checksums, and routing, communicating directly through kernel memory buffers with high throughput.",
                .flow_count = 4,
                .flow = {
                    {1, "Server bind/listen", "Server binds path and listens for incoming connections."},
                    {2, "Client connect", "Client initiates local socket connection."},
                    {3, "Accept", "Server accept() returns dedicated communication descriptor."},
                    {4, "Bidirectional I/O", "Processes send/recv messages full-duplex."}
                },
                .viva_count = 1,
                .viva = {
                    {"Why use Unix Domain Sockets over TCP loopback?", "Zero TCP/IP header, checksum, or routing overhead. Significant performance speedup and credential passing support.", "Used by Docker and X11."}
                }
            },
            {
                .id = "signals_demo",
                .title = "Asynchronous Signals (signal, kill, raise, alarm)",
                .file = "programs/co3_ipc/signals_demo.c",
                .syscalls = "signal(), kill(), raise(), alarm(), pause()",
                .difficulty = "Intermediate",
                .summary = "Software interrupts delivering asynchronous notifications and custom handlers.",
                .explanation = "Signals are software interrupts. When delivered, kernel suspends user program, pushes a stack frame for signal handler, executes handler, and resumes via sigreturn.",
                .flow_count = 4,
                .flow = {
                    {1, "Register Handler", "signal(SIGUSR1, handler) registers callback."},
                    {2, "kill(pid, SIGUSR1)", "Sender requests kernel to signal target PID."},
                    {3, "Kernel Interrupt", "Kernel interrupts target execution flow and jumps to handler."},
                    {4, "Resume", "sigreturn restores CPU state."}
                },
                .viva_count = 2,
                .viva = {
                    {"Which signals cannot be caught or ignored?", "SIGKILL (9) and SIGSTOP (19) cannot be caught, blocked, or ignored.", "Kernel handles them directly."},
                    {"What is difference between synchronous and asynchronous signals?", "Synchronous: generated by CPU instruction (SIGSEGV, SIGFPE). Asynchronous: sent externally (SIGINT, SIGALRM, kill).", "Use sig_atomic_t."}
                }
            },
            {
                .id = "shared_memory",
                .title = "Shared Memory (shmget, shmat, shmdt, shmctl)",
                .file = "programs/co3_ipc/shared_memory.c",
                .syscalls = "shmget(), shmat(), shmdt(), shmctl()",
                .difficulty = "Advanced",
                .summary = "Fastest IPC mechanism providing zero-copy direct RAM sharing between processes.",
                .explanation = "Shared memory maps the identical physical RAM frames into multiple process address spaces. After shmat(), data read/write bypasses the kernel completely with zero syscall overhead.",
                .flow_count = 4,
                .flow = {
                    {1, "shmget()", "Kernel allocates physical memory segment ID (shmid)."},
                    {2, "shmat()", "Maps segment into process page tables."},
                    {3, "Direct RAM Access", "Processes read and write directly to pointer in RAM."},
                    {4, "shmctl(IPC_RMID)", "Marks segment for deallocation when detached."}
                },
                .viva_count = 1,
                .viva = {
                    {"Why is Shared Memory faster than Pipes or Sockets?", "Pipes and sockets require 2 memory copies (user->kernel->user). Shared memory maps the same physical frame into both processes with zero copies.", "Needs synchronization primitives."}
                }
            }
        }
    },

    /* ------------------------------ CO4 ------------------------------ */
    {
        .code = "CO4",
        .title = "Virtual Memory Management",
        .summary = "Paging, Address Translation, TLB, Page Faults, Copy-on-Write, Demand Paging, mmap/munmap",
        .demo_count = 4,
        .demos = {
            {
                .id = "mmap_demo",
                .title = "Memory Mapping (mmap & munmap)",
                .file = "programs/co4_vm/mmap_demo.c",
                .syscalls = "mmap(), munmap(), msync()",
                .difficulty = "Intermediate",
                .summary = "Maps anonymous memory pages and file contents directly into user address space.",
                .explanation = "mmap() creates a Virtual Memory Area (VMA). MAP_ANONYMOUS allocates raw zeroed RAM. File mapping connects disk files to memory pointers; modifications flush via msync().",
                .flow_count = 4,
                .flow = {
                    {1, "mmap()", "Kernel creates vm_area_struct in process mm_struct."},
                    {2, "Access", "Pointer access reads data; dirty writes update page cache."},
                    {3, "msync()", "Flushes dirty pages to underlying disk storage."},
                    {4, "munmap()", "Unmaps VMA and frees page table entries."}
                },
                .viva_count = 1,
                .viva = {
                    {"Difference between MAP_SHARED and MAP_PRIVATE?", "MAP_SHARED writes modifications back to disk file and shares with others. MAP_PRIVATE uses Copy-on-Write for private isolated modifications.", "Dynamic linkers use MAP_PRIVATE."}
                }
            },
            {
                .id = "copy_on_write",
                .title = "Virtual Memory Copy-on-Write (COW)",
                .file = "programs/co4_vm/copy_on_write.c",
                .syscalls = "fork(), mmap(), munmap()",
                .difficulty = "Advanced",
                .summary = "Demonstrates frame sharing after fork, write-protection trap, and on-demand page duplication.",
                .explanation = "During fork(), Linux duplicates page tables but marks all physical pages READ-ONLY. When child writes, CPU MMU triggers Page Fault 14. Kernel allocates a new physical frame and copies 4KB data.",
                .flow_count = 4,
                .flow = {
                    {1, "fork() Read-Only", "Pages marked Read-Only in parent and child."},
                    {2, "Child Write", "Child attempts write; hardware MMU detects violation."},
                    {3, "Trap 14", "CPU raises Page Fault Exception."},
                    {4, "Frame Copy", "Kernel allocates fresh frame, copies 4KB data, sets RW permission."}
                },
                .viva_count = 1,
                .viva = {
                    {"Why is COW crucial for fork() performance?", "Most processes call exec() immediately after fork(). COW prevents copying gigabytes of RAM that would be immediately discarded.", "Makes fork() O(1) fast."}
                }
            },
            {
                .id = "paging_translation",
                .title = "Paging Architecture & Address Translation",
                .file = "programs/co4_vm/paging_translation.c",
                .syscalls = "getpid()",
                .difficulty = "Intermediate",
                .summary = "Calculates 12-bit Page Offset, Virtual Page Number (VPN), TLB lookup, and Physical Address.",
                .explanation = "In 4 KB Paging, virtual address splits into: Page Offset (lower 12 bits) and VPN (upper bits). MMU checks TLB cache (1ns). If TLB Miss, MMU walks CR3 multi-level page table (50ns) to calculate (PFN << 12) | Offset.",
                .flow_count = 4,
                .flow = {
                    {1, "Virtual Address", "Address split: Offset = Addr & 0xFFF, VPN = Addr >> 12."},
                    {2, "TLB Cache Check", "Associative hardware lookup for VPN in TLB."},
                    {3, "Page Table Walk", "On TLB Miss, hardware walks CR3 page directory."},
                    {4, "Physical Address", "Physical Address = (PFN << 12) | Offset."}
                },
                .viva_count = 2,
                .viva = {
                    {"Why is page size 4096 bytes (4 KB)?", "Optimal trade-off between internal fragmentation (wasted page bytes) and page table memory overhead.", "Huge pages are 2MB/1GB."},
                    {"What is the role of TLB?", "Translation Lookaside Buffer is high-speed CPU hardware cache storing recent VPN->PFN translations to avoid slow RAM walks.", "Hit takes 1ns; miss takes ~50ns."}
                }
            },
            {
                .id = "demand_paging",
                .title = "Demand Paging & Minor Page Faults",
                .file = "programs/co4_vm/demand_paging.c",
                .syscalls = "mmap(), getrusage()",
                .difficulty = "Intermediate",
                .summary = "Measures minor page faults (ru_minflt) to prove RAM is allocated only when touched.",
                .explanation = "Demand Paging loads pages into physical memory only when accessed. mmap allocates virtual range with 0 physical frames. Touching a page triggers minor page fault, allocating a 4KB frame on-demand.",
                .flow_count = 4,
                .flow = {
                    {1, "mmap 64MB", "Allocates virtual address space descriptor; 0 RAM frames committed."},
                    {2, "Touch Page", "Instruction accesses buffer[i*4096]."},
                    {3, "Minor Fault", "CPU fires Minor Page Fault (ru_minflt increases by 1)."},
                    {4, "Frame Committed", "Subsequent accesses to same page incur 0 new page faults."}
                },
                .viva_count = 1,
                .viva = {
                    {"Difference between Minor and Major Page Fault?", "Minor: page is in RAM (shared library or freshly allocated zeroed page), only PTE updated. Major: page must be fetched from slow disk/swap.", "Major page faults have high latency."}
                }
            }
        }
    },

    /* ------------------------------ CO5 ------------------------------ */
    {
        .code = "CO5",
        .title = "File Management",
        .summary = "open(), read(), write(), close(), lseek(), stat(), and unlink()",
        .demo_count = 4,
        .demos = {
            {
                .id = "file_crud",
                .title = "Low-Level File I/O: open, write, read, close",
                .file = "programs/co5_file/file_crud.c",
                .syscalls = "open(), write(), read(), close()",
                .difficulty = "Beginner",
                .summary = "Direct interaction with the Virtual File System (VFS) and file descriptors.",
                .explanation = "open() returns integer File Descriptor indexing process fd table. write() copies bytes into kernel page cache. read() pulls bytes from page cache. close() frees descriptor index.",
                .flow_count = 4,
                .flow = {
                    {1, "open(O_CREAT|O_WRONLY)", "Allocates file descriptor (e.g. fd=3) in files_struct."},
                    {2, "write(fd, buf, len)", "Transfers data to kernel page cache; marks inode dirty."},
                    {3, "close(fd)", "Decrements file table refcount and frees fd slot."},
                    {4, "open(O_RDONLY) & read()", "Reopens and reads data back into user buffer."}
                },
                .viva_count = 2,
                .viva = {
                    {"What are standard file descriptors 0, 1, and 2?", "0=STDIN_FILENO, 1=STDOUT_FILENO, 2=STDERR_FILENO.", "Custom files start at fd 3."},
                    {"Difference between open() and fopen()?", "open() is a direct kernel system call operating on raw descriptors without buffering. fopen() is a C stdio function using FILE* stream with user-space buffer.", "fopen buffers small writes."}
                }
            },
            {
                .id = "lseek_demo",
                .title = "Random Access File Pointer: lseek()",
                .file = "programs/co5_file/lseek_demo.c",
                .syscalls = "lseek(), open(), read(), close()",
                .difficulty = "Beginner",
                .summary = "Manipulates file offset pointers with SEEK_SET, SEEK_CUR, and SEEK_END.",
                .explanation = "Open file descriptions track current read/write offset (f_pos). lseek() repositions f_pos relative to file start (SEEK_SET), current offset (SEEK_CUR), or file end (SEEK_END).",
                .flow_count = 3,
                .flow = {
                    {1, "SEEK_SET", "Sets offset pointer relative to byte 0."},
                    {2, "SEEK_CUR", "Advances offset pointer relative to current position."},
                    {3, "SEEK_END", "Sets offset to end of file, returning file byte size."}
                },
                .viva_count = 1,
                .viva = {
                    {"Can lseek() be used on an anonymous pipe or socket?", "No, lseek() returns ESPIPE (Illegal seek) because pipes, FIFOs, and sockets are sequential streams.", "Only regular files and block devices support seek."}
                }
            },
            {
                .id = "stat_inode",
                .title = "File Metadata & Inode Query: stat() & fstat()",
                .file = "programs/co5_file/stat_inode.c",
                .syscalls = "stat(), fstat()",
                .difficulty = "Intermediate",
                .summary = "Queries Inode numbers, hard link counts, permission octals, block size, and timestamps.",
                .explanation = "In Linux filesystems, files are represented by Inodes containing metadata and block pointers. Directories simply map filenames to Inode numbers. stat() queries this metadata.",
                .flow_count = 3,
                .flow = {
                    {1, "Path Resolution", "Kernel traverses directory tree to find dentry."},
                    {2, "Inode Lookup", "Kernel reads inode from VFS inode cache/disk."},
                    {3, "struct stat", "Populates permissions, size, inode, and timestamps."}
                },
                .viva_count = 1,
                .viva = {
                    {"Does an Inode store the filename of the file?", "No. Filenames are stored inside directory data blocks mapped to Inode numbers. An Inode stores only metadata and block pointers.", "Enables hard links."}
                }
            },
            {
                .id = "unlink_demo",
                .title = "File Deletion Semantics: unlink()",
                .file = "programs/co5_file/unlink_demo.c",
                .syscalls = "unlink(), open(), read(), close()",
                .difficulty = "Intermediate",
                .summary = "Removes directory entry; proves data blocks persist while file descriptors remain open.",
                .explanation = "unlink() removes filename from directory and decrements st_nlink. If any process holds an open fd to the file, data blocks remain readable! Kernel frees blocks only when final fd closes.",
                .flow_count = 4,
                .flow = {
                    {1, "open(file)", "Process opens file descriptor (fd=3)."},
                    {2, "unlink(file)", "Directory entry removed; st_nlink drops 1->0."},
                    {3, "Read via fd", "Process reads file successfully via open descriptor."},
                    {4, "close(fd)", "Open refcount reaches 0; kernel deallocates disk blocks."}
                },
                .viva_count = 1,
                .viva = {
                    {"Why does disk space not free immediately when deleting open log files?", "Because the running process still holds an open file descriptor. Disk blocks are freed only after the process closes the fd.", "Use 'lsof | grep deleted'."}
                }
            }
        }
    },

    /* ------------------------------ CO6 ------------------------------ */
    {
        .code = "CO6",
        .title = "Thread Management & Synchronization",
        .summary = "POSIX Threads, Mutex Locks, Condition Variables, Counting Semaphores, Deadlocks",
        .demo_count = 5,
        .demos = {
            {
                .id = "pthread_basic",
                .title = "POSIX Threads Creation & Joining",
                .file = "programs/co6_threads/pthread_basic.c",
                .syscalls = "pthread_create(), pthread_join(), clone()",
                .difficulty = "Beginner",
                .summary = "Spawns 4 worker threads sharing memory space and retrieves thread return codes.",
                .explanation = "Threads share virtual memory (code, data, heap) and open file descriptors, but maintain private stacks and register states. In Linux, threads are created via clone() syscall.",
                .flow_count = 4,
                .flow = {
                    {1, "pthread_create()", "clone(CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND)."},
                    {2, "Stack Allocation", "Glibc allocates private stack memory for worker."},
                    {3, "Concurrent Execution", "Worker computes sum concurrently with main thread."},
                    {4, "pthread_join()", "Main thread blocks until worker exits and retrieves return value."}
                },
                .viva_count = 1,
                .viva = {
                    {"What is shared and private between threads of the same process?", "Shared: Code, Global Data, Heap, File Descriptors, PID. Private: Thread ID (TID), Registers (PC, SP), Stack, and errno.", "Shared memory requires synchronization."}
                }
            },
            {
                .id = "mutex_sync",
                .title = "Race Conditions vs Mutex Locks",
                .file = "programs/co6_threads/mutex_sync.c",
                .syscalls = "pthread_mutex_lock(), pthread_mutex_unlock()",
                .difficulty = "Intermediate",
                .summary = "Compares unprotected counter (lost updates from race conditions) vs atomic mutex lock.",
                .explanation = "Counter increment (counter++) is not atomic; it consists of Read, Modify, Write instructions. Race condition occurs when threads interleave. Mutex enforces Mutual Exclusion.",
                .flow_count = 4,
                .flow = {
                    {1, "Unprotected Test", "Threads interleave non-atomically, losing thousands of updates."},
                    {2, "Mutex Test", "Threads call pthread_mutex_lock() before entering Critical Section."},
                    {3, "Atomic Entry", "One thread enters; others wait on kernel futex queue."},
                    {4, "Zero Lost Updates", "Final protected counter matches exact expected total."}
                },
                .viva_count = 1,
                .viva = {
                    {"Three requirements for solving Critical Section problem?", "1. Mutual Exclusion (only one enters), 2. Progress (selection cannot be postponed), 3. Bounded Waiting (limit on times others can enter before a waiter).", "Classic Dijkstra criteria."}
                }
            },
            {
                .id = "cond_var",
                .title = "Condition Variables (Producer-Consumer)",
                .file = "programs/co6_threads/cond_var.c",
                .syscalls = "pthread_cond_wait(), pthread_cond_signal()",
                .difficulty = "Advanced",
                .summary = "Synchronizes bounded circular buffer without CPU-wasting busy waiting.",
                .explanation = "pthread_cond_wait() atomically unlocks associated mutex and puts calling thread to sleep. Signaling wakes thread, which re-acquires mutex before proceeding. Must use while loop.",
                .flow_count = 4,
                .flow = {
                    {1, "Producer Inserts", "Producer puts item into buffer and signals cond_not_empty."},
                    {2, "Consumer Wait", "If empty, consumer calls pthread_cond_wait() and sleeps."},
                    {3, "Wakeup Signal", "Producer signal wakes consumer; consumer re-acquires lock."},
                    {4, "Safe Extraction", "Consumer extracts item and signals cond_not_full."}
                },
                .viva_count = 1,
                .viva = {
                    {"Why MUST pthread_cond_wait() be inside a while loop?", "1. Spurious Wakeups (threads wake without signal), 2. Condition Change (another thread may take item before this thread re-locks mutex).", "Always use 'while (condition)'."}
                }
            },
            {
                .id = "counting_semaphore",
                .title = "Counting Semaphores (Resource Pool)",
                .file = "programs/co6_threads/counting_semaphore.c",
                .syscalls = "sem_init(), sem_wait(), sem_post(), sem_destroy()",
                .difficulty = "Intermediate",
                .summary = "Controls finite pool of 3 shared resources among 6 competing threads.",
                .explanation = "Semaphore is an integer counter with atomic operations: sem_wait() (P operation) decrements counter (blocks if 0). sem_post() (V operation) increments counter and wakes waiting thread.",
                .flow_count = 4,
                .flow = {
                    {1, "sem_init(pool, 3)", "Initializes semaphore counter to 3."},
                    {2, "First 3 Threads", "Call sem_wait(); counter drops 2, 1, 0; all three enter."},
                    {3, "Remaining 3 Threads", "Call sem_wait(); counter=0; threads block on waitqueue."},
                    {4, "sem_post()", "Exiting thread increments counter, waking one waiting thread."}
                },
                .viva_count = 1,
                .viva = {
                    {"Difference between Binary Semaphore and Mutex?", "Mutex has ownership (only locking thread can unlock). Binary Semaphore has no ownership; one thread can wait (P) and another post (V).", "Mutex supports priority inheritance."}
                }
            },
            {
                .id = "deadlock_simulation",
                .title = "Safe Deadlock Detection & Prevention",
                .file = "programs/co6_threads/deadlock_simulation.c",
                .syscalls = "pthread_mutex_timedlock(), pthread_mutex_lock()",
                .difficulty = "Advanced",
                .summary = "Simulates Circular Wait deadlock safely with timedlock; proves Lock Ordering prevention.",
                .explanation = "Deadlock requires 4 Coffman conditions: Mutual Exclusion, Hold & Wait, No Preemption, Circular Wait. Enforcing global lock ordering (acquire Lock A before Lock B) breaks Circular Wait.",
                .flow_count = 4,
                .flow = {
                    {1, "Thread 1 acquires A", "Holds Lock A; attempts to acquire Lock B."},
                    {2, "Thread 2 acquires B", "Holds Lock B; attempts to acquire Lock A."},
                    {3, "Timedlock Safe Catch", "pthread_mutex_timedlock safely times out, proving circular wait."},
                    {4, "Prevention", "Enforcing global lock order (A then B) guarantees deadlock freedom."}
                },
                .viva_count = 2,
                .viva = {
                    {"What are the 4 Coffman conditions for deadlock?", "1. Mutual Exclusion, 2. Hold and Wait, 3. No Preemption, 4. Circular Wait.", "Remember acronym MHNC."},
                    {"How does Lock Ordering prevent deadlocks?", "Assign linear priority (1..N) to all locks and require acquiring in strictly increasing order. Makes circular wait cycles impossible.", "Breaks 4th condition."}
                }
            }
        }
    }
};

static const int MODULE_COUNT = sizeof(MODULES) / sizeof(MODULES[0]);

/* Terminal UI Helpers */
void clear_screen(void) {
    printf("\033[2J\033[H");
}

void print_banner(void) {
    clear_screen();
    printf("%s╔══════════════════════════════════════════════════════════════════════════════╗%s\n", C_CYAN, C_RESET);
    printf("%s║%s%s             INTERACTIVE OPERATING SYSTEM SYSTEM CALLS SIMULATOR              %s%s║%s\n", C_CYAN, C_BOLD, C_WHITE, C_RESET, C_CYAN, C_RESET);
    printf("%s║%s%s           Pure Ubuntu Linux Terminal Edition • GCC & strace Engine           %s%s║%s\n", C_CYAN, C_YELLOW, C_BOLD, C_RESET, C_CYAN, C_RESET);
    printf("%s║%s%s             Curriculum Aligned: Course Outcomes CO1 through CO6              %s%s║%s\n", C_CYAN, C_DIM, C_WHITE, C_RESET, C_CYAN, C_RESET);
    printf("%s╚══════════════════════════════════════════════════════════════════════════════╝%s\n", C_CYAN, C_RESET);
}

void print_system_info(void) {
    char hostname[64] = "Ubuntu";
    gethostname(hostname, sizeof(hostname));
    printf("%s[HOST]%s Linux (%s) | %s[COMPILER]%s GCC (Native Ubuntu) | %s[MODE]%s 100%% Pure C\n",
           C_GREEN, C_RESET, hostname, C_CYAN, C_RESET, C_YELLOW, C_RESET);
    printf("%s═%s\n", C_CYAN, "═════════════════════════════════════════════════════════════════════");
}

void safe_read_line(char *buf, size_t size) {
    if (fgets(buf, size, stdin) == NULL) {
        printf("\n%sSession closed. Exiting...%s\n", C_YELLOW, C_RESET);
        exit(0);
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
}

void pause_enter(void) {
    char buf[16];
    printf("\n%sPress Enter to continue...%s", C_DIM, C_RESET);
    safe_read_line(buf, sizeof(buf));
}

/* C Source Code Viewer */
void view_c_source(const char *rel_path) {
    FILE *fp = fopen(rel_path, "r");
    if (!fp) {
        printf("%s[ERROR] Could not open file: %s%s\n", C_RED, rel_path, C_RESET);
        return;
    }

    printf("\n%s=== C SOURCE CODE: %s ===%s\n\n", C_CYAN, rel_path, C_RESET);
    char line[512];
    int line_num = 1;

    while (fgets(line, sizeof(line), fp)) {
        char *trimmed = line;
        while (isspace((unsigned char)*trimmed)) trimmed++;

        if (trimmed[0] == '/' || trimmed[0] == '*') {
            printf("%s%4d | %s%s%s", C_DIM, line_num++, C_GREEN, line, C_RESET);
        } else if (trimmed[0] == '#') {
            printf("%s%4d | %s%s%s", C_DIM, line_num++, C_PURPLE, line, C_RESET);
        } else {
            printf("%s%4d | %s%s%s", C_DIM, line_num++, C_WHITE, line, C_RESET);
        }
    }

    fclose(fp);
    printf("%s══════════════════════════════════════════════════════════════════════%s\n", C_CYAN, C_RESET);
}

/* Native GCC Compilation and Execution */
void compile_and_run(const char *rel_path, const char *extra_args, int use_strace) {
    char temp_bin[128];
    snprintf(temp_bin, sizeof(temp_bin), "/tmp/os_c_bin_%d_%ld", getpid(), time(NULL));

    printf("\n%s[1/2] Compiling with Ubuntu GCC:%s %sgcc -Wall -Wextra -pthread '%s' -o '%s'%s\n",
           C_CYAN, C_RESET, C_WHITE, rel_path, temp_bin, C_RESET);

    char compile_cmd[512];
    snprintf(compile_cmd, sizeof(compile_cmd), "gcc -Wall -Wextra -pthread '%s' -o '%s'", rel_path, temp_bin);

    int comp_status = system(compile_cmd);
    if (comp_status != 0) {
        printf("%s[ERROR] GCC compilation failed. Return code: %d%s\n", C_RED, comp_status, C_RESET);
        return;
    }

    printf("%s[SUCCESS] GCC compilation succeeded with 0 errors.%s\n", C_GREEN, C_RESET);

    char exec_cmd[1024];
    if (use_strace) {
        printf("%s[2/2] Executing with kernel strace audit:%s %s %s\n\n", C_PURPLE, C_RESET, temp_bin, extra_args ? extra_args : "");
        snprintf(exec_cmd, sizeof(exec_cmd), "strace -f -s 80 -e trace=process,ipc,desc,memory,signal '%s' %s", temp_bin, extra_args ? extra_args : "");
    } else {
        printf("%s[2/2] Executing natively on Ubuntu Linux:%s %s %s\n\n", C_GREEN, C_RESET, temp_bin, extra_args ? extra_args : "");
        snprintf(exec_cmd, sizeof(exec_cmd), "'%s' %s", temp_bin, extra_args ? extra_args : "");
    }

    printf("%s──────────────────────────────────────────────────────────────────────%s\n", C_DIM, C_RESET);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int exec_status = system(exec_cmd);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("%s──────────────────────────────────────────────────────────────────────%s\n", C_DIM, C_RESET);

    int exit_code = WEXITSTATUS(exec_status);
    const char *color = (exit_code == 0) ? C_GREEN : C_RED;
    printf("%sExecution Summary:%s Return Code: %s%d%s | Duration: %s%.2f ms%s\n",
           C_BOLD, C_RESET, color, exit_code, C_RESET, C_CYAN, elapsed_ms, C_RESET);

    /* Clean up temporary binary */
    unlink(temp_bin);
}

/* Individual Demonstration Submenu */
void demo_actions_menu(const Demo *d, const Module *m) {
    char input_buf[128];

    while (1) {
        print_banner();
        printf("%sModule:%s %s — %s\n", C_BOLD, m->code, m->title, C_RESET);
        printf("%sDemonstration:%s %s [%s]%s\n", C_BOLD, C_CYAN, d->title, d->difficulty, C_RESET);
        printf("%sSystem Calls:%s  %s\n", C_BOLD, d->syscalls, C_RESET);
        printf("%sSource File:%s   %s\n", C_BOLD, d->file, C_RESET);
        printf("\n%s%s%s\n\n", C_WHITE, d->summary, C_RESET);

        printf("%s┌──────────────────────────────────────────────────────────┐%s\n", C_CYAN, C_RESET);
        printf("│  %s[1] ▶ Compile & Run natively on Ubuntu with GCC%s        │\n", C_BOLD, C_RESET);
        printf("│  %s[2] ⚡ Trace Kernel Traps with strace (Audit Syscalls)%s │\n", C_BOLD, C_RESET);
        printf("│  %s[3] ⚙ Run with Custom Command-Line Arguments (argv)%s   │\n", C_BOLD, C_RESET);
        printf("│  %s[4] 📄 View C Source Code with Syntax Highlighting%s     │\n", C_BOLD, C_RESET);
        printf("│  %s[5] 🧠 Kernel Deep-Dive: Internals & Data Structures%s   │\n", C_BOLD, C_RESET);
        printf("│  %s[6] 🔄 Step-by-Step Execution Flow Sequence%s            │\n", C_BOLD, C_RESET);
        printf("│  %s[7] ❓ Viva Questions & Answers for this Unit%s          │\n", C_BOLD, C_RESET);
        printf("│  %s[0] ⬅ Back to Module Demonstrations%s                    │\n", C_BOLD, C_RESET);
        printf("%s└──────────────────────────────────────────────────────────┘%s\n", C_CYAN, C_RESET);

        printf("\n%sSelect Action [0-7]: %s", C_BOLD, C_RESET);
        safe_read_line(input_buf, sizeof(input_buf));

        if (strcmp(input_buf, "1") == 0) {
            compile_and_run(d->file, "", 0);
            pause_enter();
        } else if (strcmp(input_buf, "2") == 0) {
            compile_and_run(d->file, "", 1);
            pause_enter();
        } else if (strcmp(input_buf, "3") == 0) {
            char custom_args[256];
            printf("%sEnter arguments to pass to program: %s", C_BOLD, C_RESET);
            safe_read_line(custom_args, sizeof(custom_args));
            compile_and_run(d->file, custom_args, 0);
            pause_enter();
        } else if (strcmp(input_buf, "4") == 0) {
            view_c_source(d->file);
            pause_enter();
        } else if (strcmp(input_buf, "5") == 0) {
            printf("\n%s=== KERNEL INTERNAL EXPLANATION & ARCHITECTURE ===%s\n\n", C_CYAN, C_RESET);
            printf("%s\n\n", d->explanation);
            printf("%s──────────────────────────────────────────────────────────────────────%s\n", C_CYAN, C_RESET);
            pause_enter();
        } else if (strcmp(input_buf, "6") == 0) {
            printf("\n%s=== STEP-BY-STEP KERNEL EXECUTION FLOW ===%s\n\n", C_CYAN, C_RESET);
            for (int i = 0; i < d->flow_count; i++) {
                const char *actor_color = strcasestr(d->flow[i].actor, "kernel") || strcasestr(d->flow[i].actor, "trap") ? C_PURPLE : C_CYAN;
                printf(" [%d] %s%s:%s %s\n", d->flow[i].step, actor_color, d->flow[i].actor, C_RESET, d->flow[i].action);
            }
            printf("\n%s──────────────────────────────────────────────────────────────────────%s\n", C_CYAN, C_RESET);
            pause_enter();
        } else if (strcmp(input_buf, "7") == 0) {
            printf("\n%s=== VIVA QUESTIONS FOR %s ===%s\n\n", C_CYAN, d->title, C_RESET);
            for (int i = 0; i < d->viva_count; i++) {
                printf("%sQ%d: %s%s\n", C_BOLD, i + 1, d->viva[i].q, C_RESET);
                printf("%sAnswer:%s %s\n", C_GREEN, C_RESET, d->viva[i].a);
                if (d->viva[i].tip) {
                    printf("%s💡 Viva Tip:%s %s\n", C_YELLOW, C_RESET, d->viva[i].tip);
                }
                printf("%s────────────────────────────────────────────────────────────%s\n", C_DIM, C_RESET);
            }
            pause_enter();
        } else if (strcmp(input_buf, "0") == 0) {
            break;
        }
    }
}

/* Module Submenu */
void module_menu(const Module *m) {
    char input_buf[64];

    while (1) {
        print_banner();
        printf("%sCourse Outcome: %s — %s%s\n", C_BOLD, m->code, m->title, C_RESET);
        printf("%s%s%s\n\n", C_DIM, m->summary, C_RESET);

        printf("%sDemonstrations in this Module:%s\n", C_BOLD, C_RESET);
        for (int i = 0; i < m->demo_count; i++) {
            const char *diff_color = strcmp(m->demos[i].difficulty, "Beginner") == 0 ? C_GREEN :
                                     strcmp(m->demos[i].difficulty, "Intermediate") == 0 ? C_YELLOW : C_RED;
            printf(" [%d] %s%-40s%s %s[%s]%s %s(%s)%s\n",
                   i + 1, C_BOLD, m->demos[i].title, C_RESET,
                   diff_color, m->demos[i].difficulty, C_RESET,
                   C_DIM, m->demos[i].syscalls, C_RESET);
        }
        printf(" [0] Return to Main Menu\n");

        printf("\n%sSelect Demonstration [1-%d] or 0: %s", C_BOLD, m->demo_count, C_RESET);
        safe_read_line(input_buf, sizeof(input_buf));

        if (strcmp(input_buf, "0") == 0) {
            break;
        }

        int choice = atoi(input_buf);
        if (choice >= 1 && choice <= m->demo_count) {
            demo_actions_menu(&m->demos[choice - 1], m);
        } else {
            printf("%sInvalid choice.%s\n", C_RED, C_RESET);
            sleep(1);
        }
    }
}

/* Interactive 4 KB Virtual Memory Address Translation Sandbox */
void run_address_translation_engine(void) {
    char input_buf[128];

    print_banner();
    printf("\n%s%s─── INTERACTIVE VIRTUAL MEMORY ADDRESS TRANSLATION SANDBOX ───%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("Architecture: Standard 4 KB Paging (12-bit Page Offset, 52-bit VPN)\n");
    printf("Formula: Physical Address = (Physical Frame Number << 12) | Offset\n\n");

    while (1) {
        printf("%sEnter Virtual Address (Hex like '0x7ffd1234abcd' or Dec, or 'b' for back): %s", C_BOLD, C_RESET);
        safe_read_line(input_buf, sizeof(input_buf));

        if (strcmp(input_buf, "b") == 0 || strcmp(input_buf, "B") == 0) {
            break;
        }

        if (strlen(input_buf) == 0) {
            strcpy(input_buf, "0x7ffd1234abcd");
        }

        uint64_t addr = strtoull(input_buf, NULL, 0);
        uint64_t offset = addr & 0xFFFULL;
        uint64_t vpn = addr >> 12;
        uint64_t pfn = (vpn * 13ULL + 0x2A1ULL) & 0xFFFFFFULL;
        uint64_t phys = (pfn << 12) | offset;

        printf("%s┌────────────────────────────────────────────────────────────┐%s\n", C_CYAN, C_RESET);
        printf("│ %sInput Virtual Address:%s      %s0x%012lx%s (%lu)\n", C_BOLD, C_RESET, C_WHITE, addr, C_RESET, addr);
        printf("│ %sVirtual Page Number (VPN):%s  %s0x%lx%s (Page Table Index)\n", C_CYAN, C_RESET, C_CYAN, vpn, C_RESET);
        printf("│ %sPage Offset (12 bits):%s      %s0x%03lx%s (%lu bytes in 4KB frame)\n", C_PURPLE, C_RESET, C_PURPLE, offset, C_RESET, offset);
        printf("│ %sSimulated PFN (Frame):%s      %s0x%lx%s\n", C_YELLOW, C_RESET, C_YELLOW, pfn, C_RESET);
        printf("│ %sCalculated Physical RAM:%s    %s0x%012lx%s\n", C_GREEN, C_RESET, C_GREEN, phys, C_RESET);
        printf("%s└────────────────────────────────────────────────────────────┘%s\n\n", C_CYAN, C_RESET);
    }
}

/* POSIX System Call Explorer */
void search_syscall_explorer(void) {
    char query[64];

    print_banner();
    printf("\n%s%s─── POSIX SYSTEM CALL EXPLORER DIRECTORY ───%s\n\n", C_BOLD, C_CYAN, C_RESET);
    printf("%sEnter system call name or keyword (e.g. fork, mmap, pipe, stat, CO2): %s", C_BOLD, C_RESET);
    safe_read_line(query, sizeof(query));

    printf("\n%s%-16s %-6s %-38s %s%s\n", C_BOLD, "System Call", "CO", "Demonstration Title", "Source File", C_RESET);
    printf("%s──────────────────────────────────────────────────────────────────────────────%s\n", C_CYAN, C_RESET);

    int count = 0;
    for (int m = 0; m < MODULE_COUNT; m++) {
        for (int d = 0; d < MODULES[m].demo_count; d++) {
            if (strlen(query) == 0 ||
                strcasestr(MODULES[m].demos[d].syscalls, query) ||
                strcasestr(MODULES[m].code, query) ||
                strcasestr(MODULES[m].demos[d].title, query)) {
                printf("%s%-16s%s %s%-6s%s %s%-38s%s %s%s%s\n",
                       C_GREEN, MODULES[m].demos[d].syscalls, C_RESET,
                       C_YELLOW, MODULES[m].code, C_RESET,
                       C_WHITE, MODULES[m].demos[d].title, C_RESET,
                       C_DIM, MODULES[m].demos[d].file, C_RESET);
                count++;
            }
        }
    }

    if (count == 0) {
        printf("%sNo matching system calls found for '%s'.%s\n", C_RED, query, C_RESET);
    } else {
        printf("\n%sFound %d matching entries.%s\n", C_CYAN, count, C_RESET);
    }

    pause_enter();
}

/* Viva Exam Bank */
void show_viva_exam_bank(void) {
    char input_buf[32];

    print_banner();
    printf("\n%s%s─── OPERATING SYSTEMS VIVA EXAMINATION QUESTION BANK ───%s\n", C_BOLD, C_CYAN, C_RESET);
    printf("Select a Course Outcome module, or 'A' to browse all questions:\n\n");

    for (int i = 0; i < MODULE_COUNT; i++) {
        printf(" [%d] %s: %s\n", i + 1, MODULES[i].code, MODULES[i].title);
    }
    printf(" [A] Browse All Course Outcome Viva Questions\n");
    printf(" [0] Return to Main Menu\n");

    printf("\n%sSelect Option: %s", C_BOLD, C_RESET);
    safe_read_line(input_buf, sizeof(input_buf));

    if (strcmp(input_buf, "0") == 0) {
        return;
    }

    int start_m = 0, end_m = MODULE_COUNT;
    if (strcasecmp(input_buf, "A") != 0) {
        int idx = atoi(input_buf) - 1;
        if (idx >= 0 && idx < MODULE_COUNT) {
            start_m = idx;
            end_m = idx + 1;
        } else {
            printf("%sInvalid choice.%s\n", C_RED, C_RESET);
            sleep(1);
            return;
        }
    }

    int q_num = 1;
    char temp_buf[16];
    for (int m = start_m; m < end_m; m++) {
        printf("\n%s══════════ %s: %s ══════════%s\n", C_PURPLE, MODULES[m].code, MODULES[m].title, C_RESET);
        for (int d = 0; d < MODULES[m].demo_count; d++) {
            for (int v = 0; v < MODULES[m].demos[d].viva_count; v++) {
                printf("\n%sQ%d [%s]: %s%s\n", C_CYAN, q_num++, MODULES[m].demos[d].title, MODULES[m].demos[d].viva[v].q, C_RESET);
                printf("%s(Press Enter to reveal examiner answer...)%s", C_DIM, C_RESET);
                safe_read_line(temp_buf, sizeof(temp_buf));
                printf("%sAnswer:%s %s\n", C_GREEN, C_RESET, MODULES[m].demos[d].viva[v].a);
                if (MODULES[m].demos[d].viva[v].tip) {
                    printf("%s💡 Viva Tip:%s %s\n", C_YELLOW, C_RESET, MODULES[m].demos[d].viva[v].tip);
                }
                printf("%s────────────────────────────────────────────────────────────%s\n", C_DIM, C_RESET);
            }
        }
    }

    pause_enter();
}

/* Master Menu */
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    /* Automatically ensure working directory is where os_simulator lives */
    char exe_path[512] = {0};
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        char *dir = dirname(exe_path);
        if (dir) {
            if (chdir(dir) != 0) {
                // Keep existing cwd
            }
        }
    }

    char input_buf[64];

    while (1) {
        print_banner();
        print_system_info();
        printf("%sSelect Course Outcome Module or Lab Tool:%s\n\n", C_BOLD, C_RESET);

        printf("  %s[1]%s %sCO1: Operating System Architecture%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %sKernel vs User Mode (Ring 3/0), Syscall Flow, Shell Architecture%s\n", C_DIM, C_RESET);

        printf("  %s[2]%s %sCO2: Process Management%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %sfork(), execvp(), wait(), exit(), getpid(), getppid(), Process Trees%s\n", C_DIM, C_RESET);

        printf("  %s[3]%s %sCO3: Inter-Process Communication (IPC)%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %sAnonymous Pipes, Named Pipes (FIFO), Unix Sockets, Signals, Shared Memory%s\n", C_DIM, C_RESET);

        printf("  %s[4]%s %sCO4: Virtual Memory Management%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %sPaging, Address Translation, TLB, Page Faults, COW, mmap/munmap%s\n", C_DIM, C_RESET);

        printf("  %s[5]%s %sCO5: File Management%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %sopen(), read(), write(), close(), lseek(), stat(), unlink()%s\n", C_DIM, C_RESET);

        printf("  %s[6]%s %sCO6: Thread Management & Synchronization%s\n", C_CYAN, C_RESET, C_BOLD, C_RESET);
        printf("      %spthreads, Mutex Locks, Condition Variables, Semaphores, Deadlocks%s\n", C_DIM, C_RESET);

        printf("  %s──────────────────────────────────────────────────────────────────%s\n", C_DIM, C_RESET);
        printf("  %s[7]%s %sPOSIX System Call Explorer (Search & Reference)%s\n", C_YELLOW, C_RESET, C_BOLD, C_RESET);
        printf("  %s[8]%s %sInteractive 4 KB Virtual Memory Address Translation Sandbox%s\n", C_YELLOW, C_RESET, C_BOLD, C_RESET);
        printf("  %s[9]%s %sOperating Systems Viva Examination Question Bank & Quiz%s\n", C_YELLOW, C_RESET, C_BOLD, C_RESET);
        printf("  %s[0]%s %sExit Simulator%s\n", C_RED, C_RESET, C_BOLD, C_RESET);

        printf("\n%sEnter choice [0-9]: %s", C_BOLD, C_RESET);
        safe_read_line(input_buf, sizeof(input_buf));

        if (strcmp(input_buf, "1") == 0) module_menu(&MODULES[0]);
        else if (strcmp(input_buf, "2") == 0) module_menu(&MODULES[1]);
        else if (strcmp(input_buf, "3") == 0) module_menu(&MODULES[2]);
        else if (strcmp(input_buf, "4") == 0) module_menu(&MODULES[3]);
        else if (strcmp(input_buf, "5") == 0) module_menu(&MODULES[4]);
        else if (strcmp(input_buf, "6") == 0) module_menu(&MODULES[5]);
        else if (strcmp(input_buf, "7") == 0) search_syscall_explorer();
        else if (strcmp(input_buf, "8") == 0) run_address_translation_engine();
        else if (strcmp(input_buf, "9") == 0) show_viva_exam_bank();
        else if (strcmp(input_buf, "0") == 0) {
            printf("\n%sExiting OS Simulator. Happy Learning!%s\n\n", C_GREEN, C_RESET);
            break;
        } else {
            printf("%sInvalid choice. Please enter a number [0-9].%s\n", C_RED, C_RESET);
            sleep(1);
        }
    }

    return 0;
}
