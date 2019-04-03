#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "generic-list.h"

int insert_new_element(generic_list_t *list, void *addr){
    int ret = 1;

    list->ptr = realloc(list->ptr, (list->n+1) * list->size);
    if(list->ptr == NULL)
        goto end;

    memcpy((char *)list->ptr+(list->n*list->size), addr, list->size);
    list->n++;

    ret = 0;

    end:
    return ret;
}
