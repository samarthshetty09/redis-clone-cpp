#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <assert.h>

void die(const char *msg) {
    perror(msg);
    exit(1);
}

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

const size_t k_max_msg = 4096;

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

static int32_t read_full(int fd, char *buf, size_t n) {
    while(n>0) {
        ssize_t rv = read(fd, buf, n);
        if(rv <= 0) return -1; // error or unexpected eof
        assert((size_t)rv <= n);
        n -=  (size_t)rv;
        buf += rv;
    } 
    return 0;  
}

static int32_t write_all(int fd, const char *buf, size_t n) { 
    while (n > 0) {
        ssize_t rv = write(fd, buf, n); 
        if (rv <= 0) {
            return -1;
        }
        assert((size_t)rv <= n); 
        n -= (size_t)rv;
        buf += rv;
    }
    return 0; 
}

static int32_t one_request(int connfd) {
    // 4 bytes header
    char rbuf[5 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    rbuf[4 + len] = '\0';
    printf("client says: %s\n", &rbuf[4]);

    // reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(connfd, wbuf, 4 + len);
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Listening on port 8080\n");
    if (fd < 0) {
        die("socket error");
    }
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = ntohl(0);

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
        /* do_something(conn_fd); */
        // only serve one client connection at a time
        while(true) {
            int32_t err = one_request(conn_fd);
            if (err) {
                break;
            }
        }
        close(conn_fd);
    }

}