/*
 * file_maze.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */
#include "fill_maze.h"

#define pivot_left 100
#define pivot_right 100
#define pivot_fornt_left 100
#define pivot_fornt_right 100
#define left_wall 8
#define right_wall 4
#define top_wall 2
#define bottom_wall 1
#define front_robot 0
#define left_robot 1
#define right_robot 2

uint8_t stack[grid_size * grid_size][2];
volatile bool dma_complete;
volatile uint16_t adc_value[4];
uint8_t x, y, direction;

void set_wall(uint8_t rbl, uint8_t rbr, uint8_t rbf);

void start_fill() {
	memset(visited, 0, sizeof(visited));
	memset(maze, 0, sizeof(maze));
	stack[0][0] = starting_coordinates[0];
	stack[0][1] = starting_coordinates[1];
	dma_complete = 0;
	visited[starting_coordinates[0]][starting_coordinates[1]] = true;
	int16_t i = 1;
	x = y = 0;
	direction = west;
	bool front, left, right;
	go_straight(27, 0);
	while(i > 0){
		dma_complete = false;
		front = left = right = true;
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value, 8);
		while(dma_complete == 0);
		front = adc_value[0] < pivot_fornt_right;
		right = adc_value[1] < pivot_right;
		left  = adc_value[2] < pivot_left;
		set_wall(left, right, front);
		if()
	}
}

void set_wall(uint8_t rbl, uint8_t rbr, uint8_t rbf){
	switch(direction){
		case west:
			if(rbl){
				maze[y][x - 1] |= bottom_wall;
				if(y + 1 < grid_size){
					maze[y + 1][x - 1] |= top_wall;
				}
			}
			if(rbr){
				maze[y][x - 1] |= top_wall;
				if(y - 1 > 0){
					maze[y - 1][x - 1] |= bottom_wall;
				}
			}
			if(rbf){
				maze[y][x - 1] |= left_wall;
				if(x - 2 > 0){
					maze[y][x - 2] |= right_wall;
				}
			}
			break;
		case east:
			if(rbl){
				maze[y][x + 1] |= top_wall;
				if(y - 1 > 0){
					maze[y - 1][x + 1] |= bottom_wall;
				}
			}
			if(rbr){
				maze[y][x + 1] |= bottom_wall;
				if(y + 1 < grid_size){
					maze[y + 1][x + 1] |= top_wall;
				}
			}
			if(rbf){
				maze[y][x + 1] |= right_wall;
				if(x + 2 < grid_size){
					maze[y][x + 2] |= left_wall;
				}
			}
			break;
		case north:
			if(rbl){
				maze[y - 1][x] |= left_wall;
				if(x - 1 > 0){
					maze[y - 1][x - 1] |= right_wall;
				}
			}
			if(rbr){
				maze[y - 1][x] |= right_wall;
				if(x + 1 < grid_size){
					maze[y - 1][x + 1] |= left_wall;
				}
			}
			if(rbf){
				maze[y - 1][x] |= top_wall;
				if(y - 2 > 0){
					maze[y - 2][x] |= bottom_wall;
				}
			}
			break;
		case south:
			if(rbl){
				maze[y + 1][x] = right_wall;
				if(x + 1 < grid_size){
					maze[y + 1][x + 1] = left_wall;
				}
			}
			if(rbr){
				maze[y + 1][x] = left_wall;
				if(x - 1 > 0){
					maze[y + 1][x - 1] = right_wall;
				}
			}
			if(rbf){
				maze[y + 1][x] = bottom_wall;
				if(y + 2 < grid_size){
					maze[y + 2][x] = top_wall;
				}
			}
	}
}
