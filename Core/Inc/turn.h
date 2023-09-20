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
#include <stdint.h>


extern const int32_t speed_levels[2];
extern int32_t PID_params[3][3];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern volatile uint8_t current_speed;
extern volatile int8_t status;
extern volatile uint16_t adc_value[4];
extern int a, b, c, d, e;

//void balance();
void u_turnf(uint8_t *direction);
void turn_left45();
void turn_right45();
void turn_left90(uint8_t *direction);
void turn_right90(uint8_t *direction);
void go_straight(float distance, bool brakee);
void brake();

#endif /* INC_TURN_H_ */
