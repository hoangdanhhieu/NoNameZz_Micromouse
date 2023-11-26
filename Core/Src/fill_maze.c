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


void found(int16_t index);
void set_wall(bool rbl, bool rbr, bool rbf);

void start_fill() {
	memset(visited, false, sizeof(visited));
	memset(maze, 0, sizeof(maze));
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	visited[starting_coordinates[1]][starting_coordinates[0]] = true;
	int16_t i = 0;
	x = starting_coordinates[0];
	y = starting_coordinates[1] + 1;
	maze[y - 1][x] |= bottom_wall;
	direction = north;
	bool frontfree, leftfree, rightfree;
	uint16_t frontValue, leftValue, rightValue;
	while(1){
		vl53l0x_GetRanging_now(rightSensor0, &frontValue);
		vl53l0x_GetRanging_now(leftSensor45, &leftValue);
		vl53l0x_GetRanging_now(rightSensor45, &rightValue);
		#if debug == 1
		ts1 = frontValue;
		ts2 = leftValue;
		ts3 = rightValue;
		#endif
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		frontfree = frontValue > HasfrontWallValue;
		leftfree  = leftValue > HasleftWallValue_45;
		rightfree = rightValue > HasrightWallValue_45;
		set_wall(!leftfree, !rightfree, !frontfree);
		visited[y][x] = true;
		if(x == ending_coordinates[0] && y == ending_coordinates[1]){
			found(i);
			maze[y][x] |= top_wall;
			break;
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
		if((leftfree && rightfree) || (leftfree && frontfree) || (rightfree && frontfree)){
			i++;
			stack[i][1] = x;
			stack[i][2] = y;
			if(direction == west){
				if(rightfree){
					stack[i][0] = turn_right_90;
					go_straight(WidthOESide, 1);
					turn_right90(&direction);
					go_straight(WidthOESide, 0);
				} else {
					stack[i][0] = straight;
					go_straight(square_size, 0);
				}
			} else if(direction == east){
				if(leftfree){
					stack[i][0] = turn_left_90;
					go_straight(WidthOESide + dbtWheels_c, 1);
					turn_left90(&direction);
					go_straight(WidthOESide - dbtWheels_c, 0);
				} else {
					stack[i][0] = straight;
					go_straight(square_size, 0);
				}
			} else {
				if(frontfree){
					stack[i][0] = straight;
					go_straight(square_size, 0);
				} else {
					stack[i][0] = turn_left_90;
					go_straight(WidthOESide + dbtWheels_c, 1);
					turn_left90(&direction);
					go_straight(WidthOESide - dbtWheels_c, 0);
				}
			}
		} else if(leftfree || rightfree || frontfree){
			if(frontfree){
				if(stack[i][0] == straight && stack[i][1] == -1){
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
				go_straight(WidthOESide + dbtWheels_c, 1);
				turn_left90(&direction);
				go_straight(WidthOESide - dbtWheels_c, 0);
			} else if(rightfree){
				i++;
				stack[i][0] = turn_right_90;
				stack[i][1] = -1;
				go_straight(WidthOESide, 1);
				turn_right90(&direction);
				go_straight(WidthOESide, 0);
			}
		} else {
			go_straight(dbtWheels_c + 20, 1);
			u_turnf(&direction);
			if(stack[i][1] != -1 &&
					((maze[stack[i][2]][stack[i][1]] & 8) != 0 || visited[stack[i][2]][stack[i][1] - 1]) &&
					((maze[stack[i][2]][stack[i][1]] & 4) != 0 || visited[stack[i][2]][stack[i][1] + 1]) &&
					((maze[stack[i][2]][stack[i][1]] & 2) != 0 || visited[stack[i][2] - 1][stack[i][1]]) &&
					((maze[stack[i][2]][stack[i][1]] & 1) != 0 || visited[stack[i][2] + 1][stack[i][1]])){
				stack[i][1] = -1;
				stack[i][2] = 1;
			}
			while(stack[i][1] == -1){
				switch(stack[i][0]){
					case straight:
						go_straight((float)stack[i][2] * square_size, 0);
						break;
					case turn_left_90:
						go_straight(WidthOESide, 1);
						turn_right90(&direction);
						go_straight(WidthOESide, 0);
						break;
					case turn_right_90:
						go_straight(WidthOESide + dbtWheels_c, 1);
						turn_left90(&direction);
						go_straight(WidthOESide - dbtWheels_c, 0);
						break;
				}
				i--;
				if(stack[i][1] != -1 &&
						((maze[stack[i][2]][stack[i][1]] & 8) != 0 || visited[stack[i][2]][stack[i][1] - 1]) &&
						((maze[stack[i][2]][stack[i][1]] & 4) != 0 || visited[stack[i][2]][stack[i][1] + 1]) &&
						((maze[stack[i][2]][stack[i][1]] & 2) != 0 || visited[stack[i][2] - 1][stack[i][1]]) &&
						((maze[stack[i][2]][stack[i][1]] & 1) != 0 || visited[stack[i][2] + 1][stack[i][1]])){
					stack[i][1] = -1;
					stack[i][2] = 1;
				}
			}
			x = stack[i][1];
			y = stack[i][2];
			switch(direction){
				case west:
					if(stack[i][0] == straight){
						if((maze[y][x] & top_wall) == 0 && !visited[y - 1][x]){
							stack[i][0] = turn_left_90;
							go_straight(WidthOESide, 1);
							turn_right90(&direction);
							go_straight(WidthOESide, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_right_90;
							go_straight(WidthOESide + dbtWheels_c, 1);
							turn_left90(&direction);
							go_straight(WidthOESide - dbtWheels_c, 0);
						}
					} else {
						stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
						go_straight(square_size, 0);
					}
					break;
				case east:
					if(stack[i][0] == straight){
						if(((maze[y][x] & top_wall) == 0) && !visited[y - 1][x]){
							stack[i][0] = turn_right_90;
							go_straight(WidthOESide + dbtWheels_c, 1);
							turn_left90(&direction);
							go_straight(WidthOESide - dbtWheels_c, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_left_90;
							go_straight(WidthOESide, 1);
							turn_right90(&direction);
							go_straight(WidthOESide, 0);
						}
					} else {
						stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
						go_straight(square_size, 0);
					}
					break;
				case north:
					if(stack[i][0] == straight){
						if((maze[y][x] & left_wall) == 0 && !visited[y][x - 1]){
							stack[i][0] = turn_right_90;
							go_straight(WidthOESide + dbtWheels_c, 1);
							turn_left90(&direction);
							go_straight(WidthOESide - dbtWheels_c, 0);
						} else if((maze[y][x] & right_wall) == 0 && !visited[y][x + 1]) {
							stack[i][0] = turn_left_90;
							go_straight(WidthOESide, 1);
							turn_right90(&direction);
							go_straight(WidthOESide, 0);
						}
					} else {
						stack[i][0] = (stack[i][0] == turn_left_90) ? turn_right_90 : turn_left_90;
						go_straight(square_size, 0);
					}
					break;
				case south:
					if(stack[i][0] == straight){
						if(((maze[y][x] & left_wall) == 0) && !visited[y][x - 1]){
							stack[i][0] = turn_left_90;
							go_straight(WidthOESide, 1);
							turn_right90(&direction);
							go_straight(WidthOESide, 0);
						} else if(((maze[y][x] & right_wall) == 0) && !visited[y][x + 1]){
							stack[i][0] = turn_right_90;
							go_straight(WidthOESide + dbtWheels_c, 1);
							turn_left90(&direction);
							go_straight(WidthOESide - dbtWheels_c, 0);
						}
					} else if(stack[i][0] == turn_left_90){
						if(((maze[y][x] & right_wall) == 0) && !visited[y][x + 1]){
							stack[i][0] = straight;
							go_straight(WidthOESide + dbtWheels_c, 1);
							turn_left90(&direction);
							go_straight(WidthOESide - dbtWheels_c, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_right_90;
							go_straight(square_size, 0);
						}
					} else {
						if(((maze[y][x] & left_wall) == 0) && !visited[y][x - 1]){
							stack[i][0] = straight;
							go_straight(WidthOESide, 1);
							turn_right90(&direction);
							go_straight(WidthOESide, 0);
						} else if(((maze[y][x] & bottom_wall) == 0) && !visited[y + 1][x]){
							stack[i][0] = turn_left_90;
							go_straight(square_size, 0);
						}
					}
					break;
			}
		}
	}
	for(int n = 0; n < grid_size; n++){
		for(int m = 0; m < grid_size; m++){
			if(!visited[n][m]){
				maze[n][m] |= 15;
				if(n > 0){ maze[n - 1][m] |= bottom_wall; }
				if(n + 1 < grid_size){ maze[n + 1][m] |= top_wall; }
				if(m > 0){ maze[n][m - 1] |= right_wall; }
				if(m + 1 < grid_size){ maze[n][m + 1] |= left_wall; }
			}
		}
	}
}
void found(int16_t index){
	switch(direction){
		case west:
			go_straight(WidthOESide, 1);
			turn_right90(&direction);
			go_straight(WidthOESide + square_size, 1);
			break;
		case east:
			go_straight(WidthOESide + dbtWheels_c, 1);
			turn_left90(&direction);
			go_straight(WidthOESide + square_size - dbtWheels_c, 1);
			break;
		case north:
			go_straight(square_size * 2, 1);
			break;
	}
}

void set_wall(bool rbl, bool rbr, bool rbf){
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
