// headers
    #include "block_readwrite.h"
    #include <errno.h>
    #include <stdbool.h>

    #include <poll.h>
    #include <unistd.h>
static int const read_events = POLLIN
    #ifdef POLLRDHUB
        | POLLRDHUB
        #endif
    #ifdef POLLRDNORM
        | POLLRDNORM
        #endif
    #ifdef POLLRDBAND
        | POLLRDBAND
        #endif
    ;
static int const write_events = POLLOUT
    #ifdef POLLWRNORM
        | POLLWRNORM
        #endif
    #ifdef POLLWRBAND
        | POLLWRBAND
        #endif
    ;
ssize_t block_read(int fd, void *buffer, size_t length) {
    size_t remaining = length;
    char *next = (char *) buffer;
    while (remaining) {
        ssize_t n = block_read_some(fd, next, remaining);
        if (n == -1) return -1;
        if (n == 0) return length - remaining;
        remaining -= n;
        next += n;
        }
    return length;
    }
ssize_t block_read_some(int fd, void *buffer, size_t length) {
    while (true) {
        int events = block_poll_fd(fd, read_events);
        if (events == -1) {
            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            return -1;
            }
        ssize_t actual_len = read(fd, buffer, length);
        if (actual_len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
                }
            return -1;
            }
        return actual_len;
        }
    }
ssize_t block_write(int fd, void const *data, size_t length) {
    size_t remaining = length;
    char const *next = (char const *) data;
    while (remaining) {
        ssize_t n = block_write_some(fd, next, remaining);
        if (n == -1) return -1;
        if (n == 0) return length - remaining;
        remaining -= n;
        next += n;
        }
    return length;
    }
ssize_t block_write_some(int fd, void const *data, size_t length) {
    while (true) {
        int events = block_poll_fd(fd, write_events);
        if (events == -1) {
            if (errno == EAGAIN) continue;
            if (errno == EINTR) continue;
            return -1;
            }
        ssize_t actual_len = write(fd, data, length);
        if (actual_len == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                continue;
                }
            return -1;
            }
        return actual_len;
        }
    }
