# Interactive Operating System System Calls Simulator (Ubuntu Linux CLI Edition)

A native, interactive **Operating System Laboratory and System Calls Simulator** that runs directly in the **Ubuntu Linux Terminal** using **C programming** and the **GCC compiler**.

Designed specifically for computer science academic operating system curricula, aligning directly with **Course Outcomes CO1 through CO6**.

---

## 🌟 Key Features

1. **Pure Ubuntu Linux Terminal Application**:
   - Zero browser or UI framework dependency.
   - Runs directly inside your Ubuntu Linux terminal / console.
   - Built with ANSI styling, colored menus, and clear execution panels.
2. **Real Native C Compilation & Execution**:
   - Zero simulated or hardcoded outputs.
   - Every single demonstration compiles live using **GCC** (`gcc -Wall -Wextra -pthread`).
   - Produces actual Linux Process IDs (`PID`), File Descriptors (`fd`), and real memory addresses.
3. **Low-Level Kernel Syscall Tracing (`strace`)**:
   - Audit real-time system call traps, CPU register setups, and kernel return codes for any C program.
4. **Complete Course Outcome Coverage (CO1 — CO6)**:
   - **CO1**: Operating System Architecture (Ring 3 vs Ring 0, Trap flow, Shell REPL)
   - **CO2**: Process Management (`fork`, `exec`, `wait`, `exit`, `getpid`, `getppid`, Process Trees)
   - **CO3**: Inter-Process Communication (Anonymous Pipes, FIFOs, Unix Sockets, Signals, Shared Memory)
   - **CO4**: Virtual Memory Management (4 KB Paging, Address Translation, TLB, COW, Demand Paging, `mmap`)
   - **CO5**: File Management (`open`, `read`, `write`, `close`, `lseek`, `stat`, `unlink`)
   - **CO6**: Thread Management & Synchronization (`pthreads`, Mutex Locks, Condition Variables, Counting Semaphores, Deadlocks)
5. **Interactive Lab Tools Included**:
   - **POSIX System Call Explorer**: Search any system call, view prototype, description, and launch related demos.
   - **Virtual Memory Address Translation Sandbox**: Translate any virtual memory address into its 12-bit offset, VPN, and physical RAM frame.
   - **Comprehensive Viva Exam Question Bank**: Flashcards with questions, examiner answers, and practical tips.

---

## 🚀 Quick Start on Ubuntu Linux

### 1. Clone the Repository
```bash
git clone https://github.com/saisujithonna7-jpg/2520030129-2520030545-2520030546_OS_PROJECT.git
cd 2520030129-2520030545-2520030546_OS_PROJECT
```

### 2. Run the Interactive Simulator
Simply execute the launcher script:
```bash
chmod +x run_ubuntu.sh
./run_ubuntu.sh
```

Alternatively, you can run the simulator directly using Python:
```bash
python3 os_simulator.py
```

---

## 📂 Project Structure

```
os-syscall-simulator/
├── os_simulator.py            # Master Interactive Ubuntu CLI Terminal Application
├── run_ubuntu.sh              # Ubuntu launcher script (checks dependencies & runs)
├── backend/
│   ├── metadata.py            # Central syllabus database (CO1-CO6, explanations, viva bank)
│   ├── validate_c_files.py    # Automated test compiler for all 26 C programs
│   ├── app.py                 # (Optional) REST server if API access is needed
│   └── c_programs/
│       ├── co1_arch/          # user_kernel_mode.c, syscall_flow.c, mini_shell.c
│       ├── co2_process/       # fork_demo.c, exec_demo.c, wait_exit.c, getpid_ppid.c, process_tree.c
│       ├── co3_ipc/           # pipe_anonymous.c, fifo_named_pipe.c, unix_sockets.c, signals_demo.c, shared_memory.c
│       ├── co4_vm/            # mmap_demo.c, copy_on_write.c, paging_translation.c, demand_paging.c
│       ├── co5_file/          # file_crud.c, lseek_demo.c, stat_inode.c, unlink_demo.c
│       └── co6_threads/       # pthread_basic.c, mutex_sync.c, cond_var.c, counting_semaphore.c, deadlock_simulation.c
├── frontend/                  # (Optional) HTML/CSS/JS web dashboard
├── .gitignore                 # Ignore compiler binaries and bytecode
└── README.md                  # Comprehensive documentation
```

---

## 📋 Terminal Interactive Menu Walkthrough

When you start `./run_ubuntu.sh`, you will be greeted by the master terminal dashboard:

```text
╔══════════════════════════════════════════════════════════════════════════════╗
║             INTERACTIVE OPERATING SYSTEM SYSTEM CALLS SIMULATOR              ║
║           Pure Ubuntu Linux Terminal Edition • GCC & strace Engine           ║
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

### Action Controls for Each Demonstration:
For any chosen demonstration, you can:
- **`[1]` ▶ Compile & Run natively on Ubuntu with GCC**: Compiles and executes in real-time, displaying process stdout/stderr, return code, and execution time in ms.
- **`[2]` ⚡ Trace Kernel Traps with `strace`**: Audits actual low-level system calls invoked during execution.
- **`[3]` ⚙ Run with Custom Arguments / Stdin**: Pass custom `argv` parameters or pipe standard input into the program.
- **`[4]` 📄 View C Source Code**: Displays the commented C source with line numbers.
- **`[5]` 🧠 Kernel Deep-Dive**: Explains internal data structures (`task_struct`, `mm_struct`, `files_struct`, `inode`, `page_table`).
- **`[6]` 🔄 Execution Flow**: Step-by-step kernel state machine sequence.
- **`[7]` ❓ Viva Questions**: Targeted viva Q&A for that specific system call.

---

## 🧪 Validating All 26 C Programs

To verify that all 26 C programs compile cleanly with zero errors on Ubuntu GCC, run:
```bash
python3 backend/validate_c_files.py
```
Output:
```text
Total C files found: 26
[OK] co1_arch/mini_shell.c
[OK] co1_arch/syscall_flow.c
[OK] co1_arch/user_kernel_mode.c
[OK] co2_process/exec_demo.c
[OK] co2_process/fork_demo.c
...
Validation Summary: 26 passed, 0 failed.
```

---

## 🎓 Viva Examination Bank Overview

Topics covered in the built-in Viva Bank (`Option 9`):
1. **CO1**: Dual-Mode CPU rings (Ring 3 vs Ring 0), Interrupt Descriptor Table, MSR_LSTAR register, how `write()` translates to CPU opcode.
2. **CO2**: Return values of `fork()`, why `execvp()` never returns on success, Zombie vs Orphan processes, `WIFEXITED` status macros.
3. **CO3**: Pipe capacity (64 KB), closing unused descriptors to trigger EOF, Named Pipes vs Anonymous Pipes, Shared Memory zero-copy advantages, why SIGKILL and SIGSTOP cannot be caught.
4. **CO4**: 4 KB Paging rationale, Virtual Page Number vs Page Offset calculation, TLB hit latency (~1ns) vs page walk (~50ns), Copy-on-Write mechanism during `fork()`, Minor vs Major Page Faults.
5. **CO5**: Standard File Descriptors 0, 1, 2, `open()` vs `fopen()` (system call vs buffered stdio), why Inodes do not store filenames, `unlink()` deferred block deallocation.
6. **CO6**: Shared vs Private thread resources, Race condition 3-step cycle (Read-Modify-Write), 3 requirements for Critical Section (Mutual Exclusion, Progress, Bounded Waiting), Coffman's 4 conditions for Deadlock, and Lock Ordering prevention.
