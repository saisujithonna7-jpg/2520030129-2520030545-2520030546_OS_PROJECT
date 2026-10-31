#!/bin/bash
# ==============================================================================
# run_ubuntu.sh
# Interactive Operating System System Calls Simulator - Pure C Edition
# Zero Python - 100% C compiled natively with GCC on Ubuntu Linux
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# 1. Check GCC
if ! command -v gcc &> /dev/null; then
    echo "[!] GCC not found. Installing build-essential..."
    sudo apt-get update && sudo apt-get install -y build-essential
fi

# 2. Check strace
if ! command -v strace &> /dev/null; then
    echo "[!] strace not found. Installing strace..."
    sudo apt-get update && sudo apt-get install -y strace
fi

# 3. Compile Master C Application
echo "[*] Compiling os_simulator.c with GCC..."
gcc -Wall -Wextra -pthread -O2 os_simulator.c -o os_simulator -lrt

echo "[✓] Compilation successful."
echo "[*] Launching Master Interactive C OS Simulator..."
echo "======================================================================"

exec ./os_simulator "$@"
