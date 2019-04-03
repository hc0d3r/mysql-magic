#ifndef _MYSQL_MAGIC_H_
#define _MYSQL_MAGIC_H_

typedef struct {
    off_t *offset;
    size_t n;
} offlist_t;

typedef struct options {
    char *offset;
    int scan;
    char *basedir;
    int listen;
    int stime;
    int cptrace;
} options_t;


/*
off_t *addrs;
char *ofolder;


*/

#endif
