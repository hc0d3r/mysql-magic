#include "pretty-print.h"
#include <string.h>

#define printable(c) ((c >= 33 && c <= 95) || (c >= 97 && c <= 126))

void hdump(const char *str, size_t limit){
    static const char htable[] = "0123456789abcdef";

    size_t i = 0, j, len = 0, aux, ch_offset;
    char line[65];

    while(i < limit && str[i]){
        len += 16;
        if(len > limit){
            len = limit;
        }

        memset(line, ' ', 16*3);
        aux = 0;
        ch_offset = 48;

        for(j=i; j<len && str[j]; j++){
            char c = str[j];
            line[aux++] = htable[c/16];
            line[aux++] = htable[c%16];
            line[aux++] = ' ';

            line[ch_offset++] = printable(c) ? c : '.';
        }

        line[ch_offset] = 0x0;
        good("%s\n", line);

        i = j;
    }

}
