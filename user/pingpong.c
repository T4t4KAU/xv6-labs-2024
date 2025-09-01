#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int parent_child_fd[2];
    int child_parent_fd[2];
    
    if (pipe(parent_child_fd) == -1) {
        printf("pipe 1 failed");
        exit(1);
    }

    if (pipe(child_parent_fd) == -1) {
        printf("pipe 2 failed");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork failed");
        exit(1);
    }

    if (pid > 0) { // parent
        close(parent_child_fd[0]);
        close(child_parent_fd[1]);

        char send[] = "ping\0", recv[16] = {0};
        write(parent_child_fd[1], send, strlen(send));
        wait(0);
        read(child_parent_fd[0], recv, sizeof(recv));
        printf("%d: received %s\n", getpid(), recv);
    } else { // child
        close(parent_child_fd[1]);
        close(child_parent_fd[0]);

        char send[] = "pong\0", recv[16] = {0};
        read(parent_child_fd[0], recv, sizeof(recv));
        printf("%d: received %s\n", getpid(), recv);
        write(child_parent_fd[1], send, strlen(send));
    }

    exit(0);
}