#ifndef _CONNECTION_LISTEN_H
#define _CONNECTION_LISTEN_H

#include <sys/types.h>

struct conn_info {
    uid_t uid;
    ino_t inode;
    int checked;
};

int wait_for_connections(off_t *offset, int stime);

#endif
