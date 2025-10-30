//src/lib/memcpy.c

//custom memory copy function

#include "lib/memcpy.h"



void *memcpy(void *dest, const void *src, size_t n){
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    for (int i = 0; i <n; i++){
        d[i] = s[i];
    }
    return dest;
}