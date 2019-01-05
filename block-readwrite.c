// Write input to output, blocking even if they are non-blocking.

#include "block_readwrite.h"
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **_) {
    if (argc > 1) return 64;
    int const length = 4096;
    char buffer[length];
    while (true) {
        ssize_t n = block_read(0, buffer, length);
        if (n == -1) {
            perror("read");
            return 74;
            }
        if (n == 0) break;
        ssize_t wrote = block_write(1, buffer, n);
        if (wrote == -1 || wrote != n) {
            perror("write");
            return 74;
            }
        }
    return 0;
    }
