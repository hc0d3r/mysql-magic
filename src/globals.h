#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifndef MYSQL_CLI
 #define MYSQL_CLI "/usr/bin/mysql"
#endif

#define MYSQL_LEN sizeof(MYSQL_CLI)

#include <sys/types.h>

#endif

extern ssize_t (*memreadfunc)(pid_t, void *, size_t, off_t);
extern char *output_folder;
extern uid_t uid;
