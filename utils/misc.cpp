#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <cassert>

static int32_t read_full(int fd, char *buf, size_t n) {
    while(n>0) {
        ssize_t rv = read(fd, buf, n);
        if(rv <= 0) return -1; // error or unexpected eof
        assert((size_t)rv <= n);
        n -=  (size_t)rv;
        buf += rv;
    }   
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