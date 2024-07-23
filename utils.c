#include "utils.h"

#include <stdbool.h>
#include <stdio.h>

bool nextCharIs(FILE *f, char c) {
    return c == getc(f);
}

int parseVec(FILE *f, vec_t *vec) {
    int c,x,y;
    if((c=getc(f)) == EOF) {return -1;}
    x = c - '0';
    getc(f);
    if((c=getc(f)) == EOF) {return -1;}
    y = c - '0';
    vec->x = x;
    vec->y = y;
    return 0;
}