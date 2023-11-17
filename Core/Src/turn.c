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


void running_left_motor(uint8_t mode){
	if(mode == 0){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, speed_levels[Rmode]);
	} else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_levels[Rmode]);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
	}
}

void running_right_motor(uint8_t mode){
	if(mode == 0){
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_levels[Rmode]);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	} else {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_levels[Rmode]);
	}
}

void u_turnf(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = east;   break;
		case east:  *direction = west;   break;
		case north: *direction = south;  break;
		case south: *direction = north;  break;
	}
	uint16_t en = round(uturn_arc_en + uturn_arc_en*0.03);
	__HAL_TIM_SET_COUNTER(&htim2, en);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en);
	status = u_turn;
	flag_uturn = 0;
	brake(2);
	uint16_t speed = 300;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	int32_t P;
	while(flag_uturn == 0){
		P = ((int32_t)TIM3->CNT - ((int32_t)en - TIM2->CNT)) * 5;
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed + P);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed - P);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
		if(en - TIM3->CNT < 200){
			speed = 200;
		}
		a = (int32_t)en - TIM2->CNT;
		b = TIM3->CNT;
	}
	while(flag_uturn < 2);
	status = 0;
	brake(2);
	uint16_t last = 65535;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(100);
	}
}

void turn_left45() {
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en));

	status = turn_left_45;

	brake(0);
	running_right_motor(0);
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

	status = turn_right_45;

	brake(1);
	running_left_motor(0);
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
	uint16_t last = 65535;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(100);
	}
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en));
	status = turn_left_90;

	brake(0);
	running_right_motor(0);
	while(status != 0);
	brake(1);
	last = 65535;
	while(last != TIM3->CNT){
		last = TIM3->CNT;
		HAL_Delay(100);
	}
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
	uint16_t last = 65535;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(100);
	}
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim2, round(turn90_arc_en - turn90_arc_en*0.1));
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = turn_right_90;

	brake(1);
	running_left_motor(0);
	while(status != 0);
	brake(0);
	last = 65535;
	while(last != TIM2->CNT){
		last = TIM2->CNT;
		HAL_Delay(100);
	}
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void go_straight(float distance, bool brakee) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	__HAL_TIM_SET_AUTORELOAD(&htim2, UINT16_MAX);
	__HAL_TIM_SET_AUTORELOAD(&htim3, en);
	status = straight;
	int32_t Err, P, D, old_Error = 0;
	int32_t temp_1, temp_2;
	bool useIRSensor = true;
	uint16_t left_sensor45, right_sensor45, left_sensor90, right_sensor90;
	uint16_t speed = speed_levels[Rmode];
	while(status != 0){
		vl53l0x_GetRanging_now(pMyDevice[5], &left_sensor90);
		vl53l0x_GetRanging_now(pMyDevice[2], &left_sensor45);
		vl53l0x_GetRanging_now(pMyDevice[3], &right_sensor45);
		vl53l0x_GetRanging_now(pMyDevice[0], &right_sensor90);
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
			temp_1 = TIM2->CNT;
			temp_2 = TIM3->CNT;
			Err = temp_2 - temp_1;
			useIRSensor = false;
			a = TIM2->CNT;
			b = TIM3->CNT;
		}
		if(useIRSensor){
			P = P_params[0] * Err + D * 0.5;
		} else {
			P = P_params[1] * Err;
		}
		P = max(-200, min(P, 200));
		if(brakee && en > 600 && en - TIM3->CNT < 600){
			speed = 200;
		}
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, speed + P);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed - P);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	}
	if(brakee){
		brake(2);
		uint16_t last = 65535;
		while(last != TIM2->CNT){
			last = TIM2->CNT;
			HAL_Delay(100);
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

