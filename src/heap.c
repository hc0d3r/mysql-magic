#include <ignotum.h>
#include <string.h>

// just an ideia
// ignotum_getmaplist gets info through /proc/[pid]/maps
// but if /proc is not avaliable maybe you can get the heap
// address by inject brk(0) syscall in the target pid,
// and to measure the bound, you can use process_vm_readv
int getheapmap(pid_t pid, ignotum_mapinfo_t *heap){
    ignotum_maplist_t maplist;

    int ret = 1;
    size_t i;

    ignotum_getmaplist(&maplist, pid);
    for(i=0; i<maplist.len; i++){
        char *pathname = maplist.maps[i].pathname;
        if(pathname == NULL)
            continue;

        if(!strcmp(pathname, "[heap]")){
            memcpy(heap, maplist.maps+i, sizeof(ignotum_mapinfo_t));
            ret = 0;

            goto end;
        }

    }


    end:
    free_ignotum_maplist(&maplist);

    return ret;
}
