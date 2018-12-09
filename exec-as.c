#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char const *prog_name = "<unknown>";
void fatal(int rc, char const *fmt, ...) {
    fprintf(stderr, "%s error: ", prog_name);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(rc);
    }

int main(int argc, char **argv) {
    { // prog_name
        char const *x = strrchr((prog_name = argv[0]), '/');
        if (x) prog_name = x + 1;
        }
    if (argc < 3)
        fatal(64, "missing %sCOMMAND.. arguments", (argc == 1 ? "ARG0 " : ""));
    ++argv;
    char *x = argv[0];
    argv[0] = argv[1];
    argv[1] = x;
    execvp(argv[0], argv + 1);
    perror("execvp");
    fatal(71, "exec failed");
    }
