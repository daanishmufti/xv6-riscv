#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static const char *seps = " -\r\t\n./,";

static int is_sep(char c) {
    const char *p = seps;
    while (*p) {
        if (*p == c) return 1;
        p++;
    }
    return 0;
}

static void process_fd(int fd) {
    char buf[512];
    int n;
    unsigned long acc = 0;
    int in_number = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            char c = buf[i];

            if (c >= '0' && c <= '9') {
                in_number = 1;
                acc = acc * 10 + (c - '0');
            } else if (is_sep(c)) {

                if (in_number) {
                    if (acc % 5 == 0 || acc % 6 == 0) {
                        printf("%lu\n", acc);
                    }
                    acc = 0;
                    in_number = 0;
                }
        
            } else {

                if (in_number) {
                    if (acc % 5 == 0 || acc % 6 == 0) {
                        printf("%lu\n", acc);
                    }
                    acc = 0;
                    in_number = 0;
                }
            }
        }
    }

    if (in_number) {
        if (acc % 5 == 0 || acc % 6 == 0) {
            printf("%lu\n", acc);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        process_fd(0);
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0);
        if (fd < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            continue;
        }
        process_fd(fd);
        close(fd);
    }
    exit(0);
}

