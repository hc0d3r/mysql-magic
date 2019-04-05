#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define stringfy(x) _stringfy(x)
#define _stringfy(x) #x

#ifndef MYSQLCLI
 #define MYSQL_CLI "/usr/bin/mysql"
#else
 #define MYSQL_CLI stringfy(MYSQLCLI)
#endif

#define MYSQL_LEN sizeof(MYSQL_CLI)

#include <sys/types.h>

#endif

extern ssize_t (*memreadfunc)(pid_t, void *, size_t, off_t);
extern char *output_folder;
extern uid_t uid;
