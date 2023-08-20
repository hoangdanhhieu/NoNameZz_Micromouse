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



int8_t stack[grid_size * grid_size][3];
volatile bool dma_complete;
volatile uint16_t adc_value[4];
int8_t x, y;
static uint8_t direction;
const float d2 = (float)(square_size - (halfSize_MicroMouse * 2))/2;
const float d3 = d2 * 2;

void set_wall(uint8_t rbl, uint8_t rbr, uint8_t rbf);

void start_fill() {
	memset(visited, false, sizeof(visited));
	memset(maze, 0, sizeof(maze));
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	current_speed = 0;
	stack[0][0] = straight;
	stack[0][1] = starting_coordinates[0];
	stack[0][2] = starting_coordinates[1];
	visited[starting_coordinates[0]][starting_coordinates[1]] = true;
	int16_t i = 1;
	x = starting_coordinates[0] - 1;
	y = starting_coordinates[1] - 1;
	direction = west;
	bool front, left, right;
	backwards();
	go_straight(square_size - d1 - d2, 0);
	while(i > 0){
		dma_complete = false;
		front = left = right = true;
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value, 8);
		while(dma_complete == 0);
		HAL_Delay(10);
		front = adc_value[0] < pivot_fornt_right;
		right = adc_value[1] < pivot_right;
		left  = adc_value[2] < pivot_left;
		set_wall(!left, !right, !front);
		switch(direction){
			case west:
				x--;
				front = front && !visited[y][x - 1];
				left  = left && !visited[y + 1][x];
				right  = right && !visited[y - 1][x];
				break;
			case east:
				x++;
				front = front && !visited[y][x + 1];
				left  = left && !visited[y - 1][x];
				right  = right && !visited[y + 1][x];
				break;
			case north:
				y--;
				front = front && !visited[y - 1][x];
				left  = left && !visited[y][x - 1];
				right  = right && !visited[y][x + 1];
				break;
			case south:
				y++;
				front = front && !visited[y + 1][x];
				left  = left && !visited[y][x + 1];
				right  = right && !visited[y][x - 1];
				break;
		}
		visited[y][x] = true;
		if((left && right) || (left && front) || (right && front)){
			i++;
			stack[i][1] = x;
			stack[i][2] = y;
			if(front){
				stack[i][0] = straight;
				go_straight(square_size, 0);
			} else if(left){
				stack[i][0] = turn_left_90;
				go_straight(d3, 1);
				turn_left90(&direction);
			} else if(right){
				stack[i][0] = turn_right_90;
				go_straight(d3, 1);
				turn_right90(&direction);
			}
		} else if(left || right || front){
			if(front){
				if(stack[i][0] == -1){
					stack[i][1]++;
				} else {
					i++;
					stack[i][0] = -1;
					stack[i][1] = 1;
				}
				go_straight(square_size, 0);
			} else if(left){
				i++;
				stack[i][0] = turn_left_90;
				stack[i][1] = -1;
				go_straight(d3, 1);
				turn_left90(&direction);
			} else if(right){
				i++;
				stack[i][0] = turn_right_90;
				stack[i][1] = -1;
				go_straight(d3, 1);
				turn_right90(&direction);
			}
		} else {
			go_straight(d3, 1);
			u_turnf(&direction);
			while(i != 0 && (stack[i][0] == -1 || stack[i][1] == -1)){
				if(stack[i][0] == -1){
					go_straight((float)stack[i][0] * 180, 0);
				} else {
					if(stack[i][0] == turn_left_90){
						go_straight(d3, 1);
						turn_right90(&direction);
					} else {
						go_straight(d3, 1);
						turn_left90(&direction);
					}
				}
				i--;
				if((i != 0 && !(stack[i][0] == -1 || stack[i][1] == -1)) &&
						((maze[stack[i][1]][stack[i][0]] & 8) == 0 || visited[y][x - 1]) &&
						((maze[stack[i][1]][stack[i][0]] & 4) == 0 || visited[y][x + 1]) &&
						((maze[stack[i][1]][stack[i][0]] & 2) == 0 || visited[y - 1][x]) &&
						((maze[stack[i][1]][stack[i][0]] & 1) == 0 || visited[y + 1][x])){
					switch(stack[i][0]){
						case(straight):
							stack[i][0] = -1;
							stack[i][1] = 1;
							break;
						case(turn_left_90):
							stack[i][0] = turn_right_90;
							stack[i][1] = -1;
							break;
						case(turn_right_90):
							stack[i][0] = turn_left_90;
							stack[i][1] = -1;
							break;
					}
				}
			}
			if(i == 0){
				u_turnf(&direction);
				backwards();
				break;
			}
			x = stack[i][1];
			y = stack[i][2];
			switch(direction){
				case west:
					switch(stack[i][0]){
						case straight:      //going from the east
							if(stack[i + 1][0] == -1){ go_straight(d3, 1); }
							if(((maze[y][x] & 2) == 0) && !visited[y - 1][x]){
								stack[i][0] = turn_left_90;
								turn_right90(&direction);
							} else if(((maze[y][x] & 1) == 0) && !visited[y + 1][x]){
								stack[i][0] = turn_right_90;
								turn_left90(&direction);
							}
							break;
						case turn_left_90:  //going from the south and turn left
							if(((maze[y][x] & 8) == 0) && !visited[y][x - 1]){
								stack[i][0] = turn_right_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							 break;
						case turn_right_90: //going from the north and turn right
							if(((maze[y][x] & 8) == 0) && !visited[y][x - 1]){
								stack[i][0] = turn_left_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
					}
					break;
				case east:
					switch(stack[i][0]){
						case straight:       //going from west
							if(((maze[y][x] & 2) == 0) && !visited[y - 1][x]){
								stack[i][0] = turn_right_90;
								turn_left90(&direction);
							} else if(((maze[y][x] & 1) == 0) && !visited[y + 1][x]){
								stack[i][0] = turn_left_90;
								turn_right90(&direction);
							}
							break;
						case turn_left_90:   //going from north and turn left
							if(((maze[y][x] & 4) == 0) && !visited[y][x + 1]){
								stack[i][0] = turn_right_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
						case turn_right_90:  //going from south and turn right
							if(((maze[y][x] & 4) == 0) && !visited[y][x + 1]){
								stack[i][0] = turn_left_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
					}
					break;
				case north:
					switch(stack[i][0]){
						case straight:       //going from south
							if(((maze[y][x] & 8) == 0) && !visited[y][x - 1]){
								stack[i][0] = turn_right_90;
								turn_left90(&direction);
							} else if(((maze[y][x] & 4) == 0) && !visited[y][x + 1]){
								stack[i][0] = turn_left_90;
								turn_right90(&direction);
							}
							break;
						case turn_left_90:   //going from west and turn left
							if(((maze[y][x] & 2) == 0) && !visited[y - 1][x]){
								stack[i][0] = turn_right_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
						case turn_right_90:  //going from east and turn right
							if(((maze[y][x] & 2) == 0) && !visited[y - 1][x]){
								stack[i][0] = turn_left_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
					}
					break;
				case south:
					switch(stack[i][0]){
						case straight:       //going from north
							if(((maze[y][x] & 8) == 0) && !visited[y][x - 1]){
								stack[i][0] = turn_left_90;
								turn_right90(&direction);
							} else if(((maze[y][x] & 4) == 0) && !visited[y][x + 1]){
								stack[i][0] = turn_right_90;
								turn_left90(&direction);
							}
							break;
						case turn_left_90:   //going from east and turn left
							if(((maze[y][x] & 1) == 0) && !visited[y + 1][x]){
								stack[i][0] = turn_right_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
						case turn_right_90:  //going from west and turn right
							if(((maze[y][x] & 1) == 0) && !visited[y + 1][x]){
								stack[i][0] = turn_left_90;
								go_straight(square_size * 2 - ((stack[i + 1][0] == -1) ? 0 : d3), 1);
							}
							break;
					}
					break;
			}
		}
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
				maze[y + 1][x] |= right_wall;
				if(x + 1 < grid_size){
					maze[y + 1][x + 1] |= left_wall;
				}
			}
			if(rbr){
				maze[y + 1][x] |= left_wall;
				if(x - 1 > 0){
					maze[y + 1][x - 1] |= right_wall;
				}
			}
			if(rbf){
				maze[y + 1][x] |= bottom_wall;
				if(y + 2 < grid_size){
					maze[y + 2][x] |= top_wall;
				}
			}
			break;
	}
}
