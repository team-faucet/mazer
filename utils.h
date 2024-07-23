#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct vec {
    int x;
    int y;
} vec_t;

// reads char from file and returns true if it is in list
bool utilsNextCharIsIn(FILE *f, char *list);

int utilsParseVec(FILE *f, vec_t *vec);

#endif