#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "maze.h"


int getLeftWallOfRoom(int room_index, char dim) {
    int x = room_index%dim;
    int y = room_index/dim;
    return y*(dim-1) + x-1;
}

int getLeftRoomOfWall(int wall_index, char dim) {
    int x = wall_index%(dim-1);
    int y = wall_index/(dim-1);
    return x + dim*y;
}

// returns -1 if there are none
int getRandWall(char *walls, char dim) {
    int n_walls=0;
    for(int i=0; i< dim*(dim-1)*2; i++) {
        if(walls[i]) n_walls++;
    }
    if(n_walls==0) return -1;
    int rand_index = rand()%n_walls;
    int current_index = 0;
    for(int array_index=0; array_index<dim*(dim-1)*2;array_index++) {
        if(walls[array_index]) {
            if(current_index==rand_index) return array_index;
            current_index++;
        }
    }
    return -1;
}

int getUnvisitedRoomNextToWall(char *rooms, int wall_index, char dim) {
    bool isHorizontalWall = wall_index < dim*(dim-1);
    if(isHorizontalWall) {
        int left_index = getLeftRoomOfWall(wall_index, dim);
        int right_index = left_index+1;
        if(rooms[left_index]) return right_index;
        else return left_index;
    } else {
        int top_index = wall_index - (dim*(dim-1));
        int bot_index = top_index + dim;
        if(rooms[top_index]) return bot_index;
        else return top_index;
    }
}


// adds every wall except: borders and walls next to visited rooms
void addValidWallsOfRoom(char *walls, char *rooms, int room_index, char dim) {
    int x = room_index%dim;
    int y = room_index/dim;
    int left_wall = getLeftWallOfRoom(room_index, dim);
    int right_wall = left_wall+1;
    int bot_wall = dim*(dim-1) + room_index;
    int top_wall = bot_wall - dim;

    // check left wall
    if(x > 0){
        walls[left_wall] = !rooms[room_index-1];
    }
    // check right wall
    if(x < dim-1){
        walls[right_wall] = !rooms[room_index+1];
    }
    // check top wall
    if(y > 0){
        walls[top_wall] = !rooms[room_index-dim];
    }
    // check bot wall
    if(y < dim-1){
        walls[bot_wall] = !rooms[room_index+dim];
    }
}

// connect goal to maze(but not directly to starting point)
void connectGoalToMaze(maze_t *maze, int dim) {
    int x = maze->win.x;
    int y = maze->win.y;
    int px = maze->pos.x;
    int py = maze->pos.y;
    int room_index = x + dim*y;
    int left_wall = getLeftWallOfRoom(room_index, dim);
    int right_wall = left_wall+1;
    int bot_wall = dim*(dim-1) + room_index;
    int top_wall = bot_wall - dim;

    int n_valid_walls = 4;
    if(x == 0 || (x-1==px&&y==py)) {left_wall = -1; n_valid_walls--;}
    if(x == dim-1 || (x+1==px&&y==py)) {right_wall = -1; n_valid_walls--;}
    if(y == 0 || (x==px&&y-1==py)) {top_wall = -1; n_valid_walls--;}
    if(y == dim-1 || (x==px&&y+1==py)) {bot_wall = -1; n_valid_walls--;}

    int rand_wall = rand()%n_valid_walls;
    if(left_wall!=-1) {
        if(rand_wall==0) {
            mzSetConnByIndex(maze, left_wall, 1);
            return;
        }
        rand_wall--;
    }
    if(right_wall!=-1) {
        if(rand_wall==0) {
            mzSetConnByIndex(maze, right_wall, 1);
            return;
        }
        rand_wall--;
    }
    if(top_wall!=-1) {
        if(rand_wall==0) {
            mzSetConnByIndex(maze, top_wall, 1);
            return;
        }
        rand_wall--;
    }
    mzSetConnByIndex(maze, bot_wall, 1);
}

maze_t *mgGenerate(char dim) {
    if(dim < 2) {
        fprintf(stderr, "mazegen is only supported for n >= 2\n");
        return NULL;
    }
    srand(time(NULL));

    // init maze_t
    vec_t dims = {dim, dim};
    maze_t *maze = mzMalloc(dims);
    // init list of walls and set zero
    int n_walls = mzGetNumberOfConnections(maze);
    char walls[n_walls];
    memset(walls, 0, n_walls);
    // init list of visited rooms and set one
    int n_rooms = dim*dim;
    char rooms[n_rooms];
    memset(rooms, 0, n_rooms);

    // gen random start
    vec_t start = {rand()%dim,rand()%dim};
    maze->pos = start;
    // gen random goal
    vec_t goal;
    for(goal.x=rand()%dim; goal.x == start.x; goal.x=rand()%dim);
    for(goal.y=rand()%dim; goal.y == start.y; goal.y=rand()%dim);
    maze->win = goal;
    // set start&goal visited
    int start_index = start.x + start.y*dim;
    int goal_index = goal.x + goal.y*dim;
    rooms[start_index] = 1;
    rooms[goal_index] = 1;
    addValidWallsOfRoom(walls, rooms, start_index, dim);


    // algorithm
    //get random wall from valid list, stop loop if there are none
    for(int wall = getRandWall(walls, dim); wall != -1; wall = getRandWall(walls, dim)) {
        //break that wall
        mzSetConnByIndex(maze, wall, 1);
        //remove wall from the list
        walls[wall] = 0;
        //set new room as visited
        int room = getUnvisitedRoomNextToWall(rooms, wall, dim);
        rooms[room] = 1;
        // add new walls
        addValidWallsOfRoom(walls, rooms, room, dim);
    } 

    
    // connect goal to maze(but not directly to starting point)
    connectGoalToMaze(maze, dim);

    return maze;
}
