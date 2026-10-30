# Interactive Operating System System Calls Simulator (Ubuntu Linux Pure C Edition)

A native, interactive **Operating System Laboratory and System Calls Simulator** written **100% in C language** that runs directly in the **Ubuntu Linux Terminal** using **GCC** and standard POSIX APIs. **Zero Python, zero web frameworks, zero external interpreters.**

Designed specifically for academic computer science operating system curricula, covering **Course Outcomes CO1 through CO6**.

---

## 🌟 Key Features

1. **100% Pure C Architecture**:
   - Zero Python, Node.js, or browser runtime dependencies.
   - Master program (`os_simulator.c`) and all 26 lab modules are written in standard C (C99 / C11 / POSIX.1-2008).
   - Rich ANSI-styled terminal interface with color-coded menus, tables, and execution status panels.
2. **Live Native Compilation & Execution**:
   - Zero fake or hardcoded output strings.
   - The simulator dynamically invokes `gcc` to compile each demonstration module in real-time.
   - Outputs display real kernel-allocated Process IDs (`PID`), File Descriptors (`fd`), thread IDs, and memory addresses.
3. **Low-Level Kernel Syscall Tracing (`strace`)**:
   - Built-in live system call auditing.
   - Directly inspect kernel traps, CPU register setups, and return codes for any C program.
4. **Complete Course Outcome Syllabus Coverage (CO1 — CO6)**:
   - **CO1: Operating System Architecture**: Dual-mode CPU operation (Ring 3 vs Ring 0), trap handling, custom Shell command interpreter.
   - **CO2: Process Management**: `fork()`, `execvp()`, `wait()`, `exit()`, `getpid()`, `getppid()`, multi-level process hierarchy trees.
   - **CO3: Inter-Process Communication**: Anonymous Pipes (`pipe`), Named Pipes (`mkfifo`), Unix Domain Sockets (`socketpair`), POSIX Signals (`sigaction`, `kill`), POSIX Shared Memory (`shm_open`, `mmap`).
   - **CO4: Virtual Memory Management**: 4 KB Page allocation, Virtual-to-Physical Address Translation, TLB simulation, Copy-On-Write (COW), Demand Paging (`madvise`, page faults), Memory Mapping (`mmap`, `munmap`).
   - **CO5: File Management**: Low-level I/O (`open`, `read`, `write`, `close`), File pointer manipulation (`lseek`), Inode metadata inspection (`stat`), Hard link removal (`unlink`).
   - **CO6: Thread Management & Synchronization**: POSIX Threads (`pthread_create`, `pthread_join`), Mutex Locks (`pthread_mutex_lock`), Condition Variables (`pthread_cond_wait`), POSIX Counting Semaphores (`sem_init`, `sem_wait`, `sem_post`), Coffman Deadlock simulation.
5. **Interactive Lab Tools Included**:
   - **POSIX System Call Explorer**: Search POSIX system calls, inspect prototypes, manuals, and related source files.
   - **4 KB Virtual Memory Translation Sandbox**: Input any 32-bit/64-bit virtual address to dynamically compute Page Directory Index, Virtual Page Number (VPN), and 12-bit Page Offset.
   - **Viva Examination Question Bank**: Complete viva revision guide with examiner questions, model answers, and kernel architecture notes.

---

## 🚀 Quick Start on Ubuntu Linux

### Prerequisites
Only GCC and make are required:
```bash
sudo apt update && sudo apt install -y build-essential strace
```

### 1. Clone the Repository
```bash
git clone https://github.com/saisujithonna7-jpg/2520030129-2520030545-2520030546_OS_PROJECT.git
cd 2520030129-2520030545-2520030546_OS_PROJECT
```

### 2. Build and Run Using Make
```bash
# Compile the master simulator
make

# Launch the interactive terminal simulator
make run
```

### 3. Alternative Quick Run via Shell Script
```bash
chmod +x run_ubuntu.sh
./run_ubuntu.sh
```

---

## 📂 Project Structure

```
.
├── Makefile                   # GNU Makefile for building, cleaning, testing, and running
├── os_simulator.c             # Master Interactive Operating System Simulator in 100% Pure C
├── run_ubuntu.sh              # Ubuntu launcher script (checks GCC/strace & compiles)
├── programs/                  # 26 standalone, compilable C system call programs
│   ├── co1_arch/              # Operating System Architecture
│   │   ├── user_kernel_mode.c # Ring 3 vs Ring 0 trap demonstration
│   │   ├── syscall_flow.c     # User application to glibc to SYS_write trap transition
│   │   └── mini_shell.c       # Interactive Unix Shell with fork(), execvp(), and wait()
│   ├── co2_process/           # Process Management
│   │   ├── fork_demo.c        # Child process creation with separate address spaces
│   │   ├── exec_demo.c        # Process image replacement via execvp()
│   │   ├── wait_exit.c        # Zombie reaping and WEXITSTATUS inspection
│   │   ├── getpid_ppid.c      # Process ID and Parent Process ID queries
│   │   └── process_tree.c     # Multi-generational process hierarchy
│   ├── co3_ipc/               # Inter-Process Communication
│   │   ├── pipe_anonymous.c   # Unidirectional byte streams between parent and child
│   │   ├── fifo_named_pipe.c  # Named pipes with filesystem presence
│   │   ├── unix_sockets.c     # Bidirectional AF_UNIX socket communication
│   │   ├── signals_demo.c     # Signal handlers (SIGINT, SIGALRM, SIGUSR1)
│   │   └── shared_memory.c    # POSIX shm_open and mmap shared memory
│   ├── co4_vm/                # Virtual Memory Management
│   │   ├── paging_translation.c # 4 KB page frame mathematical translation
│   │   ├── copy_on_write.c    # Page table entry write-protection and COW trap
│   │   ├── demand_paging.c    # Lazy allocation, minor page faults, and madvise
│   │   └── mmap_demo.c        # Direct memory-mapped file access
│   ├── co5_file/              # File Management
│   │   ├── file_crud.c        # open(), read(), write(), close() system calls
│   │   ├── lseek_demo.c       # SEEK_SET, SEEK_CUR, SEEK_END, and sparse file hole creation
│   │   ├── stat_inode.c       # Inode metadata, permissions, device ID, and block size
│   │   └── unlink_demo.c      # Reference count decrement and deferred deletion
│   └── co6_threads/           # Thread Management & Synchronization
│       ├── pthread_basic.c    # Multi-threaded execution sharing process address space
│       ├── mutex_sync.c       # Mutual exclusion locks eliminating race conditions
│       ├── cond_var.c         # Producer-Consumer queue with condition variables
│       ├── counting_semaphore.c # POSIX counting semaphores for bounded resources
│       └── deadlock_simulation.c # Circular wait simulation and resource hierarchy prevention
├── .gitignore                 # Binary and object artifact exclusion rules
└── README.md                  # Complete project documentation
```

---

## 🖥️ Interactive Terminal Dashboard

When launched (`./os_simulator`), the master C program displays the interactive console menu:

```text
╔══════════════════════════════════════════════════════════════════════════════╗
║             INTERACTIVE OPERATING SYSTEM SYSTEM CALLS SIMULATOR              ║
║                 Pure C Ubuntu Linux Terminal Edition • GCC Engine            ║
║             Curriculum Aligned: Course Outcomes CO1 through CO6              ║
╚══════════════════════════════════════════════════════════════════════════════╝
[HOST] Linux 6.x x86_64 GNU/Linux | [COMPILER] gcc 15.2.0
══════════════════════════════════════════════════════════════════════
Select Course Outcome Module or Lab Tool:

  [1] CO1: Operating System Architecture
      Kernel vs User Mode (Ring 3/0), Syscall Flow, Shell Architecture
  [2] CO2: Process Management
      fork(), execvp(), wait(), exit(), getpid(), getppid(), Process Trees
  [3] CO3: Inter-Process Communication (IPC)
      Anonymous Pipes, Named Pipes (FIFO), Unix Sockets, Signals, Shared Memory
  [4] CO4: Virtual Memory Management
      Paging, Address Translation, TLB, Page Faults, COW, mmap/munmap
  [5] CO5: File Management
      open(), read(), write(), close(), lseek(), stat(), unlink()
  [6] CO6: Thread Management & Synchronization
      pthreads, Mutex Locks, Condition Variables, Semaphores, Deadlocks
  ──────────────────────────────────────────────────────────────────
  [7] POSIX System Call Explorer (Search & Reference)
  [8] Interactive 4 KB Virtual Memory Address Translation Sandbox
  [9] Operating Systems Viva Examination Question Bank & Quiz
  [0] Exit Simulator
```

### Action Controls for Every Demonstration
For each C demonstration, you can perform:
1. **`[1]` ▶ Compile & Run natively on Ubuntu with GCC**: Live compilation, displays stdout/stderr, return code, and execution time in milliseconds.
2. **`[2]` ⚡ Trace Kernel Traps with `strace`**: Audits actual low-level system call traps invoked during execution.
3. **`[3]` ⚙ Run with Custom Arguments / Stdin**: Pass custom `argv` parameters or pipe inputs into the program.
4. **`[4]` 📄 View C Source Code**: Displays the heavily commented C source code directly in the terminal with line numbers.
5. **`[5]` 🧠 Kernel Deep-Dive**: In-depth explanations of kernel data structures (`task_struct`, `mm_struct`, `files_struct`, `inode`).
6. **`[6]` 🔄 Execution Flow**: Detailed step-by-step state machine transitions.
7. **`[7]` ❓ Viva Questions**: Targeted examination questions and answers for that specific system call.

---

## 🧪 Testing and Verifying All 26 Programs

Run the automated test target in the Makefile to compile all 26 C demonstration programs using GCC:

```bash
make test
```

Expected Output:
```text
[*] Testing compilation of all 26 C demonstration programs...
Compiling programs/co1_arch/mini_shell.c... OK
Compiling programs/co1_arch/syscall_flow.c... OK
Compiling programs/co1_arch/user_kernel_mode.c... OK
Compiling programs/co2_process/exec_demo.c... OK
Compiling programs/co2_process/fork_demo.c... OK
Compiling programs/co2_process/getpid_ppid.c... OK
Compiling programs/co2_process/process_tree.c... OK
Compiling programs/co2_process/wait_exit.c... OK
Compiling programs/co3_ipc/fifo_named_pipe.c... OK
Compiling programs/co3_ipc/pipe_anonymous.c... OK
Compiling programs/co3_ipc/shared_memory.c... OK
Compiling programs/co3_ipc/signals_demo.c... OK
Compiling programs/co3_ipc/unix_sockets.c... OK
Compiling programs/co4_vm/copy_on_write.c... OK
Compiling programs/co4_vm/demand_paging.c... OK
Compiling programs/co4_vm/mmap_demo.c... OK
Compiling programs/co4_vm/paging_translation.c... OK
Compiling programs/co5_file/file_crud.c... OK
Compiling programs/co5_file/lseek_demo.c... OK
Compiling programs/co5_file/stat_inode.c... OK
Compiling programs/co5_file/unlink_demo.c... OK
Compiling programs/co6_threads/cond_var.c... OK
Compiling programs/co6_threads/counting_semaphore.c... OK
Compiling programs/co6_threads/deadlock_simulation.c... OK
Compiling programs/co6_threads/mutex_sync.c... OK
Compiling programs/co6_threads/pthread_basic.c... OK
[✓] All 26 C programs successfully compiled with GCC!
```

---

## 🎓 Viva Examination Bank Overview

The built-in viva bank (`Option 9` in the master menu) covers:
1. **CO1**: Dual-Mode CPU rings (Ring 3 vs Ring 0), Interrupt Descriptor Table, MSR_LSTAR register, how `write()` translates to CPU opcode.
2. **CO2**: Return values of `fork()`, why `execvp()` never returns on success, Zombie vs Orphan processes, `WIFEXITED` status macros.
3. **CO3**: Pipe capacity (64 KB), closing unused descriptors to trigger EOF, Named Pipes vs Anonymous Pipes, Shared Memory zero-copy advantages, why SIGKILL and SIGSTOP cannot be caught.
4. **CO4**: 4 KB Paging rationale, Virtual Page Number vs Page Offset calculation, TLB hit latency (~1ns) vs page walk (~50ns), Copy-on-Write mechanism during `fork()`, Minor vs Major Page Faults.
5. **CO5**: Standard File Descriptors 0, 1, 2, `open()` vs `fopen()` (system call vs buffered stdio), why Inodes do not store filenames, `unlink()` deferred block deallocation.
6. **CO6**: Shared vs Private thread resources, Race condition 3-step cycle (Read-Modify-Write), 3 requirements for Critical Section (Mutual Exclusion, Progress, Bounded Waiting), Coffman's 4 conditions for Deadlock, and Lock Ordering prevention.
