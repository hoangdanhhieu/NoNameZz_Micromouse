/*
 * turn.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */
#include "turn.h"
#include <math.h>
#include <stdlib.h>

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

#define running_left_motor(mode, speed){\
	if(mode == 0){ \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0); \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, speed); \
	} else { \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed); \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0); \
	} \
}

#define running_right_motor(mode, speed) \
	if(mode == 0){ \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed); \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0); \
	} else { \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); \
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed); \
	\
}

#define set_counterTIM2_3(value2, value3) \
		__HAL_TIM_SET_COUNTER(&htim2, value2); \
		__HAL_TIM_SET_COUNTER(&htim3, value3);



const double wheels_arc = (double)wheels_radius * M_PI * 2;
const double counts_per_1mm = (double)encoder_resolution / wheels_arc;
const double counts_per_300mm = counts_per_1mm * 300;

const double uturn_arc_en = (double)turning_radius * M_PI * 2 * (180.0/360) * counts_per_1mm;
const double turn90_arc_en = (double)turning_radius * M_PI * 2 * (90.0/360) * counts_per_1mm;
const double turn45_arc_en = (double)turning_radius * M_PI * 2 * (45.0/360) * counts_per_1mm;

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

	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != htim1.Init.Period ||
			__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) != htim1.Init.Period){
		running_right_motor(1, 650);
		running_left_motor(1, 650);
		HAL_Delay(50);
		brake(2);
	}
	uint16_t last = 2702;
	while(last != TIM3->CNT){
		last = TIM3->CNT;
		HAL_Delay(50);
	}

	uint16_t en = round(uturn_arc_en);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim2, en);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = u_turn;

	uint16_t speed = 500;
	int32_t P;

	TIM1->CCR3 = 150;
	TIM1->CCR4 = 0;
	TIM1->CCR1 = 150;
	TIM1->CCR2 = 0;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)TIM3->CNT - ((int32_t)en - TIM2->CNT)) * 5;
		TIM1->CCR3 = speed - 100 + P;
		TIM1->CCR4 = 0;
		TIM1->CCR1 = speed - P;
		TIM1->CCR2 = 0;
		#if debug == 1
		a = (int32_t)en - TIM2->CNT;
		b = TIM3->CNT;
		#endif
	}
	running_left_motor(0, 600);
	running_right_motor(1, 600);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
}

void turn_left45(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = south_west; break;
		case east:  *direction = north_east; break;
		case north: *direction = north_west;  break;
		case south: *direction = south_east;  break;
		case north_west: *direction = west;  break;
		case north_east: *direction = north;  break;
		case south_west: *direction = south;  break;
		case south_east: *direction = east;  break;
	}
	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != htim1.Init.Period ||
			__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) != htim1.Init.Period){
		running_right_motor(1, 650);
		running_left_motor(1, 650);
		HAL_Delay(50);
		brake(2);
	}
	uint16_t last = 10;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(20);
	}
	uint16_t en = round(turn45_arc_en) + 500;
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim3, 500);
	__HAL_TIM_SET_COUNTER(&htim2, 10000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en);
	status = turn_left_45;
	int32_t P;
	uint16_t speed = 400;

	TIM1->CCR3 = 150;
	TIM1->CCR4 = 0;
	TIM1->CCR1 = 150;
	TIM1->CCR2 = 0;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)(TIM3->CNT - 500) - (10000 - TIM2->CNT)) * 5;
		TIM1->CCR3 = speed - 50 + P;
		TIM1->CCR4 = 0;
		TIM1->CCR1 = speed - P;
		TIM1->CCR2 = 0;
	}
	running_left_motor(0, 500);
	running_right_motor(1, 500);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
}

void turn_right45(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north_west; break;
		case east:  *direction = south_east; break;
		case north: *direction = north_east;  break;
		case south: *direction = south_west;  break;
		case north_west: *direction = north;  break;
		case north_east: *direction = east;  break;
		case south_west: *direction = west;  break;
		case south_east: *direction = south;  break;
	}

	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != htim1.Init.Period ||
		__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) != htim1.Init.Period){
		running_right_motor(1, 650);
		running_left_motor(1, 650);
		HAL_Delay(50);
		brake(2);
	}
	uint16_t last = 10;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(20);
	}
	uint16_t en = round(turn45_arc_en) + 450;
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim2, 500);
	__HAL_TIM_SET_COUNTER(&htim3, 10000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, en);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = turn_right_45;
	uint16_t speed = 400;
	int32_t P;

	TIM1->CCR3 = 0;
	TIM1->CCR4 = 150;
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 150;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)(10000 - TIM3->CNT) - ((int32_t)TIM2->CNT - 500)) * 5;
		TIM1->CCR3 = 0;
		TIM1->CCR4 = speed + P;
		TIM1->CCR1 = 0;
		TIM1->CCR2 = speed - P;
		#if debug == 1
		a = (TIM2->CNT - 500);
		b = (10000 - TIM3->CNT);
		#endif
	}
	running_left_motor(1, 500);
	running_right_motor(0, 500);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
}

void turn_left90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = south; break;
		case east:  *direction = north; break;
		case north: *direction = west;  break;
		case south: *direction = east;  break;
		case north_west: *direction = south_west;  break;
		case north_east: *direction = north_west;  break;
		case south_west: *direction = south_east;  break;
		case south_east: *direction = north_east;  break;
	}
	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != htim1.Init.Period ||
			__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) != htim1.Init.Period){
		running_right_motor(1, 650);
		running_left_motor(1, 650);
		HAL_Delay(50);
		brake(2);
	}
	uint16_t last = 10;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(20);
	}
	uint16_t en = round(turn90_arc_en) + 530;
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim3, 500);
	__HAL_TIM_SET_COUNTER(&htim2, 10000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en);
	status = turn_left_90;
	int32_t P;
	uint16_t speed = 400;

	TIM1->CCR3 = 150;
	TIM1->CCR4 = 0;
	TIM1->CCR1 = 150;
	TIM1->CCR2 = 0;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)(TIM3->CNT - 500) - (10000 - TIM2->CNT)) * 5;
		TIM1->CCR3 = speed - 50 + P;
		TIM1->CCR4 = 0;
		TIM1->CCR1 = speed - P;
		TIM1->CCR2 = 0;
	}
	running_left_motor(0, 500);
	running_right_motor(1, 500);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);

}

void turn_right90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north; break;
		case east:  *direction = south; break;
		case north: *direction = east;  break;
		case south: *direction = west;  break;
		case north_west: *direction = north_east;  break;
		case north_east: *direction = south_east;  break;
		case south_west: *direction = north_west;  break;
		case south_east: *direction = south_west;  break;
	}

	#if debug == 1
	sprintf((char*)uart_buffer, "turn_right90\n");
	HAL_UART_Transmit(&huart3, uart_buffer, sizeof(uart_buffer), 10);
	#endif

	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) != htim1.Init.Period ||
		__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) != htim1.Init.Period){
		running_right_motor(1, 650);
		running_left_motor(1, 650);
		HAL_Delay(50);
		brake(2);
	}
	uint16_t last = 10;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(20);
	}
	uint16_t en = round(turn90_arc_en) + 450;
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim2, 500);
	__HAL_TIM_SET_COUNTER(&htim3, 10000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, en);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = turn_right_90;
	uint16_t speed = 400;
	int32_t P;

	TIM1->CCR3 = 0;
	TIM1->CCR4 = 150;
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 150;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)(10000 - TIM3->CNT) - ((int32_t)TIM2->CNT - 500)) * 5;
		TIM1->CCR3 = 0;
		TIM1->CCR4 = speed + P;
		TIM1->CCR1 = 0;
		TIM1->CCR2 = speed - P;
		#if debug == 1
		a = (TIM2->CNT - 500);
		b = (10000 - TIM3->CNT);
		#endif
	}
	running_left_motor(1, 500);
	running_right_motor(0, 500);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);

}

void go_straight(double distance, bool brakee, int8_t next) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	bool isRunning = true;
	if(__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) == __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2)){
		uint16_t last = 10;
		while(last != TIM2->CNT){
			last = TIM2->CNT;
			HAL_Delay(20);
		}
		set_counterTIM2_3(0, 0);
		isRunning = false;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim2, en + 100);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en + 100);
	set_counterTIM2_3(TIM2->CNT + 100, TIM3->CNT + 100);

	status = straight;
	int32_t Err, P, D, old_Error = 0;
	int32_t temp_1, temp_2, temp_3;
	bool useIRSensor = true;
	uint16_t oe2 = 0;
	int32_t ofs;
	uint16_t temp;
	uint16_t left_sensor45, right_sensor45, left_sensor90,
		right_sensor90, left_sensor0 = 8000, right_sensor0 = 8000;
	vl53l0x_GetRanging_now(leftSensor90,  &left_sensor90);
	vl53l0x_GetRanging_now(rightSensor90, &right_sensor90);
	bool hasleftWalllast = left_sensor90 < HasleftWallValue_90;
	bool hasrightWalllast = right_sensor90 < HasrightWallValue_90;
	uint16_t speed0 = (next == -1) ? speed_levels[Rmode][0] : 350;
	uint16_t speed1 = (next == -1) ? speed_levels[Rmode][1] : 350;

	if(!isRunning){
		TIM1->CCR3 = 0;
		TIM1->CCR4 = 150;
		TIM1->CCR1 = 150;
		TIM1->CCR2 = 0;
		HAL_Delay(100);
	}

	while(status != 0 && (right_sensor0 > oe2 || left_sensor0 > oe2)){
		temp_3 = (int32_t)TIM3->CNT - TIM2->CNT;
		vl53l0x_GetRanging_now(leftSensor45,  &left_sensor45);
		vl53l0x_GetRanging_now(rightSensor45, &right_sensor45);
		vl53l0x_GetRanging_now(leftSensor0,   &left_sensor0);
		vl53l0x_GetRanging_now(rightSensor0,  &right_sensor0);
		vl53l0x_GetRanging_now(leftSensor90,  &left_sensor90);
		vl53l0x_GetRanging_now(rightSensor90,  &right_sensor90);
		if(next == -1 && ((left_sensor90 < HasleftWallValue_90 && !hasleftWalllast) ||
						(right_sensor90 < HasrightWallValue_90 && !hasrightWalllast) ||
						(left_sensor90 > HasleftWallValue_90 && hasleftWalllast) ||
						(right_sensor90 > HasrightWallValue_90 && hasrightWalllast))){
			temp = round((double)round((double)(TIM3->CNT)/counts_per_300mm) * counts_per_300mm);
			if((left_sensor90 < HasleftWallValue_90 && !hasleftWalllast) ||
					(right_sensor90 < HasrightWallValue_90 && !hasrightWalllast)){
				if(abs((int32_t)temp - en) < 50){
						__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
						__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
						set_counterTIM2_3(0, 0);
						break;
				}
				set_counterTIM2_3(temp + 100, temp + 100);
			} else if((left_sensor90 > HasleftWallValue_90 && hasleftWalllast) ||
					(right_sensor90 > HasrightWallValue_90 && hasrightWalllast)){
				if(abs((int32_t)temp - en) < 50){
					__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
					__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
					set_counterTIM2_3(round(40 * counts_per_1mm), round(40 * counts_per_1mm));
					break;
				}
				temp += round(40 * counts_per_1mm);
				set_counterTIM2_3(temp + 100, temp + 100);

			}
			hasleftWalllast = left_sensor90 < HasleftWallValue_90;
			hasrightWalllast = right_sensor90 < HasrightWallValue_90;
		}
		if(left_sensor0 > 150 && left_sensor45 < 350 && left_sensor90 < HasleftWallValue_90
				&& right_sensor45 < 350 && right_sensor90 < HasrightWallValue_90){
			Err = (int32_t)right_sensor45 - left_sensor45;
			D = Err - old_Error;
			old_Error = Err;
		} else if(left_sensor0 > 150 && left_sensor45 < 350 && (left_sensor90 < HasleftWallValue_90 ||
				left_sensor45 < 230)){
			Err = (int32_t)leftWallValue - left_sensor45;
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = true;
		} else if(left_sensor0 > 150 && right_sensor45 < 350 && (right_sensor90 < HasrightWallValue_90 ||
				right_sensor45 < 230)){
			Err = (int32_t)right_sensor45 - rightWallValue;
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = true;
		} else {
			if(useIRSensor){
				ofs = temp_3;
				old_Error = 0;
			}
			temp_1 = TIM2->CNT;
			temp_2 = TIM3->CNT;
			Err = temp_2 - (temp_1 + ofs);
			D = Err - old_Error;
			old_Error = Err;
			useIRSensor = false;
			#if debug == 1
			a = TIM2->CNT;
			b = TIM3->CNT;
			#endif
		}
		if(useIRSensor){
			P = round(P_params[0] * Err + D * 0);
		} else {
			P = round(P_params[1] * Err + D * 0);
		}
		P = max(-50, min(P, 50));
		TIM1->CCR3 = 0;
		TIM1->CCR4 = (uint16_t)speed0 + P;
		TIM1->CCR1 = (uint16_t)speed1 - P;
		TIM1->CCR2 = 0;
	}
	if(brakee){
		if(next == -1){
			running_right_motor(1, 800);
			running_left_motor(1, 880);
			HAL_Delay(70);
			brake(2);
		} else {
			running_right_motor(1, 600);
			running_left_motor(1, 660);
			HAL_Delay(50);
			brake(2);
		}
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

