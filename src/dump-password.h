#ifndef _DUMP_PASSWORD_H_
#define _DUMP_PASSWORD_H_

void dump_mysql_password(pid_t pid, off_t *offset);
ssize_t xprocess_vm_readv(pid_t pid, void *out, size_t len, off_t addr);

#endif
