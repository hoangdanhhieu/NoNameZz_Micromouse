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

extern const uint8_t starting_coordinates[2];
extern const uint8_t ending_coordinates[2];
extern uint8_t maze[grid_size][grid_size];
extern bool visited[grid_size][grid_size];
extern int8_t stack[grid_size * grid_size][3];
extern VL53L0X_Dev_t *pMyDevice[n_vl53l0x];
extern uint16_t ts1, ts2, ts3, ts4, ts5, ts6;
extern uint8_t debug;
void start_fill();
#endif /* INC_FILL_MAZE_H_ */
