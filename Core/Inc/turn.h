/*
 * turn.h
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */

#ifndef INC_TURN_H_
#define INC_TURN_H_

#include <math.h>
#include "stm32f1xx_hal.h"
#include "definee.h"
#include <stdbool.h>


extern const uint16_t speed_levels[3][2];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t current_speed;
extern volatile int8_t status;
extern int c;

//void balance();
void u_turnf();
void turn_left45();
void turn_right45();
void turn_left90();
void turn_right90();
void go_straight(float distance, bool brake);
void brake();

#endif /* INC_TURN_H_ */
