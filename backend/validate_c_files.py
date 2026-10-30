import subprocess
import os
import glob

def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    programs_dir = os.path.join(base_dir, "c_programs")
    c_files = glob.glob(os.path.join(programs_dir, "**", "*.c"), recursive=True)
    print(f"Total C files found: {len(c_files)}")

    success_count = 0
    fail_count = 0

    for c_file in sorted(c_files):
        rel_name = os.path.relpath(c_file, programs_dir).replace("\\", "/")
        # Convert Windows path to WSL /mnt/c/...
        drive, rest = os.path.splitdrive(c_file)
        wsl_path = f"/mnt/{drive[0].lower()}{rest.replace(os.sep, '/')}"
        
        # Test compile
        compile_cmd = f"gcc -Wall -Wextra -pthread '{wsl_path}' -o /tmp/val_bin"
        res = subprocess.run(["wsl", "-d", "Ubuntu", "bash", "-c", compile_cmd], capture_output=True, text=True)
        if res.returncode == 0:
            print(f"[OK] {rel_name}")
            success_count += 1
        else:
            print(f"[FAIL] {rel_name}")
            print(res.stderr)
            fail_count += 1

    print(f"\nValidation Summary: {success_count} passed, {fail_count} failed.")

if __name__ == "__main__":
    main()
