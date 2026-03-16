#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return 1;
    }
    
    FILE *f = fopen(argv[1], "rb");
    if (!f) return 1;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *data = malloc(size);
    fread(data, 1, size, f);
    fclose(f);
    
    int fd = syscall(SYS_memfd_create, "[kworker/u:2]", 0);
    if (fd < 0) return 1;
    
    write(fd, data, size);
    free(data);
    
    char *const new_argv[] = { "[kworker/u:3]", NULL };
    fexecve(fd, new_argv, environ);
    return 0;
}
