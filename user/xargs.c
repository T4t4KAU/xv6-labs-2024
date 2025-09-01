#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

const int MAX_SIZE = 512;

void xargs(int argc, char *args[]) {
    while (1) {
        int pos = 0, n = 0;
        char buf[MAX_SIZE];
        while (pos < MAX_SIZE - 1) {
            int n = read(0, &buf[pos], 1);
            if (n <= 0) {
                break;
            }

            if (buf[pos] == '\n') {
                break;
            }
            pos++;
        }

        if (n <= 0 && pos == 0) {
            break;
        }

        buf[pos] = 0;
        args[argc - 1] = buf;
        args[argc] = 0;

        int pid = fork();
        if (pid == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else if (pid > 0) {
            wait(0);
        } else {
            fprintf(2, "xargs: fork failed\n");
        }
    }

    exit(0);
}

int main(int argc, char* argv[]) {
    char* args[MAXARG] = {0};

    for (int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    xargs(argc, args);

    exit(0);
}