#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>

typedef struct vec {
    int x;
    int y;
} vec_t;

typedef struct maze {
    vec_t size;
    vec_t pos;
    vec_t win;
    char *connections;
} maze_t;


maze_t *mzGetSampleMaze();

maze_t *mzParseMaze(char *filename);

bool mzGetConnByIndex(maze_t *maze, int index);

// direction dir is: 0 left, 1 up, 2 right, 3 down
bool mzGetCurrentBorder(maze_t *maze, int dir);

void mzPrintCurrentPos(maze_t *maze);

void mzPrintCurrentRoom(maze_t *maze);

bool mzIsFinished(maze_t *maze);

#endif