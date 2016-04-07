#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char const usage[] = "usage: exec-as ARG0 COMMAND [ARG]..\n";

int main(int argc, char **argv) {
    if (argc < 3) {
        write(2, usage, (sizeof usage) - 1);
        return 64; // EX_USAGE
    }
    ++argv;
    char *x = argv[0];
    argv[0] = argv[1];
    argv[1] = x;
    execvp(argv[0], argv + 1);
    fprintf(stderr, "exec-as: %s: %s\n", argv[0], strerror(errno));
    return 69;  // EX_UNAVAILABLE
}
