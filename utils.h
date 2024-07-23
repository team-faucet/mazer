#ifndef UTILS_H
#define UTIL_H

typedef struct vec {
    int x;
    int y;
} vec_t;

bool nextCharIs(FILE *f, char c);

int parseVec(FILE *f, vec_t *vec);

#endif