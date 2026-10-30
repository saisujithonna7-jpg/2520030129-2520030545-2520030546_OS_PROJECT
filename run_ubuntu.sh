#!/bin/bash
# ==============================================================================
# run_ubuntu.sh
# Interactive Operating System System Calls Simulator - Ubuntu Linux Runner
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$SCRIPT_DIR/backend"

echo "======================================================================"
echo "    Starting OS System Calls Simulator natively on Ubuntu Linux       "
echo "======================================================================"
echo "Directory: $SCRIPT_DIR"
echo "Kernel:    $(uname -srmo)"
echo "GCC:       $(gcc --version | head -n 1)"
echo "Python:    $(python3 --version)"
echo "----------------------------------------------------------------------"

# Ensure required packages exist
if ! command -v gcc &> /dev/null; then
    echo "[!] GCC not found. Installing build-essential..."
    sudo apt-get update && sudo apt-get install -y build-essential
fi

if ! command -v strace &> /dev/null; then
    echo "[!] strace not found. Installing strace..."
    sudo apt-get update && sudo apt-get install -y strace
fi

# Check Python Flask dependencies
if ! python3 -c "import flask, flask_cors" &> /dev/null; then
    echo "[!] Installing Flask and Flask-CORS in Ubuntu..."
    sudo apt-get update && sudo apt-get install -y python3-flask python3-flask-cors
fi

echo "[✓] Environment dependencies verified."
echo "[*] Launching Flask server on http://localhost:5000 ..."
echo "----------------------------------------------------------------------"
echo " Open your web browser and go to:"
echo "     http://localhost:5000"
echo " Press CTRL+C to terminate the server."
echo "======================================================================"

cd "$BACKEND_DIR"
exec python3 app.py
