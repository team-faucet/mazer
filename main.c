#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "maze.h"
#include "mazegen.h"

struct termios orig_termios;

void exitGame() {
    printf("\nYou pressed 'q'.\nExiting...\n");
    exit(EXIT_SUCCESS);
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
            printf("\nyou can't go there :/\n");
            dir = -1;
        }
    }
    
    return dir;
}

void step(maze_t *maze) {
    int dir = getNextValidStepDirection(maze);
    for(int i=0;i<25;i++) printf("\n");
    switch(dir){
        case 0: maze->pos.x--; printf("You went left\n"); break;
        case 1: maze->pos.y--; printf("You went up\n"); break;
        case 2: maze->pos.x++; printf("You went right\n"); break;
        case 3: maze->pos.y++; printf("You went down\n"); break;
    }
}

int play(maze_t *maze) {
    utilsClearScreen();
    printf("Hi! Welcome to mazer!\nYou are the 'x'.\nWalk to one of the next rooms with (w|a|s|d)\n");
    printf("Reach the star at the bottom left to win!\n");
    printf("Press any button to start\n");
    if(getc(stdin)=='q') exitGame();
    utilsClearScreen();
    
    
    mzPrintCurrentRoom(maze);
    mzPrintCurrentPos(maze);
    while(!mzIsFinished(maze)) {
        step(maze);
        utilsClearScreen();
        mzPrintCurrentRoom(maze);
        mzPrintCurrentPos(maze);
    }

    printf("\nCongratulations!\nYou have won!\n");
    getc(stdin);
    return 0;
}


void resetTerminalSettings() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\33[?25h"); // show cursor
    fflush(stdout);
}

void setupTerminal() {
    printf("\33[?25l"); // hide cursor
    fflush(stdout);
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(resetTerminalSettings);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}


int main(int argc, char *argv[]) {
    maze_t *maze;

    if(argc == 1){
        maze = mgGenerate(10);
    } else if(argc == 2){
        maze = mzParse(argv[1]);
        if(maze==NULL) exit(EXIT_FAILURE);
    } else if(argc == 3) {
        if(!strncmp("--convert", argv[1], strlen("--convert"))) {
            if(mzConvertFile(argv[2])==-1) {
                exit(EXIT_FAILURE);
            }
            printf("Successfully converted file \"%s\"\n", argv[2]);
            exit(EXIT_SUCCESS);
        }
        
    }



    setupTerminal();
    play(maze);


    mzFree(maze);
}