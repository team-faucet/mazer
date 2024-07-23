#include "utils.h"

#include <stdbool.h>
#include <stdio.h>

bool utilsNextCharIsIn(FILE *f, char *list) {
    int c = getc(f);
    for(int i=0;list[i]!=0;i++) {
        if(c == list[i]) return true;
    }
    return false;
}

int utilsParseVec(FILE *f, vec_t *vec) {
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