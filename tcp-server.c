#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define IP "0.0.0.0"
#define PORT 9998
#define BACKLOG 5
#define BUFFER_SIZE 1024

void *handle_client(void *client_socket);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t thread_id;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[*] Listening on %s:%d\n", IP, PORT);

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("[*] Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_fd) != 0) {
            perror("pthread_create failed");
        }
    }

    close(server_fd);
    return 0;
}

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    int read_size;

    read_size = recv(sock, buffer, BUFFER_SIZE, 0);
    if (read_size > 0) {
        printf("[*] Received: %s\n", buffer);
        send(sock, "ACK", strlen("ACK"), 0);
    }

    close(sock);
    return NULL;
}
