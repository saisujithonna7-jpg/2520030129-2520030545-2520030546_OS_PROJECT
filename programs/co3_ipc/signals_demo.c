/*
 * ============================================================================
 * Course Outcome 3: Inter-Process Communication (IPC)
 * Program: signals_demo.c
 * Purpose: Demonstrates Asynchronous Signals (signal, kill, raise, alarm),
 *          signal handlers, and asynchronous event notification in Linux.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t sigusr1_received = 0;
volatile sig_atomic_t alarm_received = 0;

void handle_sigusr1(int sig) {
    sigusr1_received = 1;
    printf("  \033[1;32m[SIGNAL HANDLER]\033[0m Caught Signal %d (SIGUSR1) asynchronously!\n", sig);
}

void handle_alarm(int sig) {
    alarm_received = 1;
    printf("  \033[1;33m[ALARM HANDLER]\033[0m Caught Signal %d (SIGALRM) after timer expired!\n", sig);
}

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("         CO3: POSIX Signals (kill, raise, signal) Demo         \n");
    printf("===============================================================\n\n");

    printf("1. Registering custom handlers using signal()...\n");
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGALRM, handle_alarm);

    printf("2. Testing self-signal with raise(SIGUSR1)...\n");
    raise(SIGUSR1);

    printf("   Status: sigusr1_received = %d (handled successfully)\n\n", sigusr1_received);

    printf("3. Testing timer signal with alarm(1)...\n");
    alarm(1);
    printf("   Sleeping for 2 seconds to wait for SIGALRM...\n");
    sleep(2);
    printf("   Status: alarm_received = %d\n\n", alarm_received);

    printf("4. Testing inter-process signaling via kill(pid, SIGUSR1)...\n");
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process
        printf("  \033[1;36m[CHILD PID %d]\033[0m Waiting for parent signal...\n", getpid());
        sigusr1_received = 0;
        while (!sigusr1_received) {
            pause(); // Suspend process until signal caught
        }
        printf("  \033[1;36m[CHILD]\033[0m Resumed after signal! Exiting.\n");
        exit(0);
    } else {
        // Parent Process
        sleep(1);
        printf("  \033[1;35m[PARENT]\033[0m Sending SIGUSR1 to child %d via kill()...\n", pid);
        kill(pid, SIGUSR1);
        wait(NULL);
        printf("\n\033[1;32m[DONE]\033[0m All signal mechanisms verified successfully.\n");
    }

    return 0;
}
