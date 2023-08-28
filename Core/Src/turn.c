/*
 * turn.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Admin
 */
#include "turn.h"

const float wheels_arc = (float)wheels_radius * M_PI * 2;
const float counts_per_1mm = (float)encoder_resolution / wheels_arc;

const float uturn_arc_en = (float)halfSize_MicroMouse * M_PI * 2 * (180.0/360) * counts_per_1mm;
const float turn90_arc_en = (float)halfSize_MicroMouse * M_PI * 4 * (90.0/360) * counts_per_1mm;
const float turn45_arc_en = (float)halfSize_MicroMouse * M_PI * 4 * (45.0/360) * counts_per_1mm;
const uint16_t kp = 3;
const uint16_t kd = 3;

void u_turnf(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = east;   break;
		case east:  *direction = west;   break;
		case north: *direction = south;  break;
		case south: *direction = north;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(uturn_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(uturn_arc_en) * 5 / 10);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = u_turn;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed_levels[current_speed][1]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_left45() {
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn45_arc_en) * 5 / 10);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = turn_left_45;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed_levels[current_speed][1]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_right45() {
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn45_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en) * 5 / 10);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = turn_right_45;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_left90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = south; break;
		case east:  *direction = north; break;
		case north: *direction = west;  break;
		case south: *direction = east;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn90_arc_en) * 5 / 10);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = turn_left_90;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed_levels[current_speed][1]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_right90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north; break;
		case east:  *direction = south; break;
		case north: *direction = east;  break;
		case south: *direction = west;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn90_arc_en));
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en) * 5 / 10);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = turn_right_90;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void go_straight(float distance, bool brakee) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	__HAL_TIM_SET_AUTORELOAD(&htim1, en - TIM1 -> CNT);
	if(brakee){
		__HAL_TIM_SET_AUTORELOAD(&htim3, en * 7 / 10);
	}
	status = straight;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[0][0]);
	PID();
	if(brakee){
		brake();
	}
}

void backwards(){
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed_levels[0][1]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed_levels[0][0]);
	HAL_Delay(500);
	brake();
}

void brake(){
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
}

void PID(){
	int16_t Err, pErr = 0, P, D, total;
	while(status != 0){
		if(left_sensor45 > leftWallValue && right_sensor45 > rightWallValue){
			Err = left_sensor45 - right_sensor45 - 10;
		} else if(left_sensor45 > leftWallValue){
			Err = left_sensor45 - leftWallValue;
		} else if(right_sensor45 > rightWallValue){
			Err = rightWallValue - right_sensor45;
		} else {
			Err = TIM3->CNT - TIM1->CNT;
			pErr = 0;
		}
		P = kp * Err;
		D = kd * (Err - pErr);
		pErr = Err;
		total = P + D;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[current_speed][0] + total);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed_levels[current_speed][1] - total);
	}
}

