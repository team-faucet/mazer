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

void printHelp() {
    printf("Usage: mazer (<filename> | -l | -n <number> | -c <filename>)\n\n");
    printf("Options:\n");
    printf("  -l          play the last maze played\n");
    printf("  -n          play a generated maze with dimensions n*n\n");
    printf("  -c          convert a .mzh file to .zb\n");
    printf("\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    maze_t *maze;

    if(argc == 1){
        maze = mgGenerate(6);
    } else if(argc == 2){
        if(!strcmp("-h", argv[1]) || !strcmp("--help", argv[1])) printHelp();
        if(!strcmp("-l", argv[1])) maze = mzParseTemp();
        else maze = mzParse(argv[1]);
        if(maze==NULL) exit(EXIT_FAILURE);
    } else if(argc == 3) {
        if(!strcmp("-n", argv[1])) {
            int n = utilsParsePosInt(argv[2]);
            if (n==-1) {
                printf("Coulnd't parse n\n");
                exit(EXIT_FAILURE);
            }
            maze = mgGenerate(n);
        } else if(!strncmp("--convert", argv[1], strlen("--convert"))) {
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