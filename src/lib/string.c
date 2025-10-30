//src/lib/string.c

//a simple function to compare two strings and arrive at a branch.

#include "lib/string.h"

int strcmp(const char *s1, const char *s2){
    while (*s1 && *s2 && *s1 == *s2){
        s1++;
        s2++;
    }
    return (unsigned char)*s1 -(unsigned char)*s2;
}