#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    // Open the directory or file
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // If it's a file, compare name
    if (st.type == T_FILE) {
        // Extract filename from path
        char *name = path;
        for (int i = strlen(path); i >= 0; i--) {
            if (path[i] == '/') {
                name = &path[i+1];
                break;
            }
        }

        if (strcmp(name, target) == 0) {
            printf("%s\n", path);
        }

        close(fd);
        return;
    }

    // If it's a directory, recurse into it
    if (st.type == T_DIR) {

        // Build prefix: path/
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            close(fd);
            return;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        // Read all directory entries
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {

            if (de.inum == 0) continue;   // empty entry

            // Skip "." and ".."
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            // Append directory name
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // Recursive call
            find(buf, target);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}

