#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

void setConnByIndex(maze_t *maze, int index, char val) {
    int true_index = index/8;
    int offset = 7-index%8;
    maze->connections[true_index] |= (val&1) << offset;
}

bool mzGetConnByIndex(maze_t *maze, int index) {
    int true_index = index/8;
    int offset = 7-index%8;
    return (maze->connections[true_index] >> offset) & 1;
}

// direction dir is: 0 left, 1 up, 2 right, 3 down
bool mzGetCurrentBorder(maze_t *maze, int dir) {
    int x = maze->pos.x;
    int y = maze->pos.y;
    int start_index_vertical_borders = (maze->size.x-1) * maze->size.y;
    int left_index = x + y*(maze->size.x-1)-1;
    int right_index = left_index + 1;
    int top_index = start_index_vertical_borders + x + (y-1)*maze->size.x;
    int bot_index = top_index + maze->size.x;
    switch(dir) {
        case 0:
            if(x > 0) return mzGetConnByIndex(maze, left_index);
            else return false;
        case 1:
            if(y > 0) return mzGetConnByIndex(maze, top_index);
            else return false;
        case 2:
            if(x < (maze->size.x-1)) return mzGetConnByIndex(maze, right_index);
            else return false;
        case 3:
            if(y < (maze->size.y-1)) return mzGetConnByIndex(maze, bot_index);
            else return false;
    }
    return false;
}

maze_t *mallocMaze(vec_t dims) {
    maze_t *maze = malloc(sizeof(maze_t));
    int n_conns = (dims.x-1)*dims.y + dims.x*(dims.y-1);
    int n_bytes = n_conns/8 + (n_conns%8!=0);
    maze->connections = malloc(n_bytes);
    maze->size = dims;
    return maze;
}

maze_t *mzGetSampleMaze(){
    vec_t dims = {
        .x = 4,
        .y = 4,
    };
    maze_t *maze = mallocMaze(dims);
    maze->pos.x = 0;
    maze->pos.y = 0;
    maze->win.x = dims.x-1;
    maze->win.y = dims.y-1;
    maze->connections[0] = 0b10100110;
    maze->connections[1] = 0b01100110;
    maze->connections[2] = 0b11110111;
    return maze;
}

maze_t *mzParseMaze(char *filename) {
    char errorbuf[100];
    errorbuf[0] = '\0';
    char *errorstr=errorbuf;
    int line=0;
    char *ext = strrchr(filename, '.');
    if (ext==NULL){errorstr = "filename extension not found, has to be .mzh"; goto exit_error;}
    if(strcmp(ext, ".mzh")) {fprintf(stderr, "maze filenames have to end on .mzh\n"); goto exit_error;}
    FILE *f = fopen(filename, "r"); if(f == NULL){perror("fopen"); return NULL;}

    
    vec_t dims;
    if(parseVec(f, &dims)==-1) {errorstr="parsing dim vector"; goto exit_error;}
    if(dims.x <1 || dims.y<1) {errorstr="maze dimensions have to be >0"; goto exit_error;}
    maze_t *m = mallocMaze(dims);
    if(!nextCharIs(f, ',')) {{errorstr="assumed ','"; goto exit_error;}}
    if(parseVec(f, &(m->pos))==-1) {errorstr="parsing pos vector"; goto exit_error;}
    if(!nextCharIs(f, ',')) {{errorstr="assumed ','"; goto exit_error;}}
    if(parseVec(f, &(m->win))==-1) {errorstr="parsing win vector"; goto exit_error;}

    if(!nextCharIs(f, '\n')) {{errorstr="assumed newline"; goto exit_error;}}
    
    // parsing connections
    int c;
    int n_conns = (dims.x-1)*dims.y + dims.x*(dims.y-1);
    int index = 0;
    while(index < n_conns) {
        if((c=getc(f)) == EOF) {
            if(feof(f)){
                char *format="eof while reading connections, after %d bytes"; 
                snprintf(errorbuf,sizeof(errorbuf), format, index);
                errorstr = errorbuf;
                goto exit_error;}
            fprintf(stderr, "err while parsing file\n");return NULL;
        }
        if( c != '0' && c != '1') continue;

        setConnByIndex(m, index, c == '1');
        index++;

    }
    do c=getc(f); while(c=='\n');
    if((c=getc(f)) != EOF){
        errorstr="file too long"; goto exit_error;        
    }  

    fclose(f);
    return m;

    exit_error:
    fprintf(stderr, "error while parsing file: %s(line %d)\n", errorstr, line);
    return NULL;
}


bool mzIsFinished(maze_t *maze) {
    return maze->pos.x == maze->win.x && maze->pos.y == maze->win.y;
}

void mzPrintCurrentPos(maze_t *maze) {
    printf("|"); for(int i=0; i < maze->size.x; i++) printf("---"); printf("|\n");
    for(int y=0; y < maze->size.y; y++) {
        printf("|");
        for(int x=0; x < maze->size.x; x++) {
            if(x == maze->pos.x && y == maze->pos.y) printf(" x "); 
            else if(x == maze->win.x && y == maze->win.y) printf(" * ");
            else printf("   ");
        }
        printf("|\n");
    }
    printf("|"); for(int i=0; i < maze->size.x; i++) printf("---"); printf("|\n");
    printf("\n");
}

void mzPrintCurrentRoom(maze_t *maze) {
    bool left = mzGetCurrentBorder(maze, 0);
    bool top = mzGetCurrentBorder(maze, 1);
    bool right = mzGetCurrentBorder(maze, 2);
    bool bot = mzGetCurrentBorder(maze, 3);

    // top
    if(top) printf("#######   #######\n#######   #######\n");
    else printf("#################\n#################\n");

    printf("#####       #####\n");

    // left side
    if(left) printf("     ");
    else printf("#####");
    printf("   x   ");

    // right side
    if(right) printf("     \n");
    else printf("#####\n");

    printf("#####       #####\n");

    if(bot) printf("#######   #######\n#######   #######\n");
    else printf("#################\n#################\n");
}

