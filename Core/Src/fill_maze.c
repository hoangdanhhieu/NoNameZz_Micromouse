/*
 * file_maze.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */
#include "fill_maze.h"

#define left_wall 8
#define right_wall 4
#define top_wall 2
#define bottom_wall 1



int8_t stack[grid_size * grid_size][3];
int8_t x, y;
static uint8_t direction;
const float d2 = (float)(square_size - (halfSize_MicroMouse * 2))/2;

void found(int16_t index);
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
	maze[y][x] |= bottom_wall;
	direction = west;
	bool frontfree, leftfree, rightfree;
	while(i > 0){
		frontfree = leftfree = rightfree = true;
		frontfree = adc_value[0] < frontWallValue;
		rightfree = adc_value[1] < rightWallValue;
		leftfree  = adc_value[2] < leftWallValue;
		set_wall(!leftfree, !rightfree, !frontfree);
		if(x == ending_coordinates[0] && y == ending_coordinates[1]){
			found(i);
			maze[y][x] |= top_wall;
			frontfree = rightfree = leftfree = false;
		}
		switch(direction){
			case west:
				frontfree = frontfree && !visited[y][x - 1];
				leftfree  = leftfree && !visited[y + 1][x];
				rightfree  = rightfree && !visited[y - 1][x];
				break;
			case east:
				frontfree = frontfree && !visited[y][x + 1];
				leftfree  = leftfree && !visited[y - 1][x];
				rightfree  = rightfree && !visited[y + 1][x];
				break;
			case north:
				frontfree = frontfree && !visited[y - 1][x];
				leftfree  = leftfree && !visited[y][x - 1];
				rightfree  = rightfree && !visited[y][x + 1];
				break;
			case south:
				frontfree = frontfree && !visited[y + 1][x];
				leftfree  = leftfree && !visited[y][x + 1];
				rightfree  = rightfree && !visited[y][x - 1];
				break;
		}
		visited[y][x] = true;
		if((leftfree && rightfree) || (leftfree && frontfree) || (rightfree && frontfree)){
			i++;
			stack[i][1] = x;
			stack[i][2] = y;
			if(frontfree){
				stack[i][0] = straight;
				go_straight(square_size, 0);
			} else if(leftfree){
				stack[i][0] = turn_left_90;
				go_straight(d2, 1);
				turn_left90(&direction);
				go_straight(d2, 0);
			} else if(rightfree){
				stack[i][0] = turn_right_90;
				go_straight(d2, 1);
				turn_right90(&direction);
				go_straight(d2, 0);
			}
		} else if(leftfree || rightfree || frontfree){
			if(frontfree){
				if(stack[i][0] == straight){
					stack[i][2]++;
				} else {
					i++;
					stack[i][0] = straight;
					stack[i][1] = -1;
					stack[i][2] = 1;
				}
				go_straight(square_size, 0);
			} else if(leftfree){
				i++;
				stack[i][0] = turn_left_90;
				stack[i][1] = -1;
				go_straight(d2, 1);
				turn_left90(&direction);
				go_straight(d2, 0);
			} else if(rightfree){
				i++;
				stack[i][0] = turn_right_90;
				stack[i][1] = -1;
				go_straight(d2, 1);
				turn_right90(&direction);
				go_straight(d2, 0);
			}
		} else {
			go_straight(d2, 1);
			u_turnf(&direction);
			go_straight(d2, 0);
			while(i != 0 && stack[i][1] == -1){
				switch(stack[i][0]){
					case straight:
						go_straight((float)stack[i][2] * square_size, 0);
						break;
					case turn_left_90:
						go_straight(d2, 1);
						turn_right90(&direction);
						go_straight(d2, 0);
						break;
					case turn_right_90:
						go_straight(d2, 1);
						turn_left90(&direction);
						go_straight(d2, 0);
						break;
				}
				i--;
				if((i != 0 && stack[i][1] != -1) &&
						((maze[stack[i][1]][stack[i][0]] & 8) != 0 || visited[y][x - 1]) &&
						((maze[stack[i][1]][stack[i][0]] & 4) != 0 || visited[y][x + 1]) &&
						((maze[stack[i][1]][stack[i][0]] & 2) != 0 || visited[y - 1][x]) &&
						((maze[stack[i][1]][stack[i][0]] & 1) != 0 || visited[y + 1][x])){
					switch(stack[i][0]){
						case(straight):
							stack[i][0] = straight;
							stack[i][1] = -1;
							stack[i][2] = 1;
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
				break;
			}
			x = stack[i][1];
			y = stack[i][2];
			switch(direction){
				case west:
					if(stack[i][0] == straight){
						if((maze[y][x] & top_wall) == 0 && !visited[y - 1][x]){
							stack[i][0] = turn_left_90;
							go_straight(d2, 1);
							turn_right90(&direction);
							go_straight(d2, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_right_90;
							go_straight(d2, 1);
							turn_left90(&direction);
							go_straight(d2, 0);
						}
					} else {
						if(((maze[y][x] & left_wall) == 0) && !visited[y][x - 1]){
							stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
							go_straight(square_size, 0);
						}
					}
					break;
				case east:
					if(stack[i][0] == straight){
						if(((maze[y][x] & top_wall) == 0) && !visited[y - 1][x]){
							stack[i][0] = turn_right_90;
							go_straight(d2, 1);
							turn_left90(&direction);
							go_straight(d2, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_left_90;
							go_straight(d2, 1);
							turn_right90(&direction);
							go_straight(d2, 0);
						}
					} else {
						if((maze[y][x] & right_wall) == 0 && !visited[y][x + 1]){
							stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
							go_straight(square_size, 0);
						}
					}
					break;
				case north:
					if(stack[i][0] == straight){
						if((maze[y][x] & left_wall) == 0 && !visited[y][x - 1]){
							stack[i][0] = turn_right_90;
							go_straight(d2, 1);
							turn_left90(&direction);
							go_straight(d2, 0);
						} else if((maze[y][x] & right_wall) == 0 && !visited[y][x + 1]) {
							stack[i][0] = turn_left_90;
							go_straight(d2, 1);
							turn_right90(&direction);
							go_straight(d2, 0);
						}
					} else {
						if((maze[y][x] & top_wall) == 0 && !visited[y - 1][x]){
							stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
							go_straight(square_size, 0);
						}
					}
					break;
				case south:
					if(stack[i][0] == straight){
						if(((maze[y][x] & left_wall) == 0) && !visited[y][x - 1]){
							stack[i][0] = turn_left_90;
							go_straight(d2, 1);
							turn_right90(&direction);
							go_straight(d2, 0);
						} else if(((maze[y][x] & right_wall) == 0) && !visited[y][x + 1]){
							stack[i][0] = turn_right_90;
							go_straight(d2, 1);
							turn_left90(&direction);
							go_straight(d2, 0);
						}
					} else {
						if((maze[y][x] & bottom_wall) == 0 && !visited[y + 1][x]){
							stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
							go_straight(square_size, 0);
						}
					}
					break;
			}
		}
		for(int n = 0; n < grid_size; n++){
			for(int m = 0; m < grid_size; m++){
				if(!visited[y][x]){
					maze[x][y] |= 15;
				}
			}
		}
	}
}

void found(int16_t index){
	switch(direction){
		case west:
			go_straight(d2, 1);
			turn_right90(&direction);
			go_straight(d2, 1);
			stack[++index][0] = turn_right_90;
			stack[index][1] = x;
			stack[index][2] = y;
			break;
		case east:
			go_straight(d2, 1);
			turn_left90(&direction);
			go_straight(d2, 1);
			stack[++index][0] = turn_left_90;
			stack[index][1] = x;
			stack[index][2] = y;
			break;
		case north:
			go_straight(square_size, 1);
			stack[++index][0] = straight;
			stack[index][1] = x;
			stack[index][2] = y;
			break;
	}
	while(--index > 0){
		stack[index][1] = -1;
		stack[index][2] = 1;
	}
}

void set_wall(uint8_t rbl, uint8_t rbr, uint8_t rbf){
	switch(direction){
		case west:
			x--;
			if(rbl){
				maze[y][x] |= bottom_wall;
				if(y + 1 < grid_size){ maze[y + 1][x] |= top_wall; }
			}
			if(rbr){
				maze[y][x] |= top_wall;
				if(y > 0){ maze[y - 1][x] |= bottom_wall; }
			}
			if(rbf){
				maze[y][x] |= left_wall;
				if(x > 0){ maze[y][x - 1] |= right_wall; }
			}
			break;
		case east:
			x++;
			if(rbl){
				maze[y][x] |= top_wall;
				if(y > 0){ maze[y - 1][x] |= bottom_wall; }
			}
			if(rbr){
				maze[y][x] |= bottom_wall;
				if(y + 1 < grid_size){ maze[y + 1][x] |= top_wall; }
			}
			if(rbf){
				maze[y][x] |= right_wall;
				if(x + 1 < grid_size){ maze[y][x + 1] |= left_wall; }
			}
			break;
		case north:
			y--;
			if(rbl){
				maze[y][x] |= left_wall;
				if(x > 0){ maze[y][x - 1] |= right_wall; }
			}
			if(rbr){
				maze[y][x] |= right_wall;
				if(x + 1 < grid_size){ maze[y][x + 1] |= left_wall; }
			}
			if(rbf){
				maze[y][x] |= top_wall;
				if(y > 0){ maze[y - 1][x] |= bottom_wall; }
			}
			break;
		case south:
			y++;
			if(rbl){
				maze[y][x] |= right_wall;
				if(x + 1 < grid_size){ maze[y][x + 1] |= left_wall; }
			}
			if(rbr){
				maze[y][x] |= left_wall;
				if(x > 0){ maze[y][x - 1] |= right_wall; }
			}
			if(rbf){
				maze[y][x] |= bottom_wall;
				if(y + 1 < grid_size){ maze[y + 1][x] |= top_wall; }
			}
			break;
	}
}
