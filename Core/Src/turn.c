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

void u_turnf(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = east;   break;
		case east:  *direction = west;   break;
		case north: *direction = south;  break;
		case south: *direction = north;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(uturn_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = u_turn;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, 0);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, 0);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, speed_levels[current_speed][1]);
	while(status != 0);
	brake();
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	__HAL_TIM_SET_COUNTER(&htim3, 0);
}

void turn_left45() {
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_left_45;

	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim2.Init.Period - 400);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - (3400 - 2200 * round((TIM3->CNT)/turn45_arc_en)));
	}
}

void turn_right45() {
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn45_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_right_45;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - 3400);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim2.Init.Period);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - (3400 - 2200 * round((TIM1->CNT)/turn45_arc_en)));
	}
	brake();
}

void turn_left90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = south; break;
		case east:  *direction = north; break;
		case north: *direction = west;  break;
		case south: *direction = east;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_left_90;

	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - 4000);

	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - (4000 - 1800 * round((TIM3->CNT)/turn90_arc_en)));
	}
	brake();
}

void turn_right90(uint8_t *direction) {
	switch(*direction){
		case west:  *direction = north; break;
		case east:  *direction = south; break;
		case north: *direction = east;  break;
		case south: *direction = west;  break;
	}
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn90_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_right_90;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - 4000);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);

	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - (4000 - 1800 * round((TIM1->CNT)/turn90_arc_en)));
	}
	brake();
}

void go_straight(float distance, bool brakee) { //millimeter
	uint16_t en = round(distance * counts_per_1mm);
	__HAL_TIM_SET_AUTORELOAD(&htim1, en);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = straight;
	PID(0);
	TIM1 -> CNT = 0;
	TIM3 -> CNT = 0;
	if(brakee){
		brake();
	}
}

void backwards(float distance, bool brakee){
	uint16_t en = round(distance * counts_per_1mm);
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	__HAL_TIM_SET_AUTORELOAD(&htim1, TIM1 -> CNT + en);
	if(brakee){
		//__HAL_TIM_SET_AUTORELOAD(&htim3, en * 7 / 10);
	}
	status = straight;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, 0);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, speed_levels[current_speed][0]);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, 0);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, speed_levels[current_speed][1]);
	PID(1);
	if(brakee){
		brake();
	}
}

void brake(){
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);
	TIM1->CNT = 0;
	TIM3->CNT = 0;
}

void PID(uint8_t d){
	int32_t Err, pErr, P, D, It, total;
	int32_t temp_1, temp_3;
	bool useIRSensor = true;
	while(status != 0){
		if(left_sensor45 > leftWallValue && right_sensor45 > rightWallValue){
			Err = left_sensor45 - right_sensor45 - 10;
			if(!useIRSensor){
				pErr = 0;
				It = 0;
				useIRSensor = true;
			}
		} else if(left_sensor45 > leftWallValue){
			Err = left_sensor45 - leftWallValue;
			if(!useIRSensor){
				pErr = 0;
				It = 0;
				useIRSensor = true;
			}
		} else if(right_sensor45 > rightWallValue){
			Err = rightWallValue - right_sensor45;
			if(!useIRSensor){
				pErr = 0;
				It = 0;
				useIRSensor = true;
			}
		} else {
			temp_1 = TIM1->CNT;
			temp_3 = TIM3->CNT;
			Err = temp_3 - temp_1;
			if(useIRSensor){
				pErr = 0;
				It = 0;
				useIRSensor = false;
			}
			a = TIM1->CNT;
			b = TIM3->CNT;
			c = b - a;
		}
		if(useIRSensor){
			P = PID_params[2][0] * Err;
			It = It + (Err * PID_params[2][1]);
			if(It > 500){ It = 500; }
			if(It < -500){ It = -500; }
			D = PID_params[2][2] * (Err - pErr);
			pErr = Err;
			total = P + It + D;
		} else {
			P = PID_params[current_speed][0] * Err;
			It = It + (Err * PID_params[current_speed][1]);
			if(It > 500){ It = 500; }
			if(It < -500){ It = -500; }
			D = PID_params[current_speed][2] * (Err - pErr);
			pErr = Err;
			total = P + It + D;
		}
		if(d == 0){
			__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, speed_levels[current_speed][0] + total);
			__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, 0);
			__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, speed_levels[current_speed][1] - total);
			__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, 0);
		} else {
			__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, 0);
			__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, speed_levels[current_speed][0] + total);
			__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, 0);
			__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, speed_levels[current_speed][1] - total);
		}
	}
}

