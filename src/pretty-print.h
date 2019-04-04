#ifndef _PRETTY_PRINT_H_
#define _PRETTY_PRINT_H_

#include <time.h>
#include <stdio.h>

#define RESET "\033[0m"

#define RED "\033[1;31m"
#define CYAN "\033[36m"
#define GREEN "\033[1;32m"

#define prety_print(color, fmt, args...) do { \
    struct tm *now; \
    time_t t = time(NULL); \
    now = localtime(&t); \
    printf(color "[%02d:%02d:%02d] " fmt "\033[0m", \
    now->tm_hour, now->tm_min, now->tm_sec, \
        ##args); \
} while(0)

#define good(msg...) prety_print(GREEN, msg)
#define bad(msg...) prety_print(RED, msg)
#define info(msg...) prety_print(CYAN, msg)

void hdump(const char *str, size_t limit);

#endif
