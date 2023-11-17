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
#include <vl53l0x_init.h>


extern float P_params[2];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern volatile int8_t status;
extern volatile uint8_t Rmode;
extern const int32_t speed_levels[3];
extern volatile uint8_t flag_uturn;
extern int a, b, c, d, e;
extern VL53L0X_Dev_t *pMyDevice[n_vl53l0x];

//void balance();
void u_turnf(uint8_t *direction);
void turn_left45();
void turn_right45();
void turn_left90(uint8_t *direction);
void turn_right90(uint8_t *direction);
void go_straight(float distance, bool brakee);
void running_left_motor(uint8_t mode);
void running_right_motor(uint8_t mode);
void brake(uint8_t mode);

#endif /* INC_TURN_H_ */
