"""
app.py
Interactive Operating System System Calls Simulator - Backend Server.
Supports both native Ubuntu Linux execution (when run inside Ubuntu/WSL)
and Windows host execution (bridging into Ubuntu via WSL).
"""

import os
import sys
import time
import uuid
import subprocess
from flask import Flask, jsonify, request, send_from_directory
from flask_cors import CORS
from metadata import MODULES_DATA

# Environment detection
IS_LINUX = sys.platform.startswith("linux")

# Paths configuration
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.abspath(os.path.join(BASE_DIR, "..", "frontend"))
C_PROGRAMS_DIR = os.path.join(BASE_DIR, "c_programs")

app = Flask(__name__, static_folder=FRONTEND_DIR, static_url_path="")
CORS(app)

def to_linux_path(path: str) -> str:
    """Converts a filepath to a valid Linux path (handling Windows drive mounts if needed)."""
    if IS_LINUX:
        return os.path.abspath(path)
    drive, rest = os.path.splitdrive(os.path.abspath(path))
    if drive:
        return f"/mnt/{drive[0].lower()}{rest.replace(os.sep, '/')}"
    return path.replace(os.sep, '/')

def run_command(command_str: str, input_str: str = None, timeout_sec: int = 8):
    """
    Executes a shell command either natively in Linux or via WSL if invoked on Windows.
    Enforces process timeout and safely captures stdout/stderr.
    """
    if IS_LINUX:
        exec_args = ["bash", "-c", command_str]
    else:
        exec_args = ["wsl", "-d", "Ubuntu", "bash", "-c", command_str]

    try:
        proc = subprocess.run(
            exec_args,
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
            "stderr": (e.stderr or "") + "\n\033[1;31m[EXECUTION TIMEOUT]\033[0m Program exceeded allowed time limit (8s). Terminated.",
            "timed_out": True
        }
    except Exception as e:
        return {
            "success": False,
            "exit_code": -1,
            "stdout": "",
            "stderr": f"Subprocess Error: {str(e)}",
            "timed_out": False
        }

@app.route("/")
def index():
    return send_from_directory(FRONTEND_DIR, "index.html")

@app.route("/api/system-info", methods=["GET"])
def get_system_info():
    """Queries Ubuntu Linux kernel and runtime environment details."""
    cmd = (
        "echo '===KERNEL===' && uname -srmo && "
        "echo '===GCC===' && gcc --version | head -n 1 && "
        "echo '===MEM===' && free -h | grep Mem && "
        "echo '===UPTIME===' && uptime -p"
    )
    res = run_command(cmd, timeout_sec=5)

    info = {
        "os_name": "Ubuntu Linux",
        "kernel": "Linux Native",
        "gcc_version": "gcc 15.2.0",
        "memory": "Active",
        "uptime": "Active",
        "is_native_linux": IS_LINUX,
        "raw": res["stdout"]
    }

    if res["success"]:
        parts = res["stdout"].split("===")
        for i in range(1, len(parts), 2):
            key = parts[i].strip()
            val = parts[i+1].strip() if i+1 < len(parts) else ""
            if key == "KERNEL": info["kernel"] = val
            elif key == "GCC": info["gcc_version"] = val
            elif key == "MEM": info["memory"] = val
            elif key == "UPTIME": info["uptime"] = val

    return jsonify(info)

@app.route("/api/modules", methods=["GET"])
def get_modules():
    """Returns complete course outcomes modules catalog."""
    return jsonify(MODULES_DATA)

@app.route("/api/demo/<demo_id>", methods=["GET"])
def get_demo(demo_id):
    """Retrieves metadata and C source code for a specific demonstration."""
    for mod in MODULES_DATA:
        for demo in mod["demos"]:
            if demo["id"] == demo_id:
                c_file_path = os.path.join(C_PROGRAMS_DIR, demo["file"])
                code_content = ""
                if os.path.exists(c_file_path):
                    with open(c_file_path, "r", encoding="utf-8") as f:
                        code_content = f.read()
                return jsonify({
                    "demo": demo,
                    "module": {
                        "id": mod["id"],
                        "code": mod["code"],
                        "title": mod["title"]
                    },
                    "code": code_content
                })
    return jsonify({"error": "Demonstration not found"}), 404

@app.route("/api/execute", methods=["POST"])
def execute_demo():
    """
    Compiles and executes a C demonstration program on Ubuntu Linux with GCC.
    Supports strace system call tracing and custom input piping.
    """
    data = request.get_json() or {}
    demo_id = data.get("demo_id")
    custom_code = data.get("custom_code")
    args_str = data.get("args", "").strip()
    stdin_str = data.get("stdin", "")
    use_strace = bool(data.get("use_strace", False))

    run_id = uuid.uuid4().hex[:8]
    temp_source_linux = f"/tmp/os_run_{run_id}.c"
    temp_binary_linux = f"/tmp/os_bin_{run_id}"

    # Step 1: Prepare Source Code in Linux
    if custom_code:
        write_cmd = f"cat << 'EOF_OS_CODE' > {temp_source_linux}\n{custom_code}\nEOF_OS_CODE"
        write_res = run_command(write_cmd, timeout_sec=4)
        if not write_res["success"]:
            return jsonify({
                "success": False,
                "error": "Failed to create temporary C source in Ubuntu",
                "stderr": write_res["stderr"]
            }), 500
    else:
        target_file = None
        for mod in MODULES_DATA:
            for demo in mod["demos"]:
                if demo["id"] == demo_id:
                    target_file = os.path.join(C_PROGRAMS_DIR, demo["file"])
                    break
            if target_file:
                break

        if not target_file or not os.path.exists(target_file):
            return jsonify({"success": False, "error": f"Demo '{demo_id}' not found."}), 404

        source_linux = to_linux_path(target_file)
        copy_cmd = f"cp '{source_linux}' {temp_source_linux}"
        run_command(copy_cmd, timeout_sec=4)

    # Step 2: Compile with GCC on Ubuntu
    compile_cmd = f"gcc -Wall -Wextra -pthread {temp_source_linux} -o {temp_binary_linux}"
    compile_res = run_command(compile_cmd, timeout_sec=6)

    if not compile_res["success"] or compile_res["exit_code"] != 0:
        cleanup_cmd = f"rm -f {temp_source_linux} {temp_binary_linux}"
        run_command(cleanup_cmd, timeout_sec=2)
        return jsonify({
            "success": False,
            "phase": "compilation",
            "compile_output": compile_res["stderr"] or compile_res["stdout"],
            "stdout": "",
            "stderr": "GCC Compilation Failed. Inspect compile diagnostics above.",
            "exit_code": compile_res["exit_code"],
            "execution_time_ms": 0
        })

    compile_log = compile_res["stderr"] if compile_res["stderr"] else "GCC compilation succeeded with 0 warnings."

    # Step 3: Execute the compiled binary on Ubuntu
    start_time = time.perf_counter()

    if use_strace:
        exec_cmd = (
            f"strace -f -s 80 -e trace=process,ipc,desc,memory,signal "
            f"{temp_binary_linux} {args_str}"
        )
    else:
        exec_cmd = f"{temp_binary_linux} {args_str}"

    exec_res = run_command(exec_cmd, input_str=stdin_str if stdin_str else None, timeout_sec=10)
    elapsed_ms = round((time.perf_counter() - start_time) * 1000, 2)

    # Step 4: Cleanup temporary artifacts
    cleanup_cmd = f"rm -f {temp_source_linux} {temp_binary_linux}"
    run_command(cleanup_cmd, timeout_sec=2)

    strace_output = ""
    stdout_output = exec_res["stdout"]
    stderr_output = exec_res["stderr"]

    if use_strace:
        strace_output = stderr_output
        stderr_output = ""

    return jsonify({
        "success": exec_res["success"],
        "phase": "execution",
        "compile_output": compile_log,
        "stdout": stdout_output,
        "stderr": stderr_output,
        "strace_output": strace_output,
        "exit_code": exec_res["exit_code"],
        "execution_time_ms": elapsed_ms,
        "timed_out": exec_res["timed_out"]
    })

@app.route("/api/explorer", methods=["GET"])
def get_syscall_explorer():
    """Aggregates all unique system calls across the curriculum for quick reference."""
    explorer_map = {}
    for mod in MODULES_DATA:
        for demo in mod["demos"]:
            for sc in demo["syscalls"]:
                if sc not in explorer_map:
                    explorer_map[sc] = {
                        "name": sc,
                        "co": mod["code"],
                        "module_title": mod["title"],
                        "related_demos": []
                    }
                explorer_map[sc]["related_demos"].append({
                    "id": demo["id"],
                    "title": demo["title"]
                })
    return jsonify(list(explorer_map.values()))

if __name__ == "__main__":
    mode_str = "Native Ubuntu Linux" if IS_LINUX else "Windows Host (WSL2 Ubuntu Bridged)"
    print(f"===========================================================")
    print(f" OS System Calls Simulator - Server Running on Ubuntu     ")
    print(f" Mode: {mode_str}")
    print(f" URL:  http://localhost:5000                              ")
    print(f"===========================================================")
    app.run(host="0.0.0.0", port=5000, debug=False)
