#include <sys/ptrace.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <sys/reg.h>

#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ignotum.h>
#include <pty.h>

#include "offset-scan.h"
#include "globals.h"
#include "heap.h"
#include "pretty-print.h"

#if defined(__i386)
 #define SYSCALLREG ORIG_EAX
#else
 #define SYSCALLREG ORIG_RAX
#endif

enum {
    pipe_read,
    pipe_write
};


void exec_mysql(int c){
    struct pollfd pfd;
    char buf[1024];
    pid_t pid;
    int master;

    pid = forkpty(&master, NULL, NULL, NULL);

    if(pid == 0){
        // stop the pid to be attached
        kill(getpid(), SIGSTOP);

        execve(MYSQL_CLI, (char *[]){ "mysql", "-u", "rasputin", "-p", NULL }, NULL);
        exit(0);
    }

    // send pid to main process
    write(c, &pid, sizeof(pid_t));
    close(c);

    // wait for password prompt (Enter password: )
    pfd.fd = master;
    pfd.events = POLLIN;
    poll(&pfd, 1, -1);

    // write the password
    write(master, "r0ck4w4y b34ch\n", 15);

    // just wait program exit
    while(read(master, buf, sizeof(buf)-1) > 0);
    close(master);

}


int mysql_offset_scan(void){
    ignotum_search_t search = DEFAULT_IGNOTUM_MEMSEARCH;
    ignotum_mapinfo_t heap;

    int pipefd[2], status, ret = 1;
    pid_t pid, tpid;

    long syscallnr;

    ssize_t n;
    size_t i;


    if(pipe(pipefd) == -1){
        perror("pipe()");
        goto end;
    }

    info("executing mysql ...\n");

    pid = fork();
    if(pid == 0){
        close(pipefd[pipe_read]);

        // creates a process to deal with the input
        exec_mysql(pipefd[pipe_write]);

        _exit(0);
    }

    close(pipefd[pipe_write]);

    // waiting for the pid to be attached
    if(read(pipefd[pipe_read], &tpid, sizeof(pid_t)) != sizeof(pid_t))
        goto end;

    info("pid = %d\n", tpid);

    // attach the process
    if(ptrace(PTRACE_ATTACH, tpid) == -1)
        goto end;

    if(waitpid(tpid, NULL, 0) == -1)
        goto end;

    ptrace(PTRACE_SETOPTIONS, tpid, 0, PTRACE_O_TRACESYSGOOD);

    info("waiting syscall exit_group\n");


    while(ptrace(PTRACE_SYSCALL, tpid, 0, 0) != -1){
        if(waitpid(tpid, &status, 0) == -1)
            break;

        if(WIFSTOPPED(status) && WSTOPSIG(status) & 0x80){
            syscallnr = ptrace(PTRACE_PEEKUSER, tpid, sizeof(long)*SYSCALLREG);
            if(syscallnr == __NR_exit_group){
                good("exit detected !!!\n");
                break;
            }
        }
    }

    info("getting heap info\n");

    if(getheapmap(tpid, &heap) == 0){
        info("reading the heap ...\n");
        size_t len = heap.end_addr-heap.start_addr;
        char *dump = malloc(len);
        if(dump == NULL){
            bad("malloc() failed\n");
            goto end;
        }

        n = memreadfunc(tpid, dump, len, heap.start_addr);
        if(n <= 0){
            bad("failed to read heap\n");
            goto end;
        }

        if(ignotum_search(&search, 0, dump, n, "r0ck4w4y b34ch", 15)){
            ret = 0;
            good("password found\n");
            good("offset list: ");

            printf(GREEN);
            for(i=0; i<search.len; i++){
                printf("0x%lx", search.addrs[i]);
                if(i+1 < search.len)
                    putchar(',');
            }
            printf(RESET);

            putchar('\n');
        } else {
            bad("failed to find password ...\n");
        }
    } else {
        bad("failed to get heap\n");
    }

    ptrace(PTRACE_DETACH, tpid, 0, 0);


    end:
    return ret;
}
