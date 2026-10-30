/*
 * ============================================================================
 * Course Outcome 3: Inter-Process Communication (IPC)
 * Program: unix_sockets.c
 * Purpose: Demonstrates Unix Domain Sockets (AF_UNIX, SOCK_STREAM) for
 *          high-performance bidirectional local inter-process communication.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#define SOCKET_PATH "/tmp/demo_unix_sock"

int main(int argc, char *argv[]) {
    printf("===============================================================\n");
    printf("      CO3: Unix Domain Sockets (AF_UNIX) IPC Demonstration     \n");
    printf("===============================================================\n\n");

    unlink(SOCKET_PATH);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // Child Process: Client
        sleep(1); // Wait for server to bind & listen
        printf("\033[1;36m[CLIENT (PID %d)]\033[0m Creating AF_UNIX stream socket...\n", getpid());
        int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_fd < 0) {
            perror("Client socket creation failed");
            exit(1);
        }

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        printf("\033[1;36m[CLIENT]\033[0m Connecting to Unix Socket at '%s'...\n", SOCKET_PATH);
        if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Client connect error");
            exit(1);
        }

        const char *request = "GET /system-kernel-status HTTP/1.0";
        printf("\033[1;36m[CLIENT]\033[0m Sending payload: \"%s\"\n", request);
        send(client_fd, request, strlen(request), 0);

        char response[256];
        ssize_t bytes = recv(client_fd, response, sizeof(response) - 1, 0);
        if (bytes > 0) {
            response[bytes] = '\0';
            printf("\033[1;36m[CLIENT RECEIVED RESPONSE]:\033[0m \"%s\"\n", response);
        }

        close(client_fd);
        exit(0);
    } else {
        // Parent Process: Server
        printf("\033[1;33m[SERVER (PID %d)]\033[0m Creating socket(AF_UNIX, SOCK_STREAM, 0)...\n", getpid());
        int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0) {
            perror("Server socket error");
            exit(1);
        }

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        printf("\033[1;33m[SERVER]\033[0m Binding to path '%s'...\n", SOCKET_PATH);
        if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind error");
            exit(1);
        }

        printf("\033[1;33m[SERVER]\033[0m Listening with backlog queue = 5...\n");
        listen(server_fd, 5);

        printf("\033[1;33m[SERVER]\033[0m Calling accept() to await client connection...\n");
        int client_sock = accept(server_fd, NULL, NULL);
        if (client_sock < 0) {
            perror("accept error");
            exit(1);
        }

        char buffer[256];
        ssize_t bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("\033[1;33m[SERVER RECEIVED REQUEST]:\033[0m \"%s\"\n", buffer);
        }

        const char *ack = "200 OK: Linux Kernel Handshake Successful";
        printf("\033[1;33m[SERVER]\033[0m Replying to client...\n");
        send(client_sock, ack, strlen(ack), 0);

        close(client_sock);
        close(server_fd);
        unlink(SOCKET_PATH);

        wait(NULL);
        printf("\n\033[1;32m[SUCCESS]\033[0m Full duplex Unix Domain Socket transaction completed.\n");
    }

    return 0;
}
