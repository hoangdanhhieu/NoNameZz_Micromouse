/*
 * path_optimization.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */

#include "path_optimization.h"


const int8_t move[4][3] = {{0, -1, 8}, {0, 1, 4}, {-1, 0, 2}, {1, 0, 1}};
const float half_diagonal = sqrt(pow(90, 2) + pow(90, 2));
const float offset45 = (float)tan(22.5 * M_PI / 180) * halfSize_MicroMouse;
const float offset90 = halfSize_MicroMouse;

uint8_t temp[grid_size * grid_size][3]; // [i, j, last]
float shortestPath[grid_size * grid_size * 4];
uint8_t path_index;

void findShortestPath(){
    memset(visited, false, sizeof(visited));
    for(uint16_t i = 0; i < grid_size * grid_size; i++)
    	shortestPath[i] = 0;
    uint16_t len = 1;
    uint16_t curr = 0;
    temp[0][0] = ending_coordinates[0];
    temp[0][1] = ending_coordinates[1];
    temp[0][2] = 0;
    visited[temp[0][0]][temp[0][1]] = true;
    uint16_t x, y;
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

    uint8_t direction = west;
    uint8_t lastt_x;
    uint8_t last_x = temp[curr][0];
    uint8_t last_y = temp[curr][1];
    curr = temp[curr][2];
    path_index = 0;
    shortestPath[0] = 0;
    while(!(temp[curr][2] == 0 && curr == 0)){
        if(temp[curr][1] > last_y){
            switch(direction){
                case west:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_left_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = south_west;
                    break;
                case east:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_right_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = south_east;
                    break;
                case south:
                    shortestPath[path_index]+=square_size;
                    break;
                case north_west:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_left_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = south_west;
                    break;
                case north_east:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_right_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = south_east;
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_right_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = south;
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_left_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = south;
                    }
            }
        } else if(temp[curr][1] < last_y){
            switch(direction){
                case west:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_right_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = north_west;
                    break;
                case east:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_left_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = north_east;
                    break;
                case north:
                    shortestPath[path_index]+=square_size;
                    break;
                case south_west:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_right_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = north_west;
                    break;
                case south_east:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_right_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = north_east;
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_right_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = north;
                    }
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_left_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = north;
                    }
            }
        } else if(temp[curr][0] < last_x){
            switch(direction){
                case west:
                    shortestPath[path_index]+=square_size;
                    break;
                case north:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_left_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = north_west;
                    break;
                case south:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_right_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = south_west;
                    break;
                case north_east:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_left_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = north_west;
                    break;
                case south_east:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_right_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = south_west;
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_left_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = west;
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_right_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = west;
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
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_right_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = north_east;
                    break;
                case south:
                    shortestPath[path_index]-=offset45;
                    shortestPath[++path_index] = turn_left_45;
                    shortestPath[++path_index] = half_diagonal - offset45;
                    direction = south_east;
                    break;
                case north_west:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_right_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = north_east;
                    break;
                case south_west:
                    shortestPath[path_index]-=offset90;
                    shortestPath[++path_index] = turn_left_90;
                    shortestPath[++path_index] = half_diagonal - offset90;
                    direction = south_east;
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_right_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = east;
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]-=offset45;
                        shortestPath[++path_index] = turn_left_45;
                        shortestPath[++path_index] = square_size - offset45;
                        direction = east;
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





