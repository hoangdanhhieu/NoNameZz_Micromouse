/*
 * fill_maze.h
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */

#ifndef INC_FILL_MAZE_H_
#define INC_FILL_MAZE_H_

#include <stdint.h>
#include "turn.h"
#include "definee.h"
#include <string.h>
#include <stdbool.h>

extern ADC_HandleTypeDef hadc1;
extern volatile bool dma_complete;
extern const uint8_t starting_coordinates[2];
extern uint8_t maze[grid_size][grid_size];
extern bool visited[grid_size][grid_size];
extern volatile uint8_t current_speed;

#endif /* INC_FILL_MAZE_H_ */
