#include "kernel/types.h"
#include "user/user.h"

void primes(int left_fd) {
    int num = 0, x = 0, fds[2] = {0};
    int n = read(left_fd, (void*)&num, sizeof(int));
    if (n <= 0) {
        exit(0);
    }

    printf("prime %d\n", num);

    if (num >= 280) {
        close(left_fd);
        exit(0);
    }

    if (pipe(fds) < 0) {
        printf("pipe failed\n");
        close(left_fd);
        return;
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        close(left_fd);
        close(fds[0]);
        close(fds[1]);
        return;
    }

    if (pid > 0) { // parent
        close(fds[0]);
        while (read(left_fd, (void*)&x, sizeof(int)) > 0) {
            if (x % num != 0) {
                write(fds[1], (void*)&x, sizeof(int));
            }
        }

        close(left_fd);
        close(fds[1]);
        wait(0);
    } else { // child
        close(fds[1]);
        close(left_fd);
        primes(fds[0]);
        close(fds[0]);
        exit(0);
    }
}


int main(int argc, char *argv[]) {
    int fds[2] = {0};
    if (pipe(fds) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid > 0) {
        close(fds[0]);
        for (int i = 2; i < 280; i++) {
            write(fds[1], (void*)&i, sizeof(int));
        }
        close(fds[1]);
        wait(0);
    } else {
        close(fds[1]);
        primes(fds[0]);
        close(fds[0]);
    }

    exit(0);
}