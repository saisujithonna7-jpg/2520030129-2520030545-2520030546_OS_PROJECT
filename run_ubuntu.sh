#!/bin/bash
# ==============================================================================
# run_ubuntu.sh
# Interactive Operating System System Calls Simulator - Pure Ubuntu CLI Edition
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Checking Ubuntu Linux environment..."

# 1. Ensure GCC is installed
if ! command -v gcc &> /dev/null; then
    echo "[!] GCC not found. Installing build-essential..."
    sudo apt-get update && sudo apt-get install -y build-essential
fi

# 2. Ensure strace is installed
if ! command -v strace &> /dev/null; then
    echo "[!] strace not found. Installing strace..."
    sudo apt-get update && sudo apt-get install -y strace
fi

# 3. Ensure python3 is available
if ! command -v python3 &> /dev/null; then
    echo "[!] python3 not found. Installing python3..."
    sudo apt-get update && sudo apt-get install -y python3
fi

# 4. Launch Pure Ubuntu CLI Simulator
cd "$SCRIPT_DIR"
chmod +x os_simulator.py
exec python3 os_simulator.py "$@"
