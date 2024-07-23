#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>
#include "utils.h"

typedef struct maze {
    vec_t size;
    vec_t pos;
    vec_t win;
    char *connections;
} maze_t;

// sets connection at index
void mzSetConnByIndex(maze_t *maze, int index, char val);

// true if connection is there
bool mzGetConnByIndex(maze_t *maze, int index);

/*
returns ture, if the player can walk from his current position in direction dir
dir is: 0 left, 1 up, 2 right, 3 down
*/
bool mzGetCurrentConnectionInDirection(maze_t *maze, int dir);

maze_t *mzMalloc(vec_t dims);

void mzFree(maze_t *maze);

// returns a 4x4 sample maze
maze_t *mzGetSampleMaze();

// parses maze from filename
maze_t *mzParseMaze(char *filename);

bool mzIsFinished(maze_t *maze);

void mzPrintCurrentPos(maze_t *maze);

void mzPrintCurrentRoom(maze_t *maze);



#endif