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


const double uturn_arc_en = (double)turning_radius * M_PI * 2 * (180.0/360) * counts_per_1mm;
const double turn90_arc_en = (double)turning_radius * M_PI * 2 * (90.0/360) * counts_per_1mm;
const double turn45_arc_en = (double)turning_radius * M_PI * 2 * (45.0/360) * counts_per_1mm;


int32_t Err, P, D, old_Error;
int32_t temp_1, temp_2, temp_3;
bool useIRSensor;
uint16_t oe2;
int32_t ofs;
uint16_t left_sensor45, right_sensor45, left_sensor90,
	right_sensor90, left_sensor0, right_sensor0;
bool hasleftWalllast, hasrightWalllast;
uint16_t speed0, speed1;

void pid_normal();
void pid_diagonal();

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
		HAL_Delay(20);
	}

	uint16_t en = round(uturn_arc_en);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim2, en);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = u_turn;

	uint16_t speed = 400;
	int32_t P;

	TIM1->CCR3 = 250;
	TIM1->CCR4 = 0;
	TIM1->CCR1 = 250;
	TIM1->CCR2 = 0;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)TIM3->CNT - ((int32_t)en - TIM2->CNT)) * 5;
		TIM1->CCR3 = speed + P;
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
		case west:  *direction = south_west;  break;
		case east:  *direction = north_east;  break;
		case north: *direction = north_west;  break;
		case south: *direction = south_east;  break;
		case north_west: *direction = west;   break;
		case north_east: *direction = north;  break;
		case south_west: *direction = south;  break;
		case south_east: *direction = east;   break;
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
	running_left_motor(0, 600);
	running_right_motor(1, 600);
	HAL_Delay(50);
	brake(2);
	__HAL_TIM_SET_COUNTER(&htim2, 100);
	__HAL_TIM_SET_COUNTER(&htim3, 100);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
}

void turn_right45(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north_west;  break;
		case east:  *direction = south_east;  break;
		case north: *direction = north_east;  break;
		case south: *direction = south_west;  break;
		case north_west: *direction = north;  break;
		case north_east: *direction = east;   break;
		case south_west: *direction = west;   break;
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
	running_left_motor(1, 700);
	running_right_motor(0, 7004);
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
	uint16_t en = round(turn90_arc_en) + 500;
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	__HAL_TIM_SET_COUNTER(&htim3, 500);
	__HAL_TIM_SET_COUNTER(&htim2, 10000);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en);
	status = turn_left_90;
	int32_t P;
	uint16_t speed = 400;

	TIM1->CCR3 = 250;
	TIM1->CCR4 = 0;
	TIM1->CCR1 = 250;
	TIM1->CCR2 = 0;
	HAL_Delay(100);

	while(status != 0){
		P = ((int32_t)(TIM3->CNT - 500) - (10000 - TIM2->CNT)) * 5;
		TIM1->CCR3 = speed + P;
		TIM1->CCR4 = 0;
		TIM1->CCR1 = speed - P;
		TIM1->CCR2 = 0;
	}
	TIM1->CCR3 = 0;
	TIM1->CCR4 = 700;
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 700;
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
	TIM1->CCR4 = 250;
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 250;
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
	running_left_motor(1, 700);
	running_right_motor(0, 700);
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
	old_Error = 0;
	useIRSensor = true;
	oe2 = 90;
	left_sensor45
		= right_sensor45
		= left_sensor90
		= right_sensor90
		= left_sensor0
		= right_sensor0
		= 8000;
	uint16_t temp;
	vl53l0x_GetRanging_now(leftSensor90,  &left_sensor90);
	vl53l0x_GetRanging_now(rightSensor90, &right_sensor90);
	hasleftWalllast = left_sensor90 < HasleftWallValue_90;
	hasrightWalllast = right_sensor90 < HasrightWallValue_90;
	speed0 = (next == -1 || next == -2) ? speed_levels[Rmode][0] : 400;
	speed1 = (next == -1 || next == -2) ? speed_levels[Rmode][1] : 400;
	if(!isRunning){
		TIM1->CCR3 = 0;
		TIM1->CCR4 = 250;
		TIM1->CCR1 = 250;
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
		if(next == -1 || next == 0 || next == -3){ // normal
			if((next == -1 || next == -3) && ((left_sensor90 < HasleftWallValue_90 && !hasleftWalllast) ||
							(right_sensor90 < HasrightWallValue_90 && !hasrightWalllast) ||
							(left_sensor90 > HasleftWallValue_90 && hasleftWalllast) ||
							(right_sensor90 > HasrightWallValue_90 && hasrightWalllast))){
				temp = round((double)round((double)(TIM3->CNT)/counts_per_300mm) * counts_per_300mm);
				if((left_sensor90 < HasleftWallValue_90 && !hasleftWalllast) ||
						(right_sensor90 < HasrightWallValue_90 && !hasrightWalllast)){
					if(abs((int32_t)temp - en) < 100){
							__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
							__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
							set_counterTIM2_3(0, 0);
							break;
					}
					set_counterTIM2_3(temp + 100, temp + 100);
				} else if((left_sensor90 > HasleftWallValue_90 && hasleftWalllast) ||
						(right_sensor90 > HasrightWallValue_90 && hasrightWalllast)){
					if(abs((int32_t)temp - en) < 100){
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
			pid_normal();
		} else { //diagonal
			pid_diagonal();
		}
	}
	if(brakee){
		if(next == -1 || next == -2){
			running_right_motor(1, 800);
			running_left_motor(1, 800);
			HAL_Delay(70);
			brake(2);
		} else {
			running_right_motor(1, 700);
			running_left_motor(1, 700);
			HAL_Delay(60);
			brake(2);
		}
	}
	if(next == 0 || status == 0){
		__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
		__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
		set_counterTIM2_3(0, 0);
	}

}



void pid_normal(){
	if((left_sensor0 > 150 || right_sensor0 > 150) && left_sensor45 < 310 && left_sensor90 < HasleftWallValue_90
			&& right_sensor45 < 310 && right_sensor90 < HasrightWallValue_90){
		if(left_sensor45 + right_sensor45 > 500){
			goto useEncoder;
		}

		Err = (int32_t)right_sensor45 - left_sensor45;
		D = Err - old_Error;
		old_Error = Err;
		useIRSensor = true;
	} else if((left_sensor0 > 150 || right_sensor0 > 150) && left_sensor45 < 310
			&& left_sensor90 < HasleftWallValue_90){
		Err = (int32_t)leftWallValue - left_sensor45;
		D = Err - old_Error;
		old_Error = Err;
		useIRSensor = true;
	} else if((left_sensor0 > 150 || right_sensor0 > 150) && right_sensor45 < 310
			&& right_sensor90 < HasrightWallValue_90){
		Err = (int32_t)right_sensor45 - rightWallValue;
		D = Err - old_Error;
		old_Error = Err;
		useIRSensor = true;
	} else {
		useEncoder:
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
		P = round(P_params[Rmode][0] * Err + D * 0);
	} else {
		P = round(P_params[Rmode][1] * Err + D * 0);
	}
	if(Rmode == 1){
		P = max(-80, min(P, 80));
	} else {
		P = max(-100, min(P, 100));
	}
	TIM1->CCR3 = 0;
	TIM1->CCR4 = (uint16_t)speed0 + P;
	TIM1->CCR1 = (uint16_t)speed1 - P;
	TIM1->CCR2 = 0;
}


void pid_diagonal(){
	if(left_sensor45 < 200){
		Err = (int32_t)200 - left_sensor45;
		useIRSensor = true;
	} else if(right_sensor45 < 200){
		Err = (int32_t)right_sensor45 - 200;
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
		P = round(1.5 * Err + D * 0);
	} else {
		P = round(3 * Err + D * 0);
	}
	TIM1->CCR3 = 0;
	TIM1->CCR4 = (uint16_t)speed0 + P;
	TIM1->CCR1 = (uint16_t)speed1 - P;
	TIM1->CCR2 = 0;
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

