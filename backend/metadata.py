"""
metadata.py
Central catalog for all 6 Course Outcome (CO) modules, their C demonstrations,
system call signatures, kernel internal explanations, execution flow graphs,
and viva questions & answers.
"""

MODULES_DATA = [
    {
        "id": "co1",
        "code": "CO1",
        "title": "Operating System Architecture",
        "badge": "Architecture & Flow",
        "icon": "cpu",
        "summary": "Explains Kernel Mode vs User Mode, CPU rings, System Call Interface (SCI), and Shell execution flow.",
        "demos": [
            {
                "id": "user_kernel_mode",
                "title": "Kernel Mode vs User Mode & CPU Rings",
                "file": "co1_arch/user_kernel_mode.c",
                "syscalls": ["syscall", "getuid", "getpid"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates hardware privilege levels (Ring 3 vs Ring 0), CPU page protection, and traps.",
                "explanation": (
                    "Modern CPUs implement Dual-Mode Operation (x86 Ring 3 User Mode and Ring 0 Kernel Mode). "
                    "In User Mode, applications are restricted from executing privileged machine instructions (like cli, hlt, lgdt) "
                    "or accessing kernel memory space. When an application needs hardware resources (disk, network, memory), "
                    "it executes a software interrupt or the 'syscall' machine instruction. This causes an atomic CPU state switch "
                    "from Ring 3 to Ring 0, jumping to the Kernel Entry Point in the Interrupt Descriptor Table (IDT). "
                    "Direct unauthorized memory access triggers CPU Exception 14 (Page Fault / SIGSEGV)."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "User Space (Ring 3)", "action": "Application invokes getuid() glibc wrapper."},
                    {"step": 2, "actor": "CPU Transition", "action": "CPU executes 'syscall' instruction, switches CPL=0."},
                    {"step": 3, "actor": "Kernel Space (Ring 0)", "action": "sys_call_table directs call to sys_getuid()."},
                    {"step": 4, "actor": "Task Struct", "action": "Kernel reads current->cred->uid from task_struct."},
                    {"step": 5, "actor": "Return to User", "action": "sysret instruction restores Ring 3 and places UID in %rax."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the primary difference between User Mode and Kernel Mode?",
                        "a": "User Mode (Ring 3) has restricted CPU instruction execution and isolated memory to prevent system crashes. Kernel Mode (Ring 0) has unrestricted access to hardware, all CPU instructions, and all physical memory.",
                        "tip": "Mention CPU Current Privilege Level (CPL) and protection rings."
                    },
                    {
                        "q": "How does the CPU transition from User Mode to Kernel Mode?",
                        "a": "Through hardware interrupts, software interrupts (e.g. int 0x80), or specialized fast system call instructions ('syscall' on x86_64, 'sysenter' on x86_32, 'svc' on ARM).",
                        "tip": "Highlight that this transition is hardware-enforced."
                    },
                    {
                        "q": "What happens if a user program attempts to access kernel memory directly?",
                        "a": "The Memory Management Unit (MMU) detects a page privilege violation, CPU raises Exception 14 (Page Fault), and Linux kernel delivers SIGSEGV (Segmentation Fault) terminating the process.",
                        "tip": "Explain that user page tables mark kernel pages with Supervisor-only flags."
                    }
                ]
            },
            {
                "id": "syscall_flow",
                "title": "Low-Level System Call Execution Flow",
                "file": "co1_arch/syscall_flow.c",
                "syscalls": ["syscall", "write"],
                "difficulty": "Advanced",
                "summary": "Executes raw inline assembly syscall loading arguments into rax, rdi, rsi, rdx registers.",
                "explanation": (
                    "In x86-64 Linux System V ABI, system calls use register-based parameter passing rather than stack passing. "
                    "The system call number is loaded into %rax (e.g. 1 for sys_write). "
                    "Arguments are passed in: %rdi (arg1), %rsi (arg2), %rdx (arg3), %r10 (arg4), %r8 (arg5), and %r9 (arg6). "
                    "Executing the 'syscall' opcode triggers the CPU hardware transition to the address stored in the MSR_LSTAR register. "
                    "The kernel executes the service routine and places the return code back into %rax."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Register Setup", "action": "rax = 1 (SYS_write), rdi = 1 (stdout), rsi = buffer, rdx = len."},
                    {"step": 2, "actor": "Hardware Trap", "action": "syscall opcode triggers Model-Specific Register LSTAR."},
                    {"step": 3, "actor": "Kernel Dispatcher", "action": "entry_SYSCALL_64 saves user registers on kernel stack."},
                    {"step": 4, "actor": "VFS Layer", "action": "Kernel calls ksys_write() and routes to tty/console driver."},
                    {"step": 5, "actor": "User Return", "action": "Kernel returns number of bytes written in %rax."}
                ],
                "viva_questions": [
                    {
                        "q": "Which register holds the system call number in 64-bit Linux?",
                        "a": "The %rax register holds the system call opcode (e.g. 1 for sys_write, 0 for sys_read, 60 for sys_exit).",
                        "tip": "Point out that 32-bit Linux historically used %eax with 'int 0x80'."
                    },
                    {
                        "q": "What is the role of the System Call Interface (SCI)?",
                        "a": "The SCI sits between user applications and the kernel, providing standard POSIX C functions that safely convert API calls into CPU hardware trap instructions.",
                        "tip": "Mention glibc as the primary provider of the SCI wrappers."
                    }
                ]
            },
            {
                "id": "mini_shell",
                "title": "Shell as a User-Space Program",
                "file": "co1_arch/mini_shell.c",
                "syscalls": ["fork", "execvp", "waitpid"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates how command-line shells parse input, fork child processes, and wait for completion.",
                "explanation": (
                    "A shell (like bash, zsh, or sh) is not part of the kernel; it is a regular user-space program running in Ring 3. "
                    "It implements the Read-Eval-Print Loop (REPL). When a user enters a command (e.g. 'ls' or 'uname'), "
                    "the shell tokenizes the string, forks a child process using fork(), replaces the child's address space with "
                    "the target binary using execvp(), and puts the parent shell to sleep using waitpid() until the command finishes."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Shell Read", "action": "Shell reads command line string from stdin."},
                    {"step": 2, "actor": "Tokenization", "action": "String parsed into argv array ['uname', '-r', NULL]."},
                    {"step": 3, "actor": "fork()", "action": "Shell clones itself; kernel creates child PCB."},
                    {"step": 4, "actor": "execvp() in Child", "action": "Child binary loaded from /usr/bin/uname."},
                    {"step": 5, "actor": "waitpid() in Parent", "action": "Parent sleeps until child emits SIGCHLD and exits."}
                ],
                "viva_questions": [
                    {
                        "q": "Is the bash shell part of the Linux kernel?",
                        "a": "No, the shell is entirely a user-space utility that acts as a command interpreter and interface to the kernel via system calls.",
                        "tip": "Clarify that the shell communicates with the kernel exclusively through system calls."
                    },
                    {
                        "q": "What system calls are essential for building a basic shell?",
                        "a": "fork() (to spawn process), execvp() (to execute binary), and waitpid() (to synchronize parent with child completion).",
                        "tip": "Mention pipe() and dup2() for I/O redirection."
                    }
                ]
            }
        ]
    },
    {
        "id": "co2",
        "code": "CO2",
        "title": "Process Management",
        "badge": "Processes & Lifecycle",
        "icon": "layers",
        "summary": "Demonstrates fork(), exec(), wait(), exit(), getpid(), getppid(), and hierarchical process trees.",
        "demos": [
            {
                "id": "fork_demo",
                "title": "Process Creation with fork()",
                "file": "co2_process/fork_demo.c",
                "syscalls": ["fork", "getpid", "getppid", "wait"],
                "difficulty": "Beginner",
                "summary": "Demonstrates fork() return values (0 for child, PID for parent) and memory address isolation.",
                "explanation": (
                    "fork() creates an exact duplicate of the calling process. The kernel creates a new Process Control Block (task_struct), "
                    "assigns a unique PID, duplicates file descriptor references, and marks memory pages as Copy-on-Write (COW). "
                    "fork() is unique because it is called once, but returns twice: returning 0 to the child process and the child's PID "
                    "to the parent process. Modifying variables in one process has zero effect on the other."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Parent", "action": "Calls fork() system call."},
                    {"step": 2, "actor": "Kernel", "action": "Allocates new PID and duplicates parent task_struct (copy_process)."},
                    {"step": 3, "actor": "MMU / Memory", "action": "Page tables copied and marked Read-Only (COW)."},
                    {"step": 4, "actor": "Child Execution", "action": "Child wakes up with return value = 0."},
                    {"step": 5, "actor": "Parent Execution", "action": "Parent receives child PID > 0."}
                ],
                "viva_questions": [
                    {
                        "q": "Why does fork() return twice?",
                        "a": "fork() duplicates the entire execution context including instruction pointer (RIP). Both parent and child resume immediately after the fork call, but the kernel returns 0 to the child and child's PID to the parent.",
                        "tip": "Return value of -1 indicates fork failure (e.g. EAGAIN process limit)."
                    },
                    {
                        "q": "Do parent and child share variables after fork()?",
                        "a": "No. They have completely isolated virtual memory spaces. Variable changes in child do not modify parent variables due to virtual memory isolation and Copy-on-Write.",
                        "tip": "Explain that they share open file table entries unless modified."
                    }
                ]
            },
            {
                "id": "exec_demo",
                "title": "Program Replacement with exec() Family",
                "file": "co2_process/exec_demo.c",
                "syscalls": ["execvp", "fork", "waitpid"],
                "difficulty": "Beginner",
                "summary": "Demonstrates execvp() replacing current process code, stack, and heap with a new binary image.",
                "explanation": (
                    "The exec() family (execl, execp, execv, execvp, execve) loads a new executable into the current process address space. "
                    "The original text, data, heap, and stack segments are destroyed and replaced by the ELF executable image. "
                    "The PID, PPID, and open file descriptors (unless O_CLOEXEC is set) are preserved. "
                    "Code following exec() is unreachable unless the exec call fails."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Child Process", "action": "Invokes execvp('/bin/ls', args)."},
                    {"step": 2, "actor": "Kernel ELF Loader", "action": "Kernel checks binary header, frees existing VMAs."},
                    {"step": 3, "actor": "Memory Mapping", "action": "Maps new text segment, data segment, and fresh stack."},
                    {"step": 4, "actor": "Entry Point", "action": "Sets instruction pointer (RIP) to the new binary entry point (_start)."},
                    {"step": 5, "actor": "Execution", "action": "New program starts executing within the same PID."}
                ],
                "viva_questions": [
                    {
                        "q": "Does exec() assign a new Process ID (PID)?",
                        "a": "No, the PID remains identical. exec() overwrites the existing process memory with a new program image without creating a new process.",
                        "tip": "Contrast fork() (new PID, same code) with exec() (same PID, new code)."
                    },
                    {
                        "q": "When does exec() return to the calling program?",
                        "a": "exec() ONLY returns if it encounters an error (returns -1, e.g. ENOENT file not found or EACCES permission denied). On success, it never returns.",
                        "tip": "Always place error handling code immediately following any exec call."
                    }
                ]
            },
            {
                "id": "wait_exit",
                "title": "Process Synchronization with wait() & exit()",
                "file": "co2_process/wait_exit.c",
                "syscalls": ["wait", "waitpid", "exit"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates exit status transmission, WIFEXITED/WEXITSTATUS macros, and zombie process reaping.",
                "explanation": (
                    "When a process calls exit(status), the kernel deallocates its virtual memory, closes its open descriptors, "
                    "and transitions the process into the ZOMBIE (EXIT_ZOMBIE) state. The process's entry in the process table and "
                    "its task_struct remain alive to preserve its termination exit code for the parent. "
                    "When the parent calls wait() or waitpid(), the kernel transfers the exit code to the parent and frees the PCB."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Child", "action": "Calls exit(42); kernel transitions state to TASK_DEAD/ZOMBIE."},
                    {"step": 2, "actor": "Kernel Signal", "action": "Kernel sends SIGCHLD signal to parent process."},
                    {"step": 3, "actor": "Parent", "action": "Parent calls waitpid(); reaps child and receives status word."},
                    {"step": 4, "actor": "Macro Evaluation", "action": "WIFEXITED evaluates true, WEXITSTATUS extracts 42."},
                    {"step": 5, "actor": "Deallocation", "action": "Child task_struct completely purged from process table."}
                ],
                "viva_questions": [
                    {
                        "q": "What is a Zombie Process, and how is it created?",
                        "a": "A zombie process is a process that has completed execution via exit(), but its entry still remains in the process table because its parent has not yet called wait() to read its exit status.",
                        "tip": "Zombies consume no memory or CPU, only a slot in the PID table."
                    },
                    {
                        "q": "What is an Orphan Process, and who adopts it?",
                        "a": "An orphan process is a running child whose parent terminated before calling wait(). In Linux, orphan processes are adopted by PID 1 (init / systemd), which regularly calls wait() to reap them.",
                        "tip": "Differentiate clearly between zombies (dead) and orphans (alive)."
                    }
                ]
            },
            {
                "id": "getpid_ppid",
                "title": "Process Identification: getpid() & getppid()",
                "file": "co2_process/getpid_ppid.c",
                "syscalls": ["getpid", "getppid"],
                "difficulty": "Beginner",
                "summary": "Queries the kernel for process PID and parent PPID to demonstrate hierarchy.",
                "explanation": (
                    "Every process in Linux is represented by a task_struct in kernel memory containing 'pid' (Process ID) "
                    "and a pointer to 'real_parent' containing the parent's task_struct. "
                    "getpid() returns current->pid, while getppid() returns current->real_parent->pid. "
                    "The PID namespace forms a tree originating from PID 1 (systemd/init)."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "User Application", "action": "Invokes getpid() system call."},
                    {"step": 2, "actor": "Kernel Ring 0", "action": "Accesses current task_struct pointer from CPU per-CPU data."},
                    {"step": 3, "actor": "PID Extraction", "action": "Reads task_struct->pid and returns integer."},
                    {"step": 4, "actor": "PPID Extraction", "action": "Traverses current->real_parent->pid for getppid()."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the PID of the ancestor process of all Linux processes?",
                        "a": "PID 1, which is the init / systemd process booted directly by the kernel after hardware initialization.",
                        "tip": "PID 0 is the idle swapper process."
                    }
                ]
            },
            {
                "id": "process_tree",
                "title": "Multi-Tier Process Tree & Reaping",
                "file": "co2_process/process_tree.c",
                "syscalls": ["fork", "waitpid", "getpid", "getppid"],
                "difficulty": "Intermediate",
                "summary": "Simulates Grandparent -> Parent -> Child hierarchy with multi-stage synchronized reaping.",
                "explanation": (
                    "In Linux, processes form an acyclic directed tree rooted at PID 1. When processes create children at multiple tiers, "
                    "each parent is strictly responsible for waiting on its direct children. Intermediate processes that terminate "
                    "without waiting on their children trigger kernel adoption logic."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Grandparent (Tier 1)", "action": "Forks Parent (Tier 2)."},
                    {"step": 2, "actor": "Parent (Tier 2)", "action": "Forks Grandchild (Tier 3)."},
                    {"step": 3, "actor": "Grandchild (Tier 3)", "action": "Executes task and calls exit(103)."},
                    {"step": 4, "actor": "Parent (Tier 2)", "action": "Reaps Grandchild, then calls exit(102)."},
                    {"step": 5, "actor": "Grandparent (Tier 1)", "action": "Reaps Parent. Entire tree safely resolved."}
                ],
                "viva_questions": [
                    {
                        "q": "Can a parent wait for its grandchild directly?",
                        "a": "No, wait() and waitpid() only permit a process to wait for its direct child processes. A grandparent cannot directly reap grandchildren unless adoption occurs.",
                        "tip": "Explain that POSIX process hierarchy limits wait to direct children."
                    }
                ]
            }
        ]
    },
    {
        "id": "co3",
        "code": "CO3",
        "title": "Inter-Process Communication (IPC)",
        "badge": "IPC & Synchronization",
        "icon": "share-2",
        "summary": "Covers Anonymous Pipes, Named Pipes (FIFO), Unix Domain Sockets, Signals, and Shared Memory.",
        "demos": [
            {
                "id": "pipe_anonymous",
                "title": "Anonymous Pipes (pipe)",
                "file": "co3_ipc/pipe_anonymous.c",
                "syscalls": ["pipe", "read", "write", "close"],
                "difficulty": "Beginner",
                "summary": "Creates a unidirectional kernel ring buffer between related parent and child processes.",
                "explanation": (
                    "The pipe() system call allocates an anonymous unidirectional channel managed as a FIFO ring buffer inside kernel memory "
                    "(default 64 KB capacity in Linux). It populates an array of two file descriptors: pipefd[0] for reading and pipefd[1] for writing. "
                    "When writing, data is buffered in kernel RAM without hitting the disk. "
                    "When all write descriptors are closed, readers receive EOF (0 bytes read). "
                    "If writing to a pipe with no active readers, the kernel delivers SIGPIPE."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Parent", "action": "Calls pipe(pipefd). Kernel allocates in-memory pipe inode."},
                    {"step": 2, "actor": "Fork", "action": "Child inherits pipefd[0] and pipefd[1] open file descriptors."},
                    {"step": 3, "actor": "Descriptor Cleanup", "action": "Child closes write end (fd[1]); parent closes read end (fd[0])."},
                    {"step": 4, "actor": "Write", "action": "Parent writes string; kernel copies bytes into pipe buffer."},
                    {"step": 5, "actor": "Read", "action": "Child reads bytes from pipe buffer into user memory."}
                ],
                "viva_questions": [
                    {
                        "q": "Why must the unused pipe ends be closed in parent and child?",
                        "a": "If the child reader does not close its own inherited write end, read() will block indefinitely waiting for input because the kernel sees that a write descriptor is still open.",
                        "tip": "Closing the write end generates EOF for the reader."
                    },
                    {
                        "q": "Can two unrelated processes communicate using anonymous pipes?",
                        "a": "No. Anonymous pipes require a common ancestor (parent-child relationship) because file descriptors are shared exclusively across fork(). Use Named Pipes (FIFOs) for unrelated processes.",
                        "tip": "Highlight the difference between anonymous pipes and FIFOs."
                    }
                ]
            },
            {
                "id": "fifo_named_pipe",
                "title": "Named Pipes (FIFO / mkfifo)",
                "file": "co3_ipc/fifo_named_pipe.c",
                "syscalls": ["mkfifo", "open", "read", "write", "unlink"],
                "difficulty": "Intermediate",
                "summary": "Creates a named FIFO filesystem entry enabling communication between unrelated processes.",
                "explanation": (
                    "A Named Pipe (FIFO) created via mkfifo() exists as a special file node on the filesystem directory tree (type 'p'). "
                    "Unlike standard disk files, a FIFO contains no data on disk; data flows entirely through a kernel buffer. "
                    "A FIFO allows completely independent, unrelated processes to communicate simply by opening the file path. "
                    "Opening a FIFO for reading blocks until another process opens it for writing, providing natural synchronization."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Filesystem", "action": "mkfifo('/tmp/demo_fifo', 0666) creates FIFO directory entry."},
                    {"step": 2, "actor": "Reader Open", "action": "open(O_RDONLY) blocks until a writer connects."},
                    {"step": 3, "actor": "Writer Open", "action": "open(O_WRONLY) unblocks both processes."},
                    {"step": 4, "actor": "Kernel Transfer", "action": "write() copies data to kernel buffer; read() drains it."},
                    {"step": 5, "actor": "Cleanup", "action": "unlink() removes the filesystem name node."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the key difference between an Anonymous Pipe and a Named Pipe (FIFO)?",
                        "a": "An anonymous pipe has no filesystem name and can only be used by related processes sharing descriptors via fork(). A Named Pipe has a path on the filesystem (e.g. /tmp/fifo) and can connect completely unrelated processes.",
                        "tip": "Both use kernel ring buffers without disk I/O."
                    }
                ]
            },
            {
                "id": "unix_sockets",
                "title": "Unix Domain Sockets (AF_UNIX)",
                "file": "co3_ipc/unix_sockets.c",
                "syscalls": ["socket", "bind", "listen", "accept", "connect", "send", "recv"],
                "difficulty": "Advanced",
                "summary": "Demonstrates bidirectional local stream sockets providing full duplex inter-process communication.",
                "explanation": (
                    "Unix Domain Sockets (AF_UNIX / AF_LOCAL) use the standard POSIX socket API but bypass the network stack "
                    "(no IP headers, TCP checksums, or packet routing). Communication occurs directly through kernel memory buffers, "
                    "making Unix domain sockets significantly faster than TCP loopback (127.0.0.1) while providing two-way full-duplex communication."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Server", "action": "socket(AF_UNIX, SOCK_STREAM), bind('/tmp/sock'), listen()."},
                    {"step": 2, "actor": "Client", "action": "socket(AF_UNIX), connect('/tmp/sock')."},
                    {"step": 3, "actor": "Kernel Handshake", "action": "Kernel establishes local bidirectional socket pair."},
                    {"step": 4, "actor": "Bidirectional I/O", "action": "Client sends query; server receives and returns reply."},
                    {"step": 5, "actor": "Socket Teardown", "action": "Descriptors closed and socket file unlinked."}
                ],
                "viva_questions": [
                    {
                        "q": "Why are Unix Domain Sockets preferred over TCP loopback for local IPC?",
                        "a": "Unix Domain Sockets do not incur the overhead of TCP/IP protocol encapsulation, checksum verification, routing tables, or port allocation. They also support passing file descriptors and process credentials.",
                        "tip": "Used heavily by Docker daemon, X11, and systemd."
                    }
                ]
            },
            {
                "id": "signals_demo",
                "title": "Asynchronous Signals (signal, kill, raise, alarm)",
                "file": "co3_ipc/signals_demo.c",
                "syscalls": ["signal", "sigaction", "kill", "raise", "alarm", "pause"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates asynchronous notification, custom signal handlers, self-signals, and timer alarms.",
                "explanation": (
                    "Signals are software interrupts sent to a process by the kernel or another process to notify it of an asynchronous event. "
                    "Common signals include SIGINT (Ctrl+C), SIGKILL (kill -9, uncatchable), SIGSEGV (segmentation fault), and user-defined SIGUSR1. "
                    "When a signal is delivered, the kernel interrupts the user program's normal execution flow and jumps directly to the "
                    "registered signal handler before returning to the original instruction."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Registration", "action": "signal(SIGUSR1, handler) registers handler function address."},
                    {"step": 2, "actor": "Sender Process", "action": "Invokes kill(target_pid, SIGUSR1)."},
                    {"step": 3, "actor": "Kernel Signal Table", "action": "Kernel sets pending signal bit in target task_struct."},
                    {"step": 4, "actor": "Interruption", "action": "Target process interrupted; kernel pushes stack frame for handler."},
                    {"step": 5, "actor": "sigreturn", "action": "Handler completes; sigreturn restores CPU registers."}
                ],
                "viva_questions": [
                    {
                        "q": "Which two signals cannot be caught, blocked, or ignored?",
                        "a": "SIGKILL (Signal 9) and SIGSTOP (Signal 19) cannot be caught or ignored. The kernel immediately acts upon them.",
                        "tip": "Explain that SIGTERM can be handled for graceful shutdown."
                    },
                    {
                        "q": "What is the difference between synchronous and asynchronous signals?",
                        "a": "Synchronous signals are generated directly by the executing instruction (e.g. SIGFPE divide by zero, SIGSEGV invalid memory access). Asynchronous signals come externally from other processes or hardware timers (e.g. SIGINT, SIGALRM, kill()).",
                        "tip": "Mention reentrant functions and sig_atomic_t."
                    }
                ]
            },
            {
                "id": "shared_memory",
                "title": "Shared Memory (shmget, shmat, shmdt, shmctl)",
                "file": "co3_ipc/shared_memory.c",
                "syscalls": ["shmget", "shmat", "shmdt", "shmctl"],
                "difficulty": "Advanced",
                "summary": "Demonstrates System V shared memory providing zero-copy direct RAM sharing between processes.",
                "explanation": (
                    "Shared Memory is the fastest form of IPC available in operating systems because data does not pass through the kernel "
                    "during read/write operations. Once the shared memory segment is created via shmget() and attached to each process's "
                    "address space via shmat(), both processes map the identical physical memory frames. "
                    "Writing to the memory pointer in one process is immediately visible to the other with zero system call overhead."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "shmget()", "action": "Kernel allocates physical memory segment identifier (shmid)."},
                    {"step": 2, "actor": "shmat()", "action": "Maps physical frames into calling process's page table."},
                    {"step": 3, "actor": "Direct I/O", "action": "Processes write and read memory pointers directly in RAM."},
                    {"step": 4, "actor": "shmdt()", "action": "Detaches segment from process virtual address space."},
                    {"step": 5, "actor": "shmctl(IPC_RMID)", "action": "Marks segment for deallocation when all references drop to 0."}
                ],
                "viva_questions": [
                    {
                        "q": "Why is Shared Memory faster than Pipes or Sockets?",
                        "a": "Pipes and sockets require copying data from user space to kernel buffer, then copying from kernel buffer back to user space (2 context switches and 2 memory copies). Shared memory maps the same physical RAM into both processes, requiring zero copies and zero system calls during data transfer.",
                        "tip": "Highlight that synchronization (semaphores/mutexes) is needed to avoid race conditions."
                    }
                ]
            }
        ]
    },
    {
        "id": "co4",
        "code": "CO4",
        "title": "Virtual Memory Management",
        "badge": "Paging & Memory",
        "icon": "hard-drive",
        "summary": "Demonstrates Paging, Address Translation, TLB, Page Faults, Demand Paging, Copy-on-Write, mmap/munmap.",
        "demos": [
            {
                "id": "mmap_demo",
                "title": "Memory Mapping (mmap & munmap)",
                "file": "co4_vm/mmap_demo.c",
                "syscalls": ["mmap", "munmap", "msync", "ftruncate"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates anonymous and file-backed memory mapping, zero-syscall disk access, and unmapping.",
                "explanation": (
                    "mmap() creates a new virtual memory area (VMA) in the calling process's address space. "
                    "In anonymous mapping (MAP_ANONYMOUS), it allocates zero-initialized RAM directly from the kernel without file backing. "
                    "In file-backed mapping, it maps the contents of a disk file directly to memory addresses. "
                    "Applications can read and write files using ordinary pointer dereferencing (*ptr = 'x'), and changes are lazily "
                    "synced to storage by the kernel's dirty page flusher or explicitly with msync()."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "User Space", "action": "Calls mmap(NULL, 4096, PROT_READ|WRITE, MAP_SHARED, fd, 0)."},
                    {"step": 2, "actor": "Kernel VMA", "action": "Kernel creates vm_area_struct in process mm_struct."},
                    {"step": 3, "actor": "Page Fault", "action": "First pointer access triggers page fault; kernel loads 4KB disk block into page cache."},
                    {"step": 4, "actor": "Dirty Page", "action": "Modifications mark page table dirty bit."},
                    {"step": 5, "actor": "msync & munmap", "action": "Changes flushed to disk and VMA removed from page table."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the difference between MAP_SHARED and MAP_PRIVATE in mmap()?",
                        "a": "MAP_SHARED writes modifications directly to the underlying file and shares them with other mapping processes. MAP_PRIVATE uses Copy-on-Write so modifications are private to the process and never written back to disk.",
                        "tip": "MAP_PRIVATE is used by the dynamic linker (ld.so) to load shared libraries."
                    }
                ]
            },
            {
                "id": "copy_on_write",
                "title": "Virtual Memory Copy-on-Write (COW)",
                "file": "co4_vm/copy_on_write.c",
                "syscalls": ["fork", "mmap", "munmap", "wait"],
                "difficulty": "Advanced",
                "summary": "Demonstrates physical frame sharing across fork, write-protection trap, and on-demand page duplication.",
                "explanation": (
                    "Copy-on-Write (COW) is a fundamental optimization in modern operating systems. "
                    "When fork() is called, rather than duplicating hundreds of megabytes of process physical memory, "
                    "the kernel simply copies the parent's page tables and marks all pages as READ-ONLY in both parent and child. "
                    "If either process writes to a page, the CPU MMU detects a write to a read-only page and triggers Page Fault Exception 14. "
                    "The kernel catches the trap, allocates a new physical frame, copies the 4KB data, updates the faulting process's page table "
                    "to Read-Write, and resumes execution seamlessly."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Parent", "action": "Allocates 4KB page and writes data."},
                    {"step": 2, "actor": "fork()", "action": "Kernel marks page PTE as Read-Only in both parent and child."},
                    {"step": 3, "actor": "Child Write", "action": "Child attempts write: shared_page[0] = 'X'."},
                    {"step": 4, "actor": "MMU Trap 14", "action": "Hardware raises Page Fault Exception; kernel enters handle_pte_fault()."},
                    {"step": 5, "actor": "Frame Duplication", "action": "Kernel allocates new 4KB physical frame, copies data, grants Write access."}
                ],
                "viva_questions": [
                    {
                        "q": "Why is Copy-on-Write crucial for performance during fork() + exec()?",
                        "a": "Most processes call exec() immediately after fork(). If the OS copied all memory during fork(), those gigabytes of copied RAM would be discarded seconds later when exec() wipes the address space. COW makes fork() nearly instantaneous O(1).",
                        "tip": "Explain that physical frames are only copied if written to before exec."
                    }
                ]
            },
            {
                "id": "paging_translation",
                "title": "Address Translation, Paging & TLB Simulation",
                "file": "co4_vm/paging_translation.c",
                "syscalls": ["getpid"],
                "difficulty": "Intermediate",
                "summary": "Extracts Virtual Page Number (VPN) and Offset, models TLB hit/miss, and computes physical address.",
                "explanation": (
                    "In 4 KB Paging architecture, a virtual address is divided into: "
                    "1. Virtual Page Number (VPN): upper bits indexing into the Page Table. "
                    "2. Page Offset: lower 12 bits (2^12 = 4096 bytes) pointing to the exact byte within the page. "
                    "The CPU MMU first queries the Translation Lookaside Buffer (TLB), an ultra-fast hardware associative cache. "
                    "On a TLB Hit, the Physical Frame Number (PFN) is returned in 1 clock cycle. "
                    "On a TLB Miss, the MMU walks the multi-level page table hierarchy (CR3 register -> PGD -> P4D -> PUD -> PMD -> PTE) "
                    "and calculates: Physical Address = (PFN << 12) | Offset."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Virtual Address", "action": "Virtual address received from CPU (e.g. 0x7ffd1234)."},
                    {"step": 2, "actor": "Bitmask Split", "action": "Offset = Addr & 0xFFF (12 bits); VPN = Addr >> 12."},
                    {"step": 3, "actor": "TLB Query", "action": "Associative lookup in CPU L1/L2 TLB cache."},
                    {"step": 4, "actor": "Page Table Walk", "action": "If TLB Miss, hardware walks CR3 page directory to fetch PTE."},
                    {"step": 5, "actor": "Physical Generation", "action": "Physical Address = (PFN * 4096) + Offset."}
                ],
                "viva_questions": [
                    {
                        "q": "Why is page size commonly 4096 bytes (4 KB)?",
                        "a": "4 KB strikes a balanced trade-off between internal fragmentation (wasted memory within a page) and page table size (number of page table entries needed to map memory).",
                        "tip": "Mention Huge Pages (2 MB / 1 GB) used for databases and virtualization."
                    },
                    {
                        "q": "What is the function of the Translation Lookaside Buffer (TLB)?",
                        "a": "The TLB is a high-speed hardware cache on the CPU chip that stores recent virtual-to-physical address translations to prevent slow RAM memory access cycles during page table walks.",
                        "tip": "TLB hits take ~1ns; page table walks in main memory take ~50-100ns."
                    }
                ]
            },
            {
                "id": "demand_paging",
                "title": "Demand Paging & Hardware Page Faults",
                "file": "co4_vm/demand_paging.c",
                "syscalls": ["mmap", "getrusage", "munmap"],
                "difficulty": "Intermediate",
                "summary": "Measures real Linux minor page faults (ru_minflt) to prove physical RAM is committed only when touched.",
                "explanation": (
                    "Demand Paging is a memory management scheme where pages are loaded into physical memory only when demanded "
                    "(accessed) by the program, rather than pre-allocated upfront. "
                    "When an application allocates memory (via malloc or mmap), Linux assigns only virtual address space ranges. "
                    "When the application touches a page for the first time, a Minor Page Fault occurs, prompting the kernel to allocate "
                    "a physical RAM frame. Subsequent accesses to the same page cause zero page faults."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Allocation", "action": "mmap allocates 64 MB virtual address space; 0 physical frames committed."},
                    {"step": 2, "actor": "Page Access", "action": "Instruction writes to buffer[i * 4096]."},
                    {"step": 3, "actor": "Hardware Fault", "action": "CPU fires Minor Page Fault (ru_minflt increases by 1)."},
                    {"step": 4, "actor": "Frame Allocation", "action": "Linux buddy allocator assigns physical 4KB frame."},
                    {"step": 5, "actor": "Second Access", "action": "Subsequent reads/writes hit physical RAM directly with 0 faults."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the difference between a Minor Page Fault and a Major Page Fault?",
                        "a": "A Minor Page Fault occurs when the required page is already in physical memory (e.g. shared library or newly allocated zeroed frame), requiring only page table updates. A Major Page Fault occurs when the page must be read from slow disk storage (swap space or executable file).",
                        "tip": "Major page faults incur significant I/O latency."
                    }
                ]
            }
        ]
    },
    {
        "id": "co5",
        "code": "CO5",
        "title": "File Management",
        "badge": "VFS & Filesystem",
        "icon": "folder",
        "summary": "Demonstrates open(), read(), write(), close(), lseek(), stat(), and unlink() system calls.",
        "demos": [
            {
                "id": "file_crud",
                "title": "File I/O: open(), write(), read(), close()",
                "file": "co5_file/file_crud.c",
                "syscalls": ["open", "write", "read", "close"],
                "difficulty": "Beginner",
                "summary": "Demonstrates fundamental low-level system calls, file descriptors, and VFS file operations.",
                "explanation": (
                    "In Unix/Linux, everything is a file. Low-level file operations bypass the C standard I/O library buffer "
                    "and communicate directly with the kernel's Virtual File System (VFS). "
                    "open() returns an integer File Descriptor (index in the process's file descriptor table pointing to an open file description). "
                    "write() transfers bytes into the kernel's Page Cache. "
                    "read() pulls bytes from the page cache into user memory. "
                    "close() releases the file descriptor index."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "open()", "action": "Kernel checks permissions, allocates fd (e.g. fd=3) in process files_struct."},
                    {"step": 2, "actor": "write()", "action": "Copies user buffer to kernel page cache; marks inode dirty."},
                    {"step": 3, "actor": "close()", "action": "Decrements file table refcount and frees fd index."},
                    {"step": 4, "actor": "open() (read)", "action": "Reopens file with O_RDONLY; sets file position offset to 0."},
                    {"step": 5, "actor": "read()", "action": "Reads data into buffer; advances file offset by bytes read."}
                ],
                "viva_questions": [
                    {
                        "q": "What are the standard file descriptors automatically opened for every Linux process?",
                        "a": "0 = STDIN_FILENO (Standard Input), 1 = STDOUT_FILENO (Standard Output), 2 = STDERR_FILENO (Standard Error).",
                        "tip": "Custom open files start at descriptor 3."
                    },
                    {
                        "q": "What is the difference between open() / read() and fopen() / fread()?",
                        "a": "open() and read() are direct kernel system calls operating on raw file descriptors with no user-space buffering. fopen() and fread() are C standard library (stdio) functions that operate on FILE* streams and wrap user-space memory buffers to reduce system call overhead.",
                        "tip": "Explain that stdio buffers multiple small writes before calling write() once."
                    }
                ]
            },
            {
                "id": "lseek_demo",
                "title": "Random Access File Pointer: lseek()",
                "file": "co5_file/lseek_demo.c",
                "syscalls": ["lseek", "open", "read", "write", "close"],
                "difficulty": "Beginner",
                "summary": "Demonstrates manipulating file offsets with SEEK_SET, SEEK_CUR, and SEEK_END.",
                "explanation": (
                    "Every open file description in the kernel maintains a current file offset pointer (f_pos). "
                    "Normal reads and writes automatically increment this pointer sequentially. "
                    "lseek() allows random access repositioning of the offset pointer. "
                    "SEEK_SET sets the offset relative to the beginning of the file. "
                    "SEEK_CUR sets it relative to the current position. "
                    "SEEK_END sets it relative to the end of the file (also used to determine exact file size)."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "open()", "action": "File opened; kernel sets f_pos = 0."},
                    {"step": 2, "actor": "lseek(SEEK_SET, 10)", "action": "Kernel updates f_pos = 10."},
                    {"step": 3, "actor": "read(5)", "action": "Reads 5 bytes from byte 10; f_pos advances to 15."},
                    {"step": 4, "actor": "lseek(SEEK_CUR, 5)", "action": "Advances f_pos by 5; f_pos becomes 20."},
                    {"step": 5, "actor": "lseek(SEEK_END, 0)", "action": "Sets f_pos to end of file, returning total file size."}
                ],
                "viva_questions": [
                    {
                        "q": "Can lseek() be used on an anonymous pipe or socket?",
                        "a": "No. lseek() fails and returns ESPIPE (Illegal seek) when invoked on pipes, FIFOs, or sockets because they are sequential streams without random-access seek capabilities.",
                        "tip": "Only regular files and block devices support lseek()."
                    }
                ]
            },
            {
                "id": "stat_inode",
                "title": "File Metadata & Inode Query: stat() & fstat()",
                "file": "co5_file/stat_inode.c",
                "syscalls": ["stat", "fstat"],
                "difficulty": "Intermediate",
                "summary": "Extracts inode numbers, hard link counts, permission octals, block counts, and timestamps.",
                "explanation": (
                    "In Linux filesystems (ext4, XFS), files are identified internally by an Inode (Index Node) structure. "
                    "The directory simply maps human-readable filenames to Inode numbers. "
                    "The stat() system call populates a 'struct stat' containing inode metadata: "
                    "st_ino (inode number), st_mode (file type & permissions), st_nlink (hard links), st_size (bytes), "
                    "st_blksize (optimal block size), and timestamps (st_atime, st_mtime, st_ctime)."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Path Lookup", "action": "Kernel traverses directory path to find directory dentry entry."},
                    {"step": 2, "actor": "Inode Retrieval", "action": "Kernel reads inode from disk/VFS inode cache."},
                    {"step": 3, "actor": "struct stat", "action": "Populates fields: inode, permissions, size, links, timestamps."},
                    {"step": 4, "actor": "Return to User", "action": "stat struct copied back to user memory."}
                ],
                "viva_questions": [
                    {
                        "q": "Does an Inode store the filename of the file?",
                        "a": "No. An Inode does NOT store the filename. Filenames are stored inside directory data blocks mapped to Inode numbers. An inode contains only file metadata and data block pointers.",
                        "tip": "This is why multiple filenames (hard links) can point to the exact same Inode."
                    }
                ]
            },
            {
                "id": "unlink_demo",
                "title": "File Deletion Semantics: unlink()",
                "file": "co5_file/unlink_demo.c",
                "syscalls": ["unlink", "open", "read", "write", "close"],
                "difficulty": "Intermediate",
                "summary": "Demonstrates hard link removal and shows data blocks remain accessible while file descriptors are open.",
                "explanation": (
                    "In Unix, files are deleted using unlink(), not 'delete'. "
                    "unlink() removes the specified filename entry from its directory and decrements the inode's hard link counter (st_nlink). "
                    "However, if any process currently holds an open file descriptor pointing to that inode, "
                    "the physical data blocks on disk are NOT released. The process can continue reading and writing to the file! "
                    "Only when the final open file descriptor is closed (open reference count drops to 0) does the kernel free the inode and disk blocks."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "File Open", "action": "open('test.txt') creates open file descriptor fd=3."},
                    {"step": 2, "actor": "unlink()", "action": "Directory entry removed; inode link count decrements 1 -> 0."},
                    {"step": 3, "actor": "Access via Name", "action": "Attempting to open('test.txt') fails with ENOENT."},
                    {"step": 4, "actor": "Access via fd", "action": "read(fd) succeeds; data blocks remain valid in memory/disk."},
                    {"step": 5, "actor": "close(fd)", "action": "Open refcount hits 0; kernel frees inode and deallocates data blocks."}
                ],
                "viva_questions": [
                    {
                        "q": "Why does disk space not free up immediately when a large log file is deleted while an application is running?",
                        "a": "Because the running application still holds an open file descriptor to the deleted file. The directory entry was unlinked, but the inode and disk blocks remain allocated until the application terminates or closes the file descriptor.",
                        "tip": "Use 'lsof | grep deleted' to find processes holding unlinked open files."
                    }
                ]
            }
        ]
    },
    {
        "id": "co6",
        "code": "CO6",
        "title": "Thread Management & Synchronization",
        "badge": "Threads & Concurrency",
        "icon": "git-merge",
        "summary": "Demonstrates POSIX Threads, Mutex Locks, Condition Variables, Counting Semaphores, and Deadlocks.",
        "demos": [
            {
                "id": "pthread_basic",
                "title": "POSIX Threads Creation & Joining",
                "file": "co6_threads/pthread_basic.c",
                "syscalls": ["pthread_create", "pthread_join", "pthread_self", "clone"],
                "difficulty": "Beginner",
                "summary": "Spawns 4 worker threads, passes structured data arguments, and retrieves thread return codes.",
                "explanation": (
                    "In Linux, threads are implemented via the Native POSIX Thread Library (NPTL) using the clone() system call. "
                    "Unlike processes which have independent address spaces, threads within the same process share the same text, "
                    "data, heap, and open file descriptors, but each thread maintains its own private Stack and CPU register state. "
                    "pthread_create() launches a new thread of execution; pthread_join() suspends the calling thread until the target finishes."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "pthread_create", "action": "Glibc invokes clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND)."},
                    {"step": 2, "actor": "Kernel Task", "action": "Kernel creates lightweight task_struct sharing mm_struct with parent."},
                    {"step": 3, "actor": "Stack Allocation", "action": "Glibc allocates private stack space (default 8MB) for worker thread."},
                    {"step": 4, "actor": "Worker Run", "action": "Worker executes function and computes sum."},
                    {"step": 5, "actor": "pthread_join", "action": "Main thread synchronizes, retrieves return pointer, and reaps thread stack."}
                ],
                "viva_questions": [
                    {
                        "q": "What resources do threads within the same process share, and what is private?",
                        "a": "Shared: Virtual address space (Code/Text, Global Data, Heap), open file descriptors, PID, signal handlers, and working directory. Private: Thread ID (TID), CPU registers (Program Counter, Stack Pointer), stack memory, and errno.",
                        "tip": "Highlight that threads share memory, necessitating synchronization."
                    }
                ]
            },
            {
                "id": "mutex_sync",
                "title": "Race Conditions vs Mutex Locks",
                "file": "co6_threads/mutex_sync.c",
                "syscalls": ["pthread_mutex_init", "pthread_mutex_lock", "pthread_mutex_unlock"],
                "difficulty": "Intermediate",
                "summary": "Directly compares unprotected counter (race condition with lost updates) vs protected mutex lock.",
                "explanation": (
                    "A Race Condition occurs when multiple threads concurrently read and write shared data without synchronization, "
                    "causing the final outcome to depend on the non-deterministic scheduling order of threads. "
                    "The counter increment (counter++) consists of 3 distinct CPU instructions: Read from memory, Increment register, Write back to memory. "
                    "A Mutex (Mutual Exclusion) lock ensures that only one thread can execute within the Critical Section at any time."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Test 1 (Unprotected)", "action": "4 threads increment shared variable simultaneously; CPU context switches cause lost updates."},
                    {"step": 2, "actor": "Test 2 (Mutex)", "action": "Threads invoke pthread_mutex_lock() before entering critical section."},
                    {"step": 3, "actor": "Lock Acquisition", "action": "First thread acquires lock atomically via CPU atomic test-and-set / futex."},
                    {"step": 4, "actor": "Contention Wait", "action": "Competing threads sleep on kernel futex queue until unlocked."},
                    {"step": 5, "actor": "Safe Result", "action": "Zero updates lost; final count matches expected calculation perfectly."}
                ],
                "viva_questions": [
                    {
                        "q": "What is a Critical Section, and what are the requirements for solving it?",
                        "a": "A Critical Section is a section of code that accesses shared mutable resources. A valid solution must satisfy: 1. Mutual Exclusion (only one thread enters), 2. Progress (selection of next entrant cannot be indefinitely postponed), 3. Bounded Waiting (limit on how many times others can enter before a requesting thread is granted entry).",
                        "tip": "Quote these three classic criteria from Operating System theory."
                    }
                ]
            },
            {
                "id": "cond_var",
                "title": "Condition Variables (Producer-Consumer)",
                "file": "co6_threads/cond_var.c",
                "syscalls": ["pthread_cond_wait", "pthread_cond_signal", "pthread_mutex_lock"],
                "difficulty": "Advanced",
                "summary": "Demonstrates condition variables coordinating a bounded circular buffer without busy waiting.",
                "explanation": (
                    "A Condition Variable allows threads to suspend execution (sleep) until a specific application condition becomes true, "
                    "avoiding wasteful CPU polling (busy waiting). "
                    "pthread_cond_wait() atomically releases the associated mutex lock and puts the calling thread to sleep. "
                    "When another thread signals the condition via pthread_cond_signal() or broadcast, the sleeping thread wakes up "
                    "and re-acquires the mutex lock before returning."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Producer", "action": "Produces item, puts in buffer, signals cond_not_empty."},
                    {"step": 2, "actor": "Consumer Check", "action": "If buffer empty, calls pthread_cond_wait(&cond_not_empty, &mutex)."},
                    {"step": 3, "actor": "Atomic Sleep", "action": "Kernel atomically releases mutex and enqueues consumer thread on sleep waitqueue."},
                    {"step": 4, "actor": "Wakeup & Re-lock", "action": "Producer's signal wakes consumer; consumer re-acquires mutex."},
                    {"step": 5, "actor": "Safe Consumption", "action": "Consumer pulls item from buffer and signals cond_not_full."}
                ],
                "viva_questions": [
                    {
                        "q": "Why MUST pthread_cond_wait() always be placed inside a 'while' loop instead of an 'if' condition?",
                        "a": "For two critical reasons: 1. Spurious Wakeups (threads can occasionally wake up without any signal delivered by the OS), and 2. Condition Change (another thread may wake up first and consume the resource before this thread re-acquires the mutex).",
                        "tip": "Always remember: 'while (condition) { pthread_cond_wait(); }'."
                    }
                ]
            },
            {
                "id": "counting_semaphore",
                "title": "Counting Semaphores: Resource Pool Control",
                "file": "co6_threads/counting_semaphore.c",
                "syscalls": ["sem_init", "sem_wait", "sem_post", "sem_destroy"],
                "difficulty": "Intermediate",
                "summary": "Controls access to a limited pool of 3 shared resources among 6 competing worker threads.",
                "explanation": (
                    "A Semaphore is a synchronization primitive maintained as an integer counter with atomic operations: "
                    "sem_wait() (P operation): decrements counter; if counter < 0, calling thread blocks until signaled. "
                    "sem_post() (V operation): increments counter; if waiting threads exist, one is awakened. "
                    "A Counting Semaphore initialized to N allows up to N threads to access a shared resource pool simultaneously."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "sem_init(pool, 3)", "action": "Semaphore counter initialized to 3."},
                    {"step": 2, "actor": "Threads 1, 2, 3", "action": "Call sem_wait(); counter decrements to 2, 1, 0; all three enter."},
                    {"step": 3, "actor": "Threads 4, 5, 6", "action": "Call sem_wait(); counter is 0; threads block on semaphore waitqueue."},
                    {"step": 4, "actor": "sem_post()", "action": "Working thread finishes and calls sem_post(); wakes one waiting thread."},
                    {"step": 5, "actor": "Completion", "action": "All threads safely arbitrated through the finite pool."}
                ],
                "viva_questions": [
                    {
                        "q": "What is the difference between a Binary Semaphore and a Mutex?",
                        "a": "A Mutex incorporates ownership semantics (only the thread that locked the mutex can unlock it). A Binary Semaphore has no ownership; one thread can wait (P) and a completely different thread or interrupt service routine can post (V).",
                        "tip": "Explain that mutexes support priority inheritance to prevent priority inversion."
                    }
                ]
            },
            {
                "id": "deadlock_simulation",
                "title": "Safe Deadlock Detection & Prevention",
                "file": "co6_threads/deadlock_simulation.c",
                "syscalls": ["pthread_mutex_timedlock", "pthread_mutex_lock", "pthread_mutex_unlock"],
                "difficulty": "Advanced",
                "summary": "Simulates Circular Wait deadlock hazard safely with timeout, and proves Lock Ordering prevention.",
                "explanation": (
                    "Deadlock is a state where a set of processes are permanently blocked because each holds a resource and waits for another. "
                    "Coffman's 4 Necessary Conditions for Deadlock: "
                    "1. Mutual Exclusion  2. Hold and Wait  3. No Preemption  4. Circular Wait. "
                    "Deadlock Prevention operates by invalidating at least one of these four conditions. "
                    "By establishing a global strict Lock Ordering (e.g. always acquire Mutex A before Mutex B), "
                    "Circular Wait is mathematically prevented from occurring."
                ),
                "kernel_flow": [
                    {"step": 1, "actor": "Thread 1", "action": "Acquires Lock A; prepares to acquire Lock B."},
                    {"step": 2, "actor": "Thread 2", "action": "Acquires Lock B; prepares to acquire Lock A (Inverted ordering)."},
                    {"step": 3, "actor": "Deadlock Hazard", "action": "Thread 1 waits for B; Thread 2 waits for A (Circular wait)."},
                    {"step": 4, "actor": "Safe Timedlock", "action": "pthread_mutex_timedlock times out safely after 2s without freezing."},
                    {"step": 5, "actor": "Prevention Solution", "action": "Both threads enforce strict acquisition order: Lock A then Lock B."}
                ],
                "viva_questions": [
                    {
                        "q": "What are the four Coffman conditions necessary for a deadlock to occur?",
                        "a": "1. Mutual Exclusion (resources cannot be shared), 2. Hold and Wait (process holding resources requests more), 3. No Preemption (resources cannot be forcibly taken away), 4. Circular Wait (a circular chain of processes each waiting for resource held by next).",
                        "tip": "Memorize the acronym 'MHNC' for exams."
                    },
                    {
                        "q": "How does Resource Ordering prevent deadlocks?",
                        "a": "Assign a global linear order (1, 2, ..., N) to all resources and mandate that every thread must acquire resources in strictly increasing numerical order. This makes circular dependency cycles impossible.",
                        "tip": "This breaks the 4th condition (Circular Wait)."
                    }
                ]
            }
        ]
    }
]
