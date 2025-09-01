#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *target) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512] = {0}, *p;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_DEVICE:
        case T_FILE:
            char *filename = path;
            for(char *ptr = path + strlen(path); ptr >= path; ptr--) {
                if (*ptr == '/') {
                    filename = ptr + 1;
                    break;
                }
            }
            if (strcmp(filename, target) == 0) {
                printf("%s\n", path);
            }
            break;
            
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while (read(fd, &de, sizeof(de)) == sizeof de) {
                if (de.inum == 0) {
                    continue;
                }

                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                    continue;
                }

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }

                if ((st.type == T_FILE || st.type == T_DEVICE) && 
                    strcmp(de.name, target) == 0) {
                    printf("%s\n", buf);
                }

                if (st.type == T_DIR) {
                    find(buf, target);
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: find directory name\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}