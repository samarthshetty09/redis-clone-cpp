#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void die(const char *msg) {
    perror(msg);
    exit(1);
}

void do_something(int conn_fd) {
    char read_buf[64] = {};
    int rv = recv(conn_fd, read_buf, sizeof(read_buf) - 1, 0);
    if (rv < 0) {
        fprintf(stderr, "recv error: %d\n", rv);
        return;
    }
    printf("Client Says: %s\n", read_buf);
    
    char write_buf[64] = "world";
    write(conn_fd, write_buf, strlen(write_buf));

}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        die("socket error");
    }
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int rv = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    while(1) {
        struct sockaddr_in client_addr = {};
        socklen_t client_addr_len = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (conn_fd < 0) {
            continue;
        }
        do_something(conn_fd);
        close(conn_fd);
    }

}