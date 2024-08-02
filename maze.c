#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"

#define STD_SAVEPATH "/tmp/saved_maze.mzb"

void mzSetConnByIndex(maze_t *maze, int index, char val) {
    int true_index = index/8;
    int offset = 7-index%8;
    maze->connections[true_index] |= (val&1) << offset;
}

bool mzGetConnByIndex(maze_t *maze, int index) {
    int true_index = index/8;
    int offset = 7-index%8;
    return (maze->connections[true_index] >> offset) & 1;
}

bool mzGetCurrentConnectionInDirection(maze_t *maze, int dir) {
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

int mzGetNumberOfConnections(maze_t *maze) {
    return ((maze->size.x)-1)*((maze->size).y) + ((maze->size).x)*((maze->size).y-1);
}

maze_t *mzMalloc(vec_t dims) {
    maze_t *maze = malloc(sizeof(maze_t));
    maze->size = dims;
    int n_conns = mzGetNumberOfConnections(maze);
    int n_bytes = n_conns/8 + (n_conns%8!=0);
    maze->connections = malloc(n_bytes);
    memset(maze->connections, 0, n_bytes);
    maze->besttime = -1;
    return maze;
}

void mzFree(maze_t *maze){
    free(maze->connections);
    free(maze);
}

maze_t *mzGetSampleMaze(){
    vec_t dims = {
        .x = 4,
        .y = 4,
    };
    maze_t *maze = mzMalloc(dims);
    maze->pos.x = 0;
    maze->pos.y = 0;
    maze->win.x = dims.x-1;
    maze->win.y = dims.y-1;
    maze->connections[0] = 166;
    maze->connections[1] = 102;
    maze->connections[2] = 247;
    return maze;
}

maze_t *mzParseH(char *filename) {
    char errorbuf[100];
    errorbuf[0] = '\0';
    char *errorstr=errorbuf;
    int line=0;
    FILE *f = fopen(filename, "r"); if(f == NULL){perror("fopen"); return NULL;}

    vec_t dims;
    if(utilsParseVec(f, &dims)==-1) {errorstr="parsing dim vector"; goto exit_error;}
    if(dims.x <1 || dims.y<1) {errorstr="maze dimensions have to be >0"; goto exit_error;}
    maze_t *m = mzMalloc(dims);
    if(!utilsNextCharIsIn(f, ",\n")) {{errorstr="assumed ',' or newline'"; goto exit_error;}}
    if(utilsParseVec(f, &(m->start))==-1) {errorstr="parsing pos vector"; goto exit_error;}
    m->pos = m->start;
    if(!utilsNextCharIsIn(f, ",\n")) {{errorstr="assumed ',' or newline"; goto exit_error;}}
    if(utilsParseVec(f, &(m->win))==-1) {errorstr="parsing win vector"; goto exit_error;}

    if(!utilsNextCharIsIn(f, "\n")) {{errorstr="assumed newline"; goto exit_error;}}
    
    // parsing connections
    int c;
    int n_conns = mzGetNumberOfConnections(m);
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

        mzSetConnByIndex(m, index, c == '1');
        index++;

    }
    do c=getc(f); while(c=='\n');
    if((c=getc(f)) != EOF){
        errorstr="file too long"; goto exit_error;        
    }  

    fclose(f);
    return m;

    exit_error:
    fclose(f);
    fprintf(stderr, "error while parsing file: %s(line %d)\n", errorstr, line);
    return NULL;
}


struct mazeBinaryHead {
    char fileinfo; //this is space reserved for identification that could be used in later versions
    vec_t size;
    vec_t start;
    vec_t win;
    int besttime;
};

/*
maze binary consists of 
1. standard information about the maze
2. Connection list
3. Key-value pairs of Metadata
*/
maze_t *mzParseB(char *filename) {
    char errorbuf[200];
    errorbuf[0] = '\0';
    char *errorstr=errorbuf;
    FILE *f = fopen(filename, "r"); if(f == NULL){perror("fopen"); return NULL;}
    // read in head
    struct mazeBinaryHead head;
    if(fread(&head, 1, sizeof(head), f) != sizeof(head)) {
        errorstr = "parsing head";
        goto exit_error;
    }
    if(head.size.x <1 || head.size.y<1) {errorstr="maze dimensions have to be >0"; goto exit_error;}
    // init maze_t
    maze_t *maze = mzMalloc(head.size);
    maze->pos = head.start;
    maze->start = head.start;
    maze->win = head.win;
    maze->besttime = head.besttime;
    int n_conns = mzGetNumberOfConnections(maze);
    size_t n_bytes = n_conns/8 + (n_conns%8!=0);
    // read in connections
    if(fread(maze->connections, 1, n_bytes, f) != n_bytes) {
        errorstr = "parsing connections";
        goto exit_error;
    }
    fclose(f);
    return maze;

    exit_error:
    fclose(f);
    fprintf(stderr, "error while parsing file %s: %s\n", filename, errorstr);
    return NULL;
}

int mzStoreB(maze_t *maze, char *filename) {
    char errorbuf[200];
    errorbuf[0] = '\0';
    char *errorstr=errorbuf;

    // convert maze into binaryMaze
    struct mazeBinaryHead head;
    head.size = maze->size;
    head.start = maze->start;
    head.win = maze->win;
    head.besttime = maze->besttime;
    
    int n_conns = mzGetNumberOfConnections(maze);
    size_t n_bytes = n_conns/8 + (n_conns%8!=0);

    FILE *f = fopen(filename, "w");

    if(fwrite(&head, 1, sizeof(head), f) != sizeof(head)) {
        errorstr="writing head";
        goto exit_error;
    }
    if(fwrite(maze->connections, 1, n_bytes, f) != n_bytes) {
        errorstr="writing connections";
        goto exit_error;
    }

    fclose(f);
    return 0;

    exit_error:
    fclose(f);
    fprintf(stderr, "error while parsing file %s: %s\n", filename, errorstr);
    return -1;
}

int mzConvertFile(char *filename){
    char *ext = strrchr(filename, '.');
    if (ext==NULL){fprintf(stderr,"file extension not found, has to be .mzh or .mzb"); return -1;}

    if(strcmp(ext, ".mzh")) {
        fprintf(stderr, "error while converting: can currently only convert .mzh files\n");
        return -1;
    }
    char nfilename[strlen(filename)+1];
    strcpy(nfilename, filename);
    nfilename[strlen(filename)-1] = 'b';
    maze_t *maze = mzParseH(filename);
    if(maze == NULL) return -1;
    if(mzStoreB(maze, nfilename) == -1) return -1;
    return 0;
}

maze_t *mzParse(char *filename) {
    struct stat buf;
    if(stat(filename, &buf) == -1) {perror("stat"); return NULL;}
    if(!S_ISREG(buf.st_mode)) { fprintf(stderr, "%s is not a file\n", filename); return NULL;}
    char *ext = strrchr(filename, '.');
    if(ext==NULL){ fprintf(stderr, "file extension has to be .mzh or .mzb\n"); return NULL;}


    if(!strcmp(ext, ".mzh")) {
        return mzParseH(filename);
    }
    if(!strcmp(ext, ".mzb")) {
        return mzParseB(filename);
    }

    fprintf(stderr, "file extension has to be .mzh or .mzb\n");
    return NULL;
}

int mzStoreTemp(maze_t *maze) {
    return mzStoreB(maze, STD_SAVEPATH);
}

maze_t *mzParseTemp() {
    return mzParse(STD_SAVEPATH);
}

bool mzIsFinished(maze_t *maze) {
    return maze->pos.x == maze->win.x && maze->pos.y == maze->win.y;
}

void mzPrintCurrentPos(maze_t *maze) {
    printf("\n");
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
    bool left = mzGetCurrentConnectionInDirection(maze, 0);
    bool top = mzGetCurrentConnectionInDirection(maze, 1);
    bool right = mzGetCurrentConnectionInDirection(maze, 2);
    bool bot = mzGetCurrentConnectionInDirection(maze, 3);

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

    if(bot) printf("#######   #######\n#######   #######");
    else printf("#################\n#################");
    printf("\n");
}

