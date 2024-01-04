/*
 * path_optimization.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */

#include "path_optimization.h"


const int8_t move[4][3] = {{0, -1, 8}, {0, 1, 4}, {-1, 0, 2}, {1, 0, 1}};
const double half_diagonal = sqrt(pow(square_size/2, 2) + pow(square_size/2, 2)) + 3;
const double offset45 = 0; //(float)tan(22.5 * M_PI / 180) * dbtWheels_r;
const double offset90 = 0; //dbtWheels_r;

int16_t temp[grid_size * grid_size][3];
double shortestPath[grid_size * grid_size * 4];
static uint8_t direction;
uint16_t path_index;




void running_OPPath(){
	int32_t t;
	uint8_t direction = north;
	for(int i = 1; i <= path_index; i++){
		t = round(shortestPath[i]);
		switch(t){
			case turn_left_90:
				turnLeftHere;
				break;
			case turn_right_90:
				turnRightHere;
				break;
			default:
				go_straight(shortestPath[i] + 20, 0, -1);
		}
	}
	brake(2);
}

void OPPath(){
	shortestPath[0] = 0;
	path_index = 0;
	for(int i = 0; i <= length_stack; i++){
		if(stack[i][0] == straight){
			if(stack[i][1] == -1){
				if(shortestPath[path_index] > 0){
					shortestPath[path_index] += (300 * stack[i][2]);
				} else {
					shortestPath[++path_index] = 300 * stack[i][2];
				}
			} else {
				if(shortestPath[path_index] > 0){
					shortestPath[path_index]+=300;
				} else {
					shortestPath[++path_index] = 300;
				}
			}
		} else {
			shortestPath[++path_index] = stack[i][0];
		}
	}
}


void running(){
	#if debug == 1
	sprintf((char*)uart_buffer, "shortestPath:[\n");
	HAL_UART_Transmit(&huart3, uart_buffer, sizeof(uart_buffer), 10);
	for(int i = 0; i <= path_index; i++){
		sprintf((char*)uart_buffer, "%d, ", (int)round(shortestPath[i]));
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof(uart_buffer), 10);
	}
	sprintf((char*)uart_buffer, "\n]\n");
	HAL_UART_Transmit(&huart3, uart_buffer, sizeof(uart_buffer), 10);
	#endif
	uint8_t direction = north;
	int32_t t;
	for(int i = 0; i <= path_index; i++){
		t = (int32_t)round(shortestPath[i]);
		switch(t){
			case turn_left_45:
				turn_left45(&direction);
				break;
			case turn_right_45:
				turn_right45(&direction);
				break;
			case turn_left_90:
				turn_left90(&direction);
				break;
			case turn_right_90:
				turn_right90(&direction);
				break;
			default:
				if(direction == north || direction == south || direction == west || direction == east){
					if(t >= 300){
						if(t % 300 == 0){
							go_straight((double)round(shortestPath[i]/300) * 300, 1, -1);
						} else {
							go_straight((double)round(shortestPath[i]/300) * 300, 0, -1);
							go_straight((double)(t % 300), 1, 0);
						}
					} else {
						go_straight(shortestPath[i], 1, 0);
					}
				} else {
					go_straight(shortestPath[i], 1, -2);
				}
		}
	}
}

void add_path(double pram1, double param2, double param3, uint8_t param4);
void findShortestPath(){
    memset(visited, false, sizeof(visited));
    for(uint16_t i = 0; i < grid_size * grid_size; i++)
    	shortestPath[i] = 0;

    int16_t len = 3;
    int16_t curr = 2;

    temp[0][0] = ending_coordinates[0];
    temp[0][1] = (int16_t)ending_coordinates[1] - 2;
    temp[0][2] = -1;

    temp[1][0] = ending_coordinates[0];
    temp[1][1] = (int16_t)ending_coordinates[1] - 1;
    temp[1][2] = 0;

    temp[2][0] = ending_coordinates[0];
    temp[2][1] = ending_coordinates[1];
    temp[2][2] = 1;

    visited[temp[2][1]][temp[2][0]] = true;
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
                    add_path(offset45, turn_left_45, half_diagonal, south_west);
                    break;
                case east:
                    add_path(offset45, turn_right_45, half_diagonal, south_east);
                    break;
                case south:
                    shortestPath[path_index]+=square_size;
                    break;
                case north_west:
                    add_path(offset90, turn_left_90, half_diagonal, south_west);
                    break;
                case north_east:
                    add_path(offset90, turn_right_90, half_diagonal, south_east);
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_right_45, square_size, south);
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_left_45, square_size, south);
                    }
            }
        } else if(temp[curr][1] < last_y){
            switch(direction){
                case west:
                    add_path(offset45, turn_right_45, half_diagonal, north_west);
                    break;
                case east:
                    add_path(offset45, turn_left_45, half_diagonal, north_east);
                    break;
                case north:
                    shortestPath[path_index]+=square_size;
                    break;
                case south_west:
                    add_path(offset90, turn_right_90, half_diagonal, north_west);
                    break;
                case south_east:
                    add_path(offset90, turn_left_90, half_diagonal, north_east);
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_right_45, square_size, north);
                    }
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        shortestPath[path_index]+=half_diagonal;
                    } else {
                        add_path(offset45, turn_left_45, square_size, north);
                    }
            }
        } else if(temp[curr][0] < last_x){
            switch(direction){
                case west:
                    shortestPath[path_index]+=square_size;
                    break;
                case north:
                    add_path(offset45, turn_left_45, half_diagonal, north_west);
                    break;
                case south:
                    add_path(offset45, turn_right_45, half_diagonal, south_west);
                    break;
                case north_east:
                    add_path(offset90, turn_left_90, half_diagonal, north_west);
                    break;
                case south_east:
                    add_path(offset90, turn_right_90, half_diagonal, south_west);
                    break;
                case north_west:
                    if(lastt_x > last_x){
                        add_path(offset45, turn_left_45, square_size, west);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_west:
                    if(lastt_x > last_x){
                        add_path(offset45, turn_right_45, square_size, west);
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
                    add_path(offset45, turn_right_45, half_diagonal, north_east);
                    break;
                case south:
                    add_path(offset45, turn_left_45, half_diagonal, south_east);
                    break;
                case north_west:
                    add_path(offset90, turn_right_90, half_diagonal, north_east);
                    break;
                case south_west:
                    add_path(offset90, turn_left_90, half_diagonal, south_east);
                    break;
                case north_east:
                    if(lastt_x < last_x){
                        add_path(offset45, turn_right_45, square_size, east);
                    } else {
                        shortestPath[path_index]+=half_diagonal;
                    }
                    break;
                case south_east:
                    if(lastt_x < last_x){
                        add_path(offset45, turn_left_45, square_size, east);
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

void add_path(double param1, double param2, double param3, uint8_t param4){
	shortestPath[path_index]-=param1;
	shortestPath[++path_index] = param2;
	shortestPath[++path_index] = param3;
	direction = param4;
}
