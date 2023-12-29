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
#include <inttypes.h>



extern double P_params[2];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern volatile int8_t status;
extern volatile uint8_t Rmode;
extern int16_t length_stack;
extern const int32_t speed_levels[3][2];
extern int a, b, c, d, e;
extern VL53L0X_Dev_t *pMyDevice[n_vl53l0x];
extern uint8_t uart_buffer[50];
extern UART_HandleTypeDef huart3;

//void balance();
void u_turnf(uint8_t *direction);
void turn_left45(uint8_t *direction);
void turn_right45(uint8_t *direction);
void turn_left90(uint8_t *direction);
void turn_right90(uint8_t *direction);
void go_straight(double distance, bool brakee, int8_t next);
void brake(uint8_t mode);

#endif /* INC_TURN_H_ */
