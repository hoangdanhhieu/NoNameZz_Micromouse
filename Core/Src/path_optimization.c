/*
 * path_optimization.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */

#include "path_optimization.h"


const int8_t move[4][3] = {{0, -1, 8}, {0, 1, 4}, {-1, 0, 2}, {1, 0, 1}};
const float half_diagonal = sqrt(pow(square_size/2, 2) + pow(square_size/2, 2));
const float offset45 = (float)tan(22.5 * M_PI / 180) * halfSize_MicroMouse;
const float offset90 = halfSize_MicroMouse;

int16_t temp[grid_size * grid_size][3];
float shortestPath[grid_size * grid_size * 4];
static uint8_t direction;
uint8_t path_index;


void add_path(float pram1, float param2, float param3, uint8_t param4);

void findShortestPath(){
    memset(visited, false, sizeof(visited));
    for(uint16_t i = 0; i < grid_size * grid_size; i++)
    	shortestPath[i] = 0;
    int16_t len = 2;
    int16_t curr = 1;

    temp[0][0] = ending_coordinates[0];
    temp[0][1] = ending_coordinates[1] - 1;
    temp[0][2] = -1;

    temp[1][0] = ending_coordinates[0];
    temp[1][1] = ending_coordinates[1];
    temp[1][2] = 0;
    visited[temp[0][0]][temp[0][1]] = true;
    int16_t x, y;
    while(curr != len){
        x = temp[curr][0];
        y = temp[curr][1];
        if(x == starting_coordinates[0] && y == starting_coordinates[1]){
            break;
        }
        uint8_t wall = maze[y][x];
        for(uint8_t _ = 0; _ < 4; _++){
            if(!visited[y + move[_][0]][x + move[_][1]] && !(wall & move[_][2])){
                temp[len][0] = x + move[_][1];
                temp[len][1] = y + move[_][0];
                visited[temp[len][1]][temp[len][0]] = true;
                temp[len++][2] = curr;
            }
        }
        curr++;
    }

    direction = north;
    uint8_t lastt_x;
    uint8_t last_x = temp[curr][0];
    uint8_t last_y = temp[curr][1];
    curr = temp[curr][2];
    path_index = 0;
    shortestPath[0] = 0;
    while(curr != -1){
        if(temp[curr][1] > last_y){
            switch(direction){
                case west:
                    add_path(offset45, turn_left_45, half_diagonal - offset45, south_west);
                    break;
                case east:
                    add_path(offset45, turn_right_45, half_diagonal - offset45, south_east);
                    break;
                case south:
                    shortestPath[path_index]+=square_size;
                    break;
                case north_west:
                    add_path(offset90, turn_left_90, half_diagonal - offset90, south_west);
                    break;
                case north_east:
                    add_path(offset90, turn_right_90, half_diagonal - offset90, south_east);
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_right_45, square_size - offset45, south);
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_left_45, square_size - offset45, south);
                    }
            }
        } else if(temp[curr][1] < last_y){
            switch(direction){
                case west:
                    add_path(offset45, turn_right_45, half_diagonal - offset45, north_west);
                    break;
                case east:
                    add_path(offset45, turn_left_45, half_diagonal - offset45, north_east);
                    break;
                case north:
                    shortestPath[path_index]+=square_size;
                    break;
                case south_west:
                    add_path(offset90, turn_right_90, half_diagonal - offset90, north_west);
                    break;
                case south_east:
                    add_path(offset90, turn_left_90, half_diagonal - offset90, north_east);
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_right_45, square_size - offset45, north);
                    }
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_left_45, square_size - offset45, north);
                    }
            }
        } else if(temp[curr][0] < last_x){
            switch(direction){
                case west:
                    shortestPath[path_index]+=square_size;
                    break;
                case north:
                    add_path(offset45, turn_left_45, half_diagonal - offset45, north_west);
                    break;
                case south:
                    add_path(offset45, turn_right_45, half_diagonal - offset45, south_west);
                    break;
                case north_east:
                    add_path(offset90, turn_left_90, half_diagonal - offset90, north_west);
                    break;
                case south_east:
                    add_path(offset90, turn_right_90, half_diagonal - offset90, south_west);
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        add_path(offset45, turn_left_45, square_size - offset45, west);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        add_path(offset45, turn_right_45, square_size - offset45, west);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
            }
        } else {
            switch(direction){
                case east:
                    shortestPath[path_index]+=square_size;
                    break;
                case north:
                    add_path(offset45, turn_right_45, half_diagonal - offset45, north_east);
                    break;
                case south:
                    add_path(offset45, turn_left_45, half_diagonal - offset45, south_east);
                    break;
                case north_west:
                    add_path(offset90, turn_right_90, half_diagonal - offset90, north_east);
                    break;
                case south_west:
                    add_path(offset90, turn_left_90, half_diagonal - offset90, south_east);
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        add_path(offset45, turn_right_45, square_size - offset45, east);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        add_path(offset45, turn_left_45, square_size - offset45, east);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
            }
        }
        lastt_x = last_x;
        last_x = temp[curr][0];
        last_y = temp[curr][1];
        curr = temp[curr][2];
    }
}

void add_path(float param1, float param2, float param3, uint8_t param4){
	shortestPath[path_index]-=param1;
	shortestPath[++path_index] = param2;
	shortestPath[++path_index] = param3;
	direction = param4;
}
