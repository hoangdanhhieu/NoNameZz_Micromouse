/*
 * turn.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */
#include "turn.h"
#include "math.h"

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

const float wheels_arc = (float)wheels_radius * M_PI * 2;
const float counts_per_1mm = (float)encoder_resolution / wheels_arc;

const float uturn_arc_en = (float)turning_radius * M_PI * 2 * (180.0/360) * counts_per_1mm;
const float turn90_arc_en = (float)turning_radius * M_PI * 4 * (90.0/360) * counts_per_1mm;
const float turn45_arc_en = (float)turning_radius * M_PI * 4 * (45.0/360) * counts_per_1mm;
volatile uint8_t flag_uturn;


void running_left_motor(uint8_t mode, uint16_t speed){
	if(mode == 0){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, speed);
	} else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
	}
}

void running_right_motor(uint8_t mode, uint16_t speed){
	if(mode == 0){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	} else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
	}
}

void u_turnf(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = east;   break;
		case east:  *direction = west;   break;
		case north: *direction = south;  break;
		case south: *direction = north;  break;
	}
	#if debug == 1
		sprintf((char*)uart_buffer, "uturn\n");
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof (uart_buffer), 10);
	#endif

	uint16_t last = 2702;
	while(last != TIM3->CNT){
		last = TIM3->CNT;
		HAL_Delay(50);
	}

	uint16_t en = round(uturn_arc_en);
	__HAL_TIM_SET_COUNTER(&htim2, en);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = u_turn;

	uint16_t speed = 400;
	int32_t P;
	while(status != 0){
		P = ((int32_t)TIM3->CNT - ((int32_t)en - TIM2->CNT)) * 5;
		running_left_motor(1, speed + P);
		running_right_motor(0, speed - P);
		#if debug == 1
		a = (int32_t)en - TIM2->CNT;
		b = TIM3->CNT;
		#endif
	}
	running_left_motor(0, 500);
	running_right_motor(1, 500);
	HAL_Delay(30);
	brake(2);
}

void turn_left45() {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en));

	#if debug == 1
		sprintf((char*)uart_buffer, "turn_left45\n");
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof (uart_buffer), 10);
	#endif
	status = turn_left_45;

	brake(0);
	running_right_motor(0, speed_levels[Rmode]);
	while(status != 0);
	brake(1);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_right45() {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, round(turn45_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);


	#if debug == 1
		sprintf((char*)uart_buffer, "turn_right45\n");
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof (uart_buffer), 10);
	#endif
	status = turn_right_45;

	brake(1);
	running_left_motor(0, speed_levels[Rmode]);
	while(status != 0);
	brake(0);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_left90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = south; break;
		case east:  *direction = north; break;
		case north: *direction = west;  break;
		case south: *direction = east;  break;
	}
	#if debug == 1
		sprintf((char*)uart_buffer, "turn_left90\n");
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof (uart_buffer), 10);
	#endif

	uint16_t last = 2702;
	while(last != TIM3->CNT){
		last = TIM3->CNT;
		HAL_Delay(50);
	}

	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en));
	status = turn_left_90;

	while(status != 0){
		running_right_motor(0, 400 + 200 * (float)TIM3->CNT/turn90_arc_en);
	}
	running_right_motor(1, 500);
	HAL_Delay(50);
	brake(1);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_right90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north; break;
		case east:  *direction = south; break;
		case north: *direction = east;  break;
		case south: *direction = west;  break;
	}

	#if debug == 1
	sprintf((char*)uart_buffer, "turn_right90\n");
	HAL_UART_Transmit(&huart3, uart_buffer, sizeof(uart_buffer), 10);
	#endif

	uint16_t last = 2702;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(50);
	}

	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, round(turn90_arc_en - turn90_arc_en *0.05));
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = turn_right_90;

	while(status != 0){
		#if debug == 1
		a = TIM2->CNT;
		b = TIM3->CNT;
		#endif
		running_left_motor(0, 400 + 200 * (float)TIM2->CNT/turn90_arc_en);
	}
	running_left_motor(1, 500);
	HAL_Delay(50);
	brake(0);
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void go_straight(float distance, bool brakee) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	#if debug == 1
		sprintf((char*)uart_buffer, "go_straight: %d | brake: %d\n", (int)distance, (int)brake);
		HAL_UART_Transmit(&huart3, uart_buffer, sizeof (uart_buffer), 10);
	#endif

	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) == htim1.Init.Period){
		uint16_t last = 2702;
		while(last != TIM3->CNT){
			last = TIM3->CNT;
			HAL_Delay(50);
		}
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		__HAL_TIM_SET_COUNTER(&htim3, 0);
	}
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en + 100);
	__HAL_TIM_SET_COUNTER(&htim2, TIM2->CNT + 100);
	__HAL_TIM_SET_COUNTER(&htim3, TIM3->CNT + 100);

	status = straight;
	int32_t Err, P, D, old_Error = 0;
	int32_t temp_1, temp_2;
	bool useIRSensor = true;
	uint16_t oe2 = WidthOESide + 100;
	uint16_t left_sensor45, right_sensor45, left_sensor90, right_sensor90, right_sensor0 = 8000;
	uint16_t speed = speed_levels[Rmode];
	int32_t ofs;
	while(status != 0 && right_sensor0 > oe2){
		vl53l0x_GetRanging_now(leftSensor90, &left_sensor90);
		vl53l0x_GetRanging_now(leftSensor45, &left_sensor45);
		vl53l0x_GetRanging_now(rightSensor45, &right_sensor45);
		vl53l0x_GetRanging_now(rightSensor90, &right_sensor90);
		vl53l0x_GetRanging_now(rightSensor0, &right_sensor0);
		if(left_sensor45 < HasleftWallValue_45 && right_sensor45 < HasrightWallValue_45
				&& left_sensor90 < HasleftWallValue_90 && right_sensor90 < HasrightWallValue_90){
			Err = right_sensor45 - left_sensor45 + 10;
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = true;
		} else if(left_sensor45 < HasleftWallValue_45 && left_sensor90 < HasleftWallValue_90){
			Err = (int32_t)leftWallValue - (int32_t)left_sensor45;
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = true;
		} else if(right_sensor45 < HasrightWallValue_45 && right_sensor90 < HasrightWallValue_90){
			Err = (int32_t)right_sensor45 - (int32_t)rightWallValue;
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = true;
		} else {
			if(useIRSensor){
				ofs = TIM3->CNT - TIM2->CNT;
			}
			temp_1 = TIM2->CNT;
			temp_2 = TIM3->CNT;
			Err = temp_2 - (temp_1 + ofs);
			useIRSensor = false;
			#if debug == 1
			a = TIM2->CNT;
			b = TIM3->CNT;
			#endif
		}
		if(useIRSensor){
			P = P_params[0] * Err + D * 0.5;
		} else {
			P = P_params[1] * Err;
		}
		P = max(-200, min(P, 200));
		running_left_motor(0, speed + P);
		running_right_motor(0, speed - P);
	}
	if(brakee){
		running_right_motor(1, 500);
		running_left_motor(1, 500);
		HAL_Delay(50);
		brake(2);
	}
}

void brake(uint8_t mode){
	if(mode == 0){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Init.Period);
	} else if(mode == 1){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Init.Period);
	} else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, htim1.Init.Period);
	}

}

