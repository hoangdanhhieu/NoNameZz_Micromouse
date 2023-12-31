#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <turn.h>
#include "definee.h"

extern double shortestPath[grid_size * grid_size * 4];
extern int8_t stack[grid_size * grid_size][3];
extern uint16_t path_index;
extern uint8_t maze[grid_size][grid_size];
extern const uint8_t starting_coordinates[2];
extern bool visited[grid_size][grid_size];
extern const uint8_t ending_coordinates[2];
void running();
void OPPath();
void running_OPPath();
void findShortestPath();
