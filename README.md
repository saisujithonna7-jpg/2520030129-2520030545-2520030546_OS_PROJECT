# Interactive Operating System System Calls Simulator

An interactive, web-based Operating System laboratory and simulator that compiles and executes real **C programs** natively on **Ubuntu Linux** using the **GCC compiler**.

Designed specifically for academic computer science operating system curricula, aligning directly with **Course Outcomes CO1 through CO6**.

---

## 🌟 Key Features

1. **Native Ubuntu Linux & GCC Execution**:
   - Zero hardcoded or mocked outputs.
   - Every demonstration compiles and runs natively on an actual Ubuntu Linux environment using GCC.
   - Accurate process PIDs, file descriptors, memory addresses, and return codes.
2. **Kernel Syscall Tracing (`strace`)**:
   - Live tracing of register dispatch, interrupt vectors, and system call traps directly from the Linux kernel.
3. **In-Browser C Source Code Editor**:
   - Inspect well-documented C source code for all demonstrations.
   - Modify code directly in the browser and re-compile on the fly.
4. **Interactive Linux Terminal**:
   - Dark-mode, monospace terminal with support for ANSI color codes.
   - Dedicated tabs for **STDOUT**, **STDERR**, **GCC Compilation Log**, and **Kernel Syscalls (strace)**.
   - Measures real execution duration down to the millisecond.
5. **Interactive Architecture Visualizers**:
   - **User Mode (Ring 3) vs Kernel Mode (Ring 0)** transition step sequence.
   - **Virtual Memory 4 KB Paging & Address Translation Engine** with live bitmask splitting (`VPN` + `Offset`) and Physical Frame Number calculation.
   - **Process Hierarchy Trees** (`Grandparent` → `Parent` → `Child`).
6. **Course Outcome Aligned**:
   - Organized into 6 distinct modules (CO1 to CO6) with comprehensive Viva Questions and examiner tips for each unit.

---

## 📚 Curriculum & Course Outcome (CO) Mapping

| Course Outcome | Subsystem | System Calls & Concepts Demonstrated | C Source Files |
| :--- | :--- | :--- | :--- |
| **CO1** | **Operating System Architecture** | Kernel Mode vs User Mode, Trap Mechanisms, CPU Registers, `syscall` opcode, Shell as a User-Space Program | `user_kernel_mode.c`<br>`syscall_flow.c`<br>`mini_shell.c` |
| **CO2** | **Process Management** | `fork()`, `execvp()`, `wait()`, `waitpid()`, `exit()`, `getpid()`, `getppid()`, Process Trees, Zombie Reaping | `fork_demo.c`<br>`exec_demo.c`<br>`wait_exit.c`<br>`getpid_ppid.c`<br>`process_tree.c` |
| **CO3** | **Inter-Process Communication (IPC)** | Anonymous Pipes (`pipe()`), Named Pipes (`mkfifo()`), Unix Domain Sockets (`AF_UNIX`), Signals (`signal`, `kill`, `raise`, `alarm`), System V Shared Memory (`shmget`, `shmat`, `shmdt`, `shmctl`) | `pipe_anonymous.c`<br>`fifo_named_pipe.c`<br>`unix_sockets.c`<br>`signals_demo.c`<br>`shared_memory.c` |
| **CO4** | **Virtual Memory Management** | Memory Mapping (`mmap()`, `munmap()`, `msync()`), Copy-on-Write (`COW`), 4 KB Paging, Address Translation (`VPN` + `Offset`), TLB Hit/Miss simulation, Demand Paging & Minor Page Faults (`ru_minflt`) | `mmap_demo.c`<br>`copy_on_write.c`<br>`paging_translation.c`<br>`demand_paging.c` |
| **CO5** | **File Management** | Low-level file I/O (`open()`, `read()`, `write()`, `close()`), Random Access (`lseek()`), Inode Metadata (`stat()`, `fstat()`), File Deletion Semantics (`unlink()`) | `file_crud.c`<br>`lseek_demo.c`<br>`stat_inode.c`<br>`unlink_demo.c` |
| **CO6** | **Thread Management & Synchronization** | POSIX Threads (`pthread_create()`, `pthread_join()`), Race Conditions vs Mutex Locks (`pthread_mutex_lock/unlock`), Condition Variables (`pthread_cond_wait/signal`), Counting Semaphores (`sem_wait/post`), Safe Deadlock Detection & Lock Ordering Prevention | `pthread_basic.c`<br>`mutex_sync.c`<br>`cond_var.c`<br>`counting_semaphore.c`<br>`deadlock_simulation.c` |

---

## 🛠️ Technology Stack

- **Operating System Execution**: Ubuntu Linux 24.04+ (via WSL2 or native Linux host)
- **C Compiler**: GCC 15 with `-Wall -Wextra -pthread -lrt`
- **Low-Level Syscall Tracing**: `strace`
- **Backend**: Python 3.10+ with Flask & Flask-CORS
- **Frontend**: HTML5, Vanilla CSS3 (Custom Glassmorphism Design System), Modern Vanilla JavaScript (ES6+)
- **Typography**: Google Fonts (*Outfit* for UI headings, *JetBrains Mono* for code/terminals)

---

## 📂 Project Structure

```
os-syscall-simulator/
├── backend/
│   ├── app.py                     # Flask REST API server & GCC runner
│   ├── metadata.py                # CO1-CO6 syllabus catalog, explanations, viva bank
│   ├── validate_c_files.py        # Automated test compiler for all C sources
│   └── c_programs/
│       ├── co1_arch/              # CO1 Architecture (Kernel/User mode, traps, shell)
│       ├── co2_process/           # CO2 Process management (fork, exec, wait, exit)
│       ├── co3_ipc/               # CO3 Inter-process communication (pipes, sockets, shm, signals)
│       ├── co4_vm/                # CO4 Virtual memory (mmap, COW, paging, demand paging)
│       ├── co5_file/              # CO5 File management (open, read, write, lseek, stat, unlink)
│       └── co6_threads/           # CO6 Threads & synchronization (pthread, mutex, semaphores, deadlocks)
├── frontend/
│   ├── index.html                 # Interactive Single Page Application
│   ├── css/
│   │   └── style.css              # Cybernetic slate & glassmorphism stylesheet
│   └── js/
│       ├── app.js                 # Frontend application controller & state
│       └── diagrams.js            # SVG/Canvas diagrams & Memory Translation Engine
└── README.md                      # Comprehensive project documentation
```

---

## 🚀 How to Run Locally

### Prerequisites
1. **Ubuntu Linux (or Windows with WSL2 Ubuntu)**:
   ```bash
   wsl --list --verbose
   ```
2. **GCC Compiler & strace installed on Ubuntu**:
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential gcc strace
   ```
3. **Python 3 with Flask installed**:
   ```bash
   pip install flask flask-cors
   ```

### Starting the Simulator

1. Navigate to the backend directory:
   ```bash
   cd os-syscall-simulator/backend
   ```

2. Start the Flask server:
   ```bash
   python app.py
   ```

3. Open your browser and navigate to:
   ```
   http://localhost:5000
   ```

---

## 🧪 Validating All 26 C Programs

To verify that all 26 C programs compile cleanly without errors or missing headers on Ubuntu GCC, run:
```bash
python backend/validate_c_files.py
```
Expected output:
```
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

## 🎓 Operating Systems Viva Examination Guide

Each module in the simulator comes equipped with curated viva questions. Key topics covered include:

- **CO1**: Dual-Mode CPU execution (Ring 3 vs Ring 0), Interrupt Descriptor Table, MSR_LSTAR register, how `write()` translates to CPU opcode.
- **CO2**: Return values of `fork()`, why `execvp()` never returns on success, Zombie vs Orphan processes, `WIFEXITED` status macros.
- **CO3**: Pipe capacity (64 KB), closing unused descriptors to trigger EOF, Named Pipes vs Anonymous Pipes, Shared Memory zero-copy advantages, why SIGKILL and SIGSTOP cannot be caught.
- **CO4**: 4 KB Paging rationale, Virtual Page Number vs Page Offset calculation, TLB hit latency (~1ns) vs page walk (~50ns), Copy-on-Write mechanism during `fork()`, Minor vs Major Page Faults.
- **CO5**: File Descriptors 0, 1, 2, `open()` vs `fopen()` (system call vs buffered stdio), why Inodes do not store filenames, `unlink()` deferred block deallocation.
- **CO6**: Shared vs Private thread resources, Race condition 3-step cycle (Read-Modify-Write), 3 requirements for Critical Section (Mutual Exclusion, Progress, Bounded Waiting), Coffman's 4 conditions for Deadlock, and Lock Ordering prevention.
