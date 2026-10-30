# ==============================================================================
# Makefile for Interactive Operating System System Calls Simulator (Ubuntu Linux)
# Pure C Implementation - Zero External Runtimes
# ==============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
LDFLAGS = -pthread -lrt

TARGET = os_simulator
SRC = os_simulator.c

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "[*] Compiling Master OS Simulator in Pure C..."
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	@echo "[✓] Build complete: ./$(TARGET)"

clean:
	@echo "[*] Cleaning binaries..."
	rm -f $(TARGET) /tmp/os_c_bin_* /tmp/os_cli_bin_*

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "[*] Testing compilation of all 26 C demonstration programs..."
	@for f in programs/*/*.c; do \
		echo -n "Compiling $$f... "; \
		$(CC) $(CFLAGS) "$$f" -o /tmp/test_build $(LDFLAGS) && echo "OK" || exit 1; \
	done
	@rm -f /tmp/test_build
	@echo "[✓] All 26 C programs successfully compiled with GCC!"

.PHONY: all clean run test
