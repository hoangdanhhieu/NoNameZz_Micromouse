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

void u_turnf() {
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
	HAL_Delay(500);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed_levels[current_speed][1]);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
	HAL_Delay(500);
	brake();
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
	HAL_Delay(1000);
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
}

void turn_left90() {
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
	HAL_Delay(1000);
}

void turn_right90() {
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
}

void go_straight(float distance, bool brake) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	int16_t offset = 0;
	__HAL_TIM_SET_AUTORELOAD(&htim1, en);
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	status = straight;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[0][0]);

	while(status != 0){
		if(TIM1 -> CNT < TIM3 -> CNT){
			offset--;
		} else if(TIM1 -> CNT > TIM3 -> CNT){
			offset++;
		}
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed_levels[0][1] + offset);
	}
}

void brake(){
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
}
/*
void balance() {
	uint32_t min = speed_levels[current_speed][0] - 200;
	uint32_t max = speed_levels[current_speed][0] + 200;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
	while (max - min > 1) {
		HAL_Delay(5000);
		uint32_t mid = min + (max - min) / 2;
		TIM1->CNT = 0;
		TIM3->CNT = 0;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed_levels[current_speed][0]);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, mid);
		HAL_Delay(1000);
		uint32_t a = TIM1->CNT;
		uint32_t b = TIM3->CNT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, htim2.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, htim2.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, htim2.Init.Period);
		if (a < b) {
			max = mid;
		} else {
			min = mid;
		}
	}
	speed_levels[current_speed][1] = min;
}
*/
