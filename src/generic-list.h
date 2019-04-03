#ifndef _GENERIC_LIST_H_
#define _GENERIC_LIST_H_

#include <stddef.h>

#define init_generic_list(x) do { \
    (x).ptr = NULL; \
    (x).n = 0; \
    (x).size = 0; \
} while(0)

#define free_generic_list(x) do { \
    free((x).ptr); \
    init_generic_list(x); \
} while(0)

#define set_list_type(l, type) ((l).size = sizeof(type))

typedef struct generic_list {
    void *ptr;
    size_t n;
    size_t size;
} generic_list_t;

int insert_new_element(generic_list_t *list, void *addr);



#endif
