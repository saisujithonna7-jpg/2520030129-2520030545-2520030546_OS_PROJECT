#!/usr/bin/env python3
"""
==============================================================================
Interactive Operating System System Calls Simulator - Pure Ubuntu CLI Terminal
Operating System: Ubuntu Linux
Compiler: GCC with POSIX pthread & realtime extensions
Tracing: strace Linux kernel system call audit
Curriculum: Course Outcomes CO1 through CO6
==============================================================================
"""

import os
import sys
import time
import uuid
import shutil
import subprocess

# Ensure backend directory is in path to import metadata
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(BASE_DIR, "backend"))

try:
    from metadata import MODULES_DATA
except ImportError:
    print("\033[1;31m[ERROR] Failed to load backend/metadata.py.\033[0m")
    sys.exit(1)

# Platform detection
IS_LINUX = sys.platform.startswith("linux")

# ANSI Color Codes
C_RESET = "\033[0m"
C_BOLD = "\033[1m"
C_DIM = "\033[2m"
C_CYAN = "\033[1;36m"
C_GREEN = "\033[1;32m"
C_YELLOW = "\033[1;33m"
C_RED = "\033[1;31m"
C_PURPLE = "\033[1;35m"
C_BLUE = "\033[1;34m"
C_WHITE = "\033[1;37m"

def safe_input(prompt: str) -> str:
    """Helper to safely read user input, handling Ctrl+D / EOF cleanly."""
    try:
        return input(prompt).strip()
    except (EOFError, KeyboardInterrupt):
        print(f"\n{C_YELLOW}Exiting cleanly...{C_RESET}")
        sys.exit(0)

def clear_screen():
    print("\033[2J\033[H", end="")

def print_banner():
    clear_screen()
    width = min(shutil.get_terminal_size((80, 24)).columns, 90)
    print(C_CYAN + "╔" + "═" * (width - 2) + "╗" + C_RESET)
    title = "INTERACTIVE OPERATING SYSTEM SYSTEM CALLS SIMULATOR"
    sub = "Pure Ubuntu Linux Terminal Edition • GCC & strace Engine"
    co_sub = "Curriculum Aligned: Course Outcomes CO1 through CO6"
    print(C_CYAN + "║" + C_BOLD + C_WHITE + title.center(width - 2) + C_CYAN + "║" + C_RESET)
    print(C_CYAN + "║" + C_YELLOW + sub.center(width - 2) + C_CYAN + "║" + C_RESET)
    print(C_CYAN + "║" + C_DIM + C_WHITE + co_sub.center(width - 2) + C_CYAN + "║" + C_RESET)
    print(C_CYAN + "╚" + "═" * (width - 2) + "╝" + C_RESET)

def to_linux_path(rel_or_abs_path: str) -> str:
    """Returns an absolute Linux path for compilation and execution."""
    if os.path.isabs(rel_or_abs_path):
        p = rel_or_abs_path
    else:
        p = os.path.join(BASE_DIR, "backend", "c_programs", rel_or_abs_path)

    if IS_LINUX:
        return os.path.abspath(p)
    drive, rest = os.path.splitdrive(os.path.abspath(p))
    if drive:
        return f"/mnt/{drive[0].lower()}{rest.replace(os.sep, '/')}"
    return p.replace(os.sep, '/')

def run_command(command_str: str, input_str: str = None, timeout_sec: int = 10):
    """Executes a shell command natively on Linux (or via WSL if on Windows)."""
    if IS_LINUX:
        cmd = ["bash", "-c", command_str]
    else:
        cmd = ["wsl", "-d", "Ubuntu", "bash", "-c", command_str]

    try:
        proc = subprocess.run(
            cmd,
            input=input_str,
            capture_output=True,
            text=True,
            timeout=timeout_sec
        )
        return {
            "success": proc.returncode == 0,
            "exit_code": proc.returncode,
            "stdout": proc.stdout,
            "stderr": proc.stderr,
            "timed_out": False
        }
    except subprocess.TimeoutExpired as e:
        return {
            "success": False,
            "exit_code": -1,
            "stdout": e.stdout or "",
            "stderr": (e.stderr or "") + f"\n{C_RED}[TIMEOUT]{C_RESET} Program exceeded {timeout_sec}s limit.",
            "timed_out": True
        }
    except Exception as e:
        return {
            "success": False,
            "exit_code": -1,
            "stdout": "",
            "stderr": f"Execution Error: {str(e)}",
            "timed_out": False
        }

def get_system_badge():
    res = run_command("uname -srmo && gcc --version | head -n 1", timeout_sec=3)
    lines = res["stdout"].strip().splitlines()
    kernel_str = lines[0] if len(lines) > 0 else "Linux 6.x"
    gcc_str = lines[1] if len(lines) > 1 else "GCC 15"
    return f"{C_GREEN}[HOST]{C_RESET} {kernel_str} | {C_CYAN}[COMPILER]{C_RESET} {gcc_str}"

def compile_and_run_c(c_file_rel: str, args_str: str = "", stdin_str: str = "", use_strace: bool = False):
    """Compiles C file with GCC on Ubuntu and executes with real-time output."""
    full_path = to_linux_path(c_file_rel)
    run_id = uuid.uuid4().hex[:6]
    temp_binary = f"/tmp/os_cli_bin_{run_id}"

    print(f"\n{C_CYAN}[1/2] Compiling with Ubuntu GCC:{C_RESET} {C_WHITE}gcc -Wall -Wextra -pthread '{full_path}' -o {temp_binary}{C_RESET}")
    compile_cmd = f"gcc -Wall -Wextra -pthread '{full_path}' -o {temp_binary}"
    comp_res = run_command(compile_cmd, timeout_sec=6)

    if not comp_res["success"] or comp_res["exit_code"] != 0:
        print(f"{C_RED}[FAILED] Compilation Error (Exit {comp_res['exit_code']}):{C_RESET}")
        print(comp_res["stderr"] or comp_res["stdout"])
        return

    print(f"{C_GREEN}[SUCCESS] GCC compilation succeeded (0 errors).{C_RESET}")

    if use_strace:
        print(f"{C_PURPLE}[2/2] Executing with kernel strace audit:{C_RESET} {temp_binary} {args_str}\n")
        exec_cmd = f"strace -f -s 80 -e trace=process,ipc,desc,memory,signal {temp_binary} {args_str}"
    else:
        print(f"{C_GREEN}[2/2] Executing on Ubuntu Linux:{C_RESET} {temp_binary} {args_str}\n")
        exec_cmd = f"{temp_binary} {args_str}"

    print(C_DIM + "─" * 70 + C_RESET)
    start_time = time.perf_counter()
    run_res = run_command(exec_cmd, input_str=stdin_str if stdin_str else None, timeout_sec=12)
    elapsed_ms = (time.perf_counter() - start_time) * 1000
    print(C_DIM + "─" * 70 + C_RESET)

    if run_res["stdout"]:
        print(run_res["stdout"], end="")
    if run_res["stderr"]:
        if use_strace:
            print(f"\n{C_PURPLE}=== Low-Level Kernel Syscall Trace (strace) ==={C_RESET}")
            print(run_res["stderr"], end="")
        else:
            print(f"\n{C_RED}[STDERR]:{C_RESET}\n" + run_res["stderr"], end="")

    code_color = C_GREEN if run_res["exit_code"] == 0 else C_RED
    print(f"\n{C_BOLD}Execution Summary:{C_RESET} Return Code: {code_color}{run_res['exit_code']}{C_RESET} | Duration: {C_CYAN}{elapsed_ms:.2f} ms{C_RESET}")

    run_command(f"rm -f {temp_binary}", timeout_sec=2)

def view_c_source(c_file_rel: str):
    """Displays C source code in terminal with line numbers and syntax formatting."""
    full_path = os.path.join(BASE_DIR, "backend", "c_programs", c_file_rel)
    if not os.path.exists(full_path):
        print(f"{C_RED}[ERROR] Source file not found: {full_path}{C_RESET}")
        return

    print(f"\n{C_CYAN}=== C SOURCE CODE: {c_file_rel} ==={C_RESET}\n")
    with open(full_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for idx, line in enumerate(lines, 1):
        line_clean = line.rstrip()
        if line_clean.strip().startswith("//") or line_clean.strip().startswith("/*") or line_clean.strip().startswith("*"):
            print(f"{C_DIM}{idx:4d} | {C_GREEN}{line_clean}{C_RESET}")
        elif line_clean.strip().startswith("#"):
            print(f"{C_DIM}{idx:4d} | {C_PURPLE}{line_clean}{C_RESET}")
        else:
            print(f"{C_DIM}{idx:4d} | {C_WHITE}{line_clean}{C_RESET}")
    print(C_CYAN + "═" * 70 + C_RESET)

def run_address_translation_engine():
    """Interactive 4 KB Paging & Virtual Address Translation Sandbox."""
    print_banner()
    print(f"\n{C_BOLD}{C_CYAN}─── INTERACTIVE VIRTUAL MEMORY ADDRESS TRANSLATION SANDBOX ───{C_RESET}")
    print("Architecture: Standard 4 KB Paging (12-bit Page Offset, 52-bit VPN)")
    print("Formula: Physical Address = (Physical Frame Number << 12) | Offset\n")

    while True:
        val = safe_input(f"{C_BOLD}Enter Virtual Address (Hex like '0x7ffd1234abcd' or Dec, or 'b' for back): {C_RESET}")
        if val.lower() == 'b':
            break
        if not val:
            val = "0x7ffd1234abcd"

        try:
            addr = int(val, 0)
            offset = addr & 0xFFF
            vpn = addr >> 12
            pfn = (vpn * 13 + 0x2A1) & 0xFFFFFF
            phys = (pfn << 12) | offset

            print(C_CYAN + "┌" + "─" * 60 + "┐" + C_RESET)
            print(f"│ {C_BOLD}Input Virtual Address:{C_RESET}      {C_WHITE}0x{addr:012x}{C_RESET} ({addr})")
            print(f"│ {C_CYAN}Virtual Page Number (VPN):{C_RESET}  {C_CYAN}0x{vpn:x}{C_RESET} (Page Table Index)")
            print(f"│ {C_PURPLE}Page Offset (12 bits):{C_RESET}      {C_PURPLE}0x{offset:03x}{C_RESET} ({offset} bytes in 4KB page)")
            print(f"│ {C_YELLOW}Simulated PFN (Frame):{C_RESET}      {C_YELLOW}0x{pfn:x}{C_RESET}")
            print(f"│ {C_GREEN}Calculated Physical RAM:{C_RESET}    {C_GREEN}0x{phys:012x}{C_RESET}")
            print(C_CYAN + "└" + "─" * 60 + "┘" + C_RESET)
            print()
        except ValueError:
            print(f"{C_RED}Invalid numeric address format. Please enter hex (0x...) or decimal.{C_RESET}\n")

def show_viva_exam_bank():
    """Interactive Viva Examination Question Bank & Flashcards."""
    print_banner()
    print(f"\n{C_BOLD}{C_CYAN}─── OPERATING SYSTEMS VIVA EXAMINATION QUESTION BANK ───{C_RESET}")
    print("Select a Course Outcome module to browse viva questions, or 'A' for all.\n")

    for i, mod in enumerate(MODULES_DATA, 1):
        print(f" [{i}] {mod['code']}: {mod['title']}")
    print(" [A] Browse All Course Outcomes Questions")
    print(" [0] Return to Main Menu")

    choice = safe_input(f"\n{C_BOLD}Select Option: {C_RESET}").upper()
    if choice == '0':
        return

    selected_mods = MODULES_DATA if choice == 'A' else [MODULES_DATA[int(choice) - 1]] if choice in [str(x) for x in range(1, 7)] else []

    if not selected_mods:
        print(f"{C_RED}Invalid option.{C_RESET}")
        time.sleep(1)
        return

    q_count = 1
    for mod in selected_mods:
        print(f"\n{C_BOLD}{C_PURPLE}══════════ {mod['code']}: {mod['title']} ══════════{C_RESET}")
        for demo in mod["demos"]:
            for vq in demo.get("viva_questions", []):
                print(f"\n{C_BOLD}{C_CYAN}Q{q_count} [{demo['title']}]: {vq['q']}{C_RESET}")
                safe_input(f"{C_DIM}(Press Enter to reveal examiner answer...){C_RESET}")
                print(f"{C_GREEN}Answer:{C_RESET} {vq['a']}")
                if vq.get("tip"):
                    print(f"{C_YELLOW}💡 Viva Tip:{C_RESET} {vq['tip']}")
                print(C_DIM + "─" * 60 + C_RESET)
                q_count += 1

    safe_input(f"\n{C_DIM}Press Enter to return to menu...{C_RESET}")

def search_syscall_explorer():
    """Searchable directory of POSIX system calls across CO1 to CO6."""
    print_banner()
    print(f"\n{C_BOLD}{C_CYAN}─── POSIX SYSTEM CALL EXPLORER DIRECTORY ───{C_RESET}\n")
    query = safe_input(f"{C_BOLD}Enter system call name or keyword (e.g. fork, mmap, pipe, stat, CO2): {C_RESET}").lower()

    matches = []
    for mod in MODULES_DATA:
        for demo in mod["demos"]:
            for sc in demo["syscalls"]:
                if not query or query in sc.lower() or query in mod["code"].lower() or query in demo["title"].lower():
                    matches.append((sc, mod["code"], demo))

    if not matches:
        print(f"{C_RED}No matching system calls found for '{query}'.{C_RESET}")
    else:
        print(f"\nFound {len(matches)} matching entries:\n")
        print(f"{C_BOLD}{'System Call':<16} {'CO':<6} {'Demonstration Title':<38} {'File'}{C_RESET}")
        print(C_CYAN + "─" * 80 + C_RESET)
        for sc, co, demo in matches:
            print(f"{C_GREEN}{sc + '()':<16}{C_RESET} {C_YELLOW}{co:<6}{C_RESET} {C_WHITE}{demo['title']:<38}{C_RESET} {C_DIM}{demo['file']}{C_RESET}")

    safe_input(f"\n{C_DIM}Press Enter to return...{C_RESET}")

def demo_action_menu(demo, mod):
    """Action menu for an individual C demonstration."""
    while True:
        print_banner()
        print(f"{C_BOLD}{C_YELLOW}Module:{C_RESET} {mod['code']} — {mod['title']}")
        print(f"{C_BOLD}{C_CYAN}Demonstration:{C_RESET} {demo['title']} [{demo['difficulty']}]")
        print(f"{C_BOLD}System Calls:{C_RESET}  {', '.join([s + '()' for s in demo['syscalls']])}")
        print(f"{C_BOLD}Source File:{C_RESET}   backend/c_programs/{demo['file']}")
        print(f"\n{C_WHITE}{demo['summary']}{C_RESET}\n")

        print(C_CYAN + "┌" + "─" * 58 + "┐" + C_RESET)
        print(f"│  {C_BOLD}[1] ▶ Compile & Run natively on Ubuntu with GCC{C_RESET}        │")
        print(f"│  {C_BOLD}[2] ⚡ Trace Kernel Traps with strace (Audit Syscalls){C_RESET} │")
        print(f"│  {C_BOLD}[3] ⚙ Run with Custom Arguments (argv) / stdin{C_RESET}         │")
        print(f"│  {C_BOLD}[4] 📄 View C Source Code with Syntax Highlighting{C_RESET}     │")
        print(f"│  {C_BOLD}[5] 🧠 Kernel Deep-Dive: Internals & Data Structures{C_RESET}   │")
        print(f"│  {C_BOLD}[6] 🔄 Step-by-Step Execution Flow Sequence{C_RESET}            │")
        print(f"│  {C_BOLD}[7] ❓ Viva Questions & Answers for this Demonstration{C_RESET} │")
        print(f"│  {C_BOLD}[0] ⬅ Back to Module Demonstrations{C_RESET}                    │")
        print(C_CYAN + "└" + "─" * 58 + "┘" + C_RESET)

        choice = safe_input(f"\n{C_BOLD}Select Action [0-7]: {C_RESET}")

        if choice == '1':
            compile_and_run_c(demo['file'], use_strace=False)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '2':
            compile_and_run_c(demo['file'], use_strace=True)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '3':
            args = safe_input(f"{C_BOLD}Enter command line arguments (argv): {C_RESET}")
            stdin_data = safe_input(f"{C_BOLD}Enter standard input (stdin, or press Enter): {C_RESET}")
            compile_and_run_c(demo['file'], args_str=args, stdin_str=stdin_data, use_strace=False)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '4':
            view_c_source(demo['file'])
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '5':
            print(f"\n{C_BOLD}{C_CYAN}=== KERNEL INTERNAL EXPLANATION & ARCHITECTURE ==={C_RESET}\n")
            print(demo['explanation'])
            print(C_CYAN + "─" * 70 + C_RESET)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '6':
            print(f"\n{C_BOLD}{C_CYAN}=== STEP-BY-STEP KERNEL EXECUTION FLOW ==={C_RESET}\n")
            for step in demo.get('kernel_flow', []):
                actor_color = C_PURPLE if 'kernel' in step['actor'].lower() or 'ring 0' in step['actor'].lower() else C_CYAN
                print(f" [{step['step']}] {actor_color}{step['actor']}:{C_RESET} {step['action']}")
            print(C_CYAN + "─" * 70 + C_RESET)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '7':
            print(f"\n{C_BOLD}{C_CYAN}=== VIVA QUESTIONS FOR {demo['title'].upper()} ==={C_RESET}\n")
            for idx, vq in enumerate(demo.get('viva_questions', []), 1):
                print(f"{C_BOLD}Q{idx}: {vq['q']}{C_RESET}")
                print(f"{C_GREEN}Answer:{C_RESET} {vq['a']}")
                if vq.get('tip'):
                    print(f"{C_YELLOW}💡 Examiner Tip:{C_RESET} {vq['tip']}")
                print(C_DIM + "─" * 60 + C_RESET)
            safe_input(f"\n{C_DIM}Press Enter to continue...{C_RESET}")
        elif choice == '0':
            break

def module_menu(mod):
    """Menu listing demonstrations for a selected Course Outcome."""
    while True:
        print_banner()
        print(f"{C_BOLD}{C_PURPLE}Course Outcome:{C_RESET} {mod['code']} — {mod['title']}")
        print(f"{C_DIM}{mod['summary']}{C_RESET}\n")

        print(f"{C_BOLD}Demonstrations in this Module:{C_RESET}")
        for idx, demo in enumerate(mod['demos'], 1):
            syscalls = ", ".join(demo['syscalls'])
            diff_color = C_GREEN if demo['difficulty'] == 'Beginner' else C_YELLOW if demo['difficulty'] == 'Intermediate' else C_RED
            print(f" [{idx}] {C_BOLD}{demo['title']:<38}{C_RESET} {diff_color}[{demo['difficulty']}]{C_RESET} {C_DIM}({syscalls}){C_RESET}")

        print(f" [0] Return to Main Menu")

        choice = safe_input(f"\n{C_BOLD}Select Demonstration [1-{len(mod['demos'])}] or 0: {C_RESET}")
        if choice == '0':
            break

        try:
            demo_idx = int(choice) - 1
            if 0 <= demo_idx < len(mod['demos']):
                demo_action_menu(mod['demos'][demo_idx], mod)
            else:
                print(f"{C_RED}Invalid option.{C_RESET}")
                time.sleep(1)
        except ValueError:
            print(f"{C_RED}Please enter a number.{C_RESET}")
            time.sleep(1)

def main():
    while True:
        print_banner()
        print(get_system_badge())
        print(C_CYAN + "═" * 70 + C_RESET)
        print(f"{C_BOLD}Select Course Outcome Module or Lab Tool:{C_RESET}\n")

        print(f"  {C_CYAN}[1]{C_RESET} {C_BOLD}CO1: Operating System Architecture{C_RESET}")
        print(f"      {C_DIM}Kernel vs User Mode (Ring 3/0), Syscall Flow, Shell Architecture{C_RESET}")

        print(f"  {C_CYAN}[2]{C_RESET} {C_BOLD}CO2: Process Management{C_RESET}")
        print(f"      {C_DIM}fork(), execvp(), wait(), exit(), getpid(), getppid(), Process Trees{C_RESET}")

        print(f"  {C_CYAN}[3]{C_RESET} {C_BOLD}CO3: Inter-Process Communication (IPC){C_RESET}")
        print(f"      {C_DIM}Anonymous Pipes, Named Pipes (FIFO), Unix Sockets, Signals, Shared Memory{C_RESET}")

        print(f"  {C_CYAN}[4]{C_RESET} {C_BOLD}CO4: Virtual Memory Management{C_RESET}")
        print(f"      {C_DIM}Paging, Address Translation, TLB, Page Faults, COW, mmap/munmap{C_RESET}")

        print(f"  {C_CYAN}[5]{C_RESET} {C_BOLD}CO5: File Management{C_RESET}")
        print(f"      {C_DIM}open(), read(), write(), close(), lseek(), stat(), unlink(){C_RESET}")

        print(f"  {C_CYAN}[6]{C_RESET} {C_BOLD}CO6: Thread Management & Synchronization{C_RESET}")
        print(f"      {C_DIM}pthreads, Mutex Locks, Condition Variables, Semaphores, Deadlocks{C_RESET}")

        print(C_DIM + "  " + "─" * 66 + C_RESET)
        print(f"  {C_YELLOW}[7]{C_RESET} {C_BOLD}POSIX System Call Explorer (Search & Reference){C_RESET}")
        print(f"  {C_YELLOW}[8]{C_RESET} {C_BOLD}Interactive 4 KB Virtual Memory Address Translation Sandbox{C_RESET}")
        print(f"  {C_YELLOW}[9]{C_RESET} {C_BOLD}Operating Systems Viva Examination Question Bank & Quiz{C_RESET}")
        print(f"  {C_RED}[0]{C_RESET} {C_BOLD}Exit Simulator{C_RESET}")

        choice = safe_input(f"\n{C_BOLD}Enter choice [0-9]: {C_RESET}")

        if choice in ['1', '2', '3', '4', '5', '6']:
            module_menu(MODULES_DATA[int(choice) - 1])
        elif choice == '7':
            search_syscall_explorer()
        elif choice == '8':
            run_address_translation_engine()
        elif choice == '9':
            show_viva_exam_bank()
        elif choice == '0':
            print(f"\n{C_GREEN}Exiting OS Simulator. Happy Learning!{C_RESET}\n")
            break
        else:
            print(f"{C_RED}Invalid option. Please choose [0-9].{C_RESET}")
            time.sleep(1)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt, EOFError):
        print(f"\n\n{C_YELLOW}Session closed. Exited safely.{C_RESET}\n")
