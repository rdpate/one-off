// headers
    #include <errno.h>
    #include <limits.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include <fcntl.h>
    #include <unistd.h>
static struct self {
    char const *prog_name;
    } self = {"<unknown>"};
void fatal(int rc, char const *fmt, ...) {
    fprintf(stderr, "%s error: ", self.prog_name);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(rc);
    }
void nonfatal(char const *fmt, ...) {
    fprintf(stderr, "%s: ", self.prog_name);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    }
bool to_int(int *dest, char const *s) {
    char const *end;
    errno = 0;
    long temp = strtol(s, (char **)&end, 10);
    if (end == s) errno = EINVAL;
    else if (!errno) {
        if (INT_MIN <= temp && temp <= INT_MAX) {
            *dest = temp;
            return true;
            }
        errno = ERANGE;
        }
    return false;
    }
int handle_option(char const *name, char const *value) {
    fatal(64, "unknown option %s", name);
    return 64;
    }
int parse_options(char **args, char ***rest) {
    // modifies argument for long options with values, though does reverse the modification afterwards
    // returns 0 or the first non-zero return from handle_option
    // if rest is non-null, *rest is the first unprocessed argument, which will either be the first non-option argument or the argument for which handle_option returned non-zero
    int rc = 0;
    for (; args[0]; ++args) {
        if (!strcmp(args[0], "--")) {
            args += 1;
            break;
            }
        else if (args[0][0] == '-' && args[0][1] != '\0') {
            if (args[0][1] == '-') {
                // long option
                char *eq = strchr(args[0], '=');
                if (!eq) {
                    // long option without value
                    if ((rc = handle_option(args[0] + 2, NULL))) {
                        break;
                        }
                    }
                else {
                    // long option with value
                    *eq = '\0';
                    if ((rc = handle_option(args[0] + 2, eq + 1))) {
                        *eq = '=';
                        break;
                        }
                    *eq = '=';
                    }
                }
            else if (args[0][2] == '\0') {
                // short option without value
                if ((rc = handle_option(args[0] + 1, NULL))) {
                    break;
                    }
                }
            else {
                // short option with value
                char name[2] = {args[0][1]};
                if ((rc = handle_option(name, args[0] + 2))) {
                    break;
                    }
                }
            }
        else break;
    }
    if (rest) {
        *rest = args;
        }
    return rc;
    }
int next_fd(char **parg) {
    char *rest;
    errno = 0;
    long fd = strtol(*parg, &rest, 10);
    if (errno) return -1;
    if (*rest && *rest != ',') return -1;
    if (fd < 0 || fd > INT_MAX) return -1;
    *parg = rest;
    return fd;
    }
int main(int _, char **argv) {
    { // parse options
        char const *x = strrchr((self.prog_name = argv[0]), '/');
        if (x) self.prog_name = x + 1;
        char **rest = 0;
        int rc = parse_options(argv + 1, &rest);
        if (rc) return rc;
        argv = rest;
        }
    if (!*argv || !**argv) return 64;
    { // modify FDs
        char *rest = *argv;
        int fd;
        while ((fd = next_fd(&rest)) != -1) {
            if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
                perror("fcntl");
                return 71;
                }
            if (!*rest) break;
            if (*rest != ',') return 64;
            rest ++;
            }
        }
    argv ++;
    if (!*argv) return 0;
    execvp(argv[0], argv);
    perror("exec");
    return 65;
    }
