#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "maze.h"


void exitGame() {
    printf("\nYou pressed 'q'.\nExiting...\n");
    exit(EXIT_SUCCESS);
}

// returns duration in milliseconds
int getmDurationSince(struct timeval time_start) {
    struct timeval time_end;
    if(gettimeofday(&time_end, NULL) == -1) {perror("gettimeofday"); exit(1);}
    int duration = time_end.tv_sec - time_start.tv_sec;
    int uduration = time_end.tv_usec - time_start.tv_usec;
    return duration*1000 + uduration/1000;
}

void printCurrentTime(struct timeval time_start) {
    int mduration = getmDurationSince(time_start);
    printf("%d.%03ds\n", mduration/1000, mduration);
}


char *getDirectionString(int dir) {
    switch(dir){
        case 0: return "left";
        case 1: return "up";
        case 2: return "right";
        case 3: return "down";
    }
    return "";
}

int getNextValidStepDirection(maze_t *maze) {
    int dir = -1;
    for(int c; dir == -1;) {
        c = getc(stdin);
        switch (c) {
            case EOF:
                perror("getc");
                exit(EXIT_FAILURE);
            case 'a':
                dir = 0; break;
            case 'w':
                dir = 1; break;
            case 'd':
                dir = 2; break;
            case 's':
                dir = 3; break;
            case 'q':
                printf("\nYou pressed 'q'.\nExiting...\n");
                exit(EXIT_SUCCESS);
        }
        if(!mzGetCurrentConnectionInDirection(maze, dir)) {
            utilsEraseLine();
            printf("you can't go %s :/", getDirectionString(dir));
            dir = -1;
        }
    }
    
    return dir;
}

int step(maze_t *maze) {
    int dir = getNextValidStepDirection(maze);
    switch(dir){
        case 0: maze->pos.x--; break;
        case 1: maze->pos.y--; break;
        case 2: maze->pos.x++; break;
        case 3: maze->pos.y++; break;
    }
    return dir;
}

void printWinScreen() {
    utilsClearScreen();
    printf("\n");
    printf("##  ##  ######  ##  ##      ##      ##  ######  ####    ######        ##  ######      ##\n");
    printf("##  ##  ##  ##  ##  ##      ####  ####  ##  ##  ##  ##  ##            ##    ##        ##\n");
    printf("##  ##  ##  ##  ##  ##      ##  ##  ##  ######  ##  ##  ####          ##    ##        ##\n");
    printf("  ##    ##  ##  ##  ##      ##      ##  ##  ##  ##  ##  ##            ##    ##          \n");
    printf("  ##    ######  ######      ##      ##  ##  ##  ####    ######        ##    ##        ##\n");
    printf("\n\n");
}

void play(maze_t *maze) {
    utilsClearScreen();
    printf("Hi! Welcome to mazer!\nYou are the 'x'.\nWalk to one of the next rooms with (w|a|s|d)\n");
    printf("Reach the star to win!\n");
    printf("Do that as fast as you can.");
    if(maze->besttime != -1) printf("Best time is: %d.%03ds\n", maze->besttime/1000, maze->besttime);
    printf("\n\n");
    printf("Press any button to start\n");
    getc(stdin);
    utilsClearScreen();
    
    mzPrintCurrentPos(maze);
    mzPrintCurrentRoom(maze);
    struct timeval time_start;
    if(gettimeofday(&time_start, NULL) == -1) {perror("gettimeofday"); exit(1);}
    printCurrentTime(time_start);
    while(!mzIsFinished(maze)) {
        int dir = step(maze);
        utilsClearScreen();
        mzPrintCurrentPos(maze);
        mzPrintCurrentRoom(maze);
        printCurrentTime(time_start);
        printf("You went %s", getDirectionString(dir));
        fflush(stdout);
    }
    
    int mduration = getmDurationSince(time_start);

    printWinScreen();
    
    printf("Your time: %d.%03ds\n", mduration/1000, mduration);
    if(maze->besttime != -1) {
        printf("Best time is: %d.%03ds\n", maze->besttime/1000, maze->besttime);
        if (maze->besttime > mduration) {
            printf("Congratulations, You beat it!\n");
            maze->besttime = mduration;
        } else {
            printf("Better luck next time!\n");
        }
    } else {
        maze->besttime = mduration;
    }

    if(mzStoreTemp(maze) == -1) {fprintf(stderr, "err while trying to save the maze");}
    
}