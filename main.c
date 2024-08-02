#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "maze.h"
#include "mazegen.h"
#include "game.h"

struct termios orig_termios;




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
        maze = mgGenerate(4);
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