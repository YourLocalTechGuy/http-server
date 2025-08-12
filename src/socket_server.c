#include "socket_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int start_server(int port) {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 1. CREATE SOCKET
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("SOCKET FAILED");
        return -1;
    }

    // 2. SET ADDRESS AND PORT
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 3. BIND THE SOCKET
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("BIND FAILED");
        close(server_fd);
        return -1;
    }

    // 4. LISTEN FOR CONNECTIONS
    if (listen(server_fd, 3) < 0) {
        perror("LISTEN FAILED");
        close(server_fd);
        return -1;
    }

    printf("SERVER READY TO GO ON PORT %d\n", port);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("ACCEPT FAILED");
            continue;
        }

        // Log client info
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("CONNECTED: %s:%d\n", client_ip, ntohs(address.sin_port));

        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);

        // Parse the request to get the path
        char method[8], path[256];
        if (sscanf(buffer, "%7s %255s", method, path) == 2) {
            printf("REQUEST: %s %s\n", method, path);

            if (strcmp(path, "/") == 0) {
                strcpy(path, "/index.html");
            }
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "./www%s", path);

            FILE *fp = fopen(filepath, "r");
            if (fp) {
                printf("SERVING FILE: %s\n", filepath);
                // Send HTTP header
                char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                write(new_socket, header, strlen(header));

                // Send file contents
                char filebuf[1024];
                size_t n;
                while ((n = fread(filebuf, 1, sizeof(filebuf), fp)) > 0) {
                    write(new_socket, filebuf, n);
                }
                fclose(fp);
            } else {
                printf("FILE NOT FOUND: %s\n", filepath);
                // File not found
                char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found\n";
                write(new_socket, not_found, strlen(not_found));
            }
        } else {
            printf("INVALID REQUEST: %s\n", buffer);
        }

        printf("CONNECTION CLOSED\n");
        close(new_socket);
    }

    close(server_fd);
    return 0;
}