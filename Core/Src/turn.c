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

void average_adc(){
	uint16_t last_value = adc_value[0];
	adc_average_value[0] = adc_average_value[1] = adc_average_value[2] = adc_average_value[3] = 0;
	for(int i = 0; i < 20; i++){
		while(last_value == adc_value[0]);
		adc_average_value[0]+=adc_value[0];
		adc_average_value[1]+=adc_value[1];
		adc_average_value[2]+=adc_value[2];
		adc_average_value[3]+=adc_value[3];
		last_value = adc_value[0];
	}
	adc_average_value[0]/=20;
	adc_average_value[1]/=20;
	adc_average_value[2]/=20;
	adc_average_value[3]/=20;
}

void u_turnf(uint8_t *direction) {
	HAL_Delay(200);
	switch(*direction){
		case west:  *direction = east;   break;
		case east:  *direction = west;   break;
		case north: *direction = south;  break;
		case south: *direction = north;  break;
	}

	status = u_turn;
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn90_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - (2100 - 1000 * round((TIM3->CNT)/turn90_arc_en)));
	}
	brake();

	HAL_Delay(500);

	status = u_turn;
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn90_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period - (1000 + 200 * round((TIM1->CNT)/turn90_arc_en)));
	}
	brake();

}

void turn_left45() {
	 HAL_Delay(200);
	__HAL_TIM_SET_AUTORELOAD(&htim3, round(turn45_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_left_45;

	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - 3400);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - (2000 - 1000 * round((TIM3->CNT)/turn45_arc_en)));
	}
	brake();
}

void turn_right45() {
	HAL_Delay(200);
	__HAL_TIM_SET_AUTORELOAD(&htim1, round(turn45_arc_en));
	TIM1->CNT = 0;
	TIM3->CNT = 0;
	status = turn_right_45;
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);
	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - (2000 - 1000 * round((TIM1->CNT)/turn45_arc_en)));
	}
	brake();
}

void turn_left90(uint8_t *direction) {
	HAL_Delay(200);
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
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);

	while(status != 0){
		a  = TIM3 -> CNT;
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - (2100 - 1000 * round((TIM3->CNT)/turn90_arc_en)));
	}
	brake();
}

void turn_right90(uint8_t *direction) {
	HAL_Delay(200);
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
	__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
	__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period);

	while(status != 0){
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - (2100 - 1000 * round((TIM1->CNT)/turn90_arc_en)));
	}
	brake();
}

void go_straight(float distance, bool brakee) { //millimeter
	HAL_Delay(200);
	uint16_t en = round(distance * counts_per_1mm);
	__HAL_TIM_SET_AUTORELOAD(&htim1, en);
	__HAL_TIM_SET_AUTORELOAD(&htim3, UINT16_MAX);
	status = straight;
	TIM1 -> CNT = 0;
	TIM3 -> CNT = 0;
	int32_t Err, pErr, P, D, It, total;
	int32_t temp_1, temp_3;
	bool useIRSensor = true;
	while(status != 0){
		average_adc();
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
		}
		if(useIRSensor){
			P = PID_params[2][0] * Err;
			It = It + (Err * PID_params[2][1]);
			if(It > 200){ It = 200; }
			if(It < -200){ It = -200; }
			D = PID_params[2][2] * (Err - pErr);
			pErr = Err;
			total = P + It + D;
		} else {
			P = PID_params[current_speed][0] * Err;
			It = It + (Err * PID_params[current_speed][1]);
			if(It > 200){ It = 200; }
			if(It < -200){ It = -200; }
			D = PID_params[current_speed][2] * (Err - pErr);
			pErr = Err;
			total = P + It + D;
		}
		uint16_t speed = speed_levels[current_speed];
		if(current_speed == 1){
			if(TIM1->CNT < en/2){
				speed = speed_levels[1] - 500*(1 - (float)TIM1->CNT/(en/2));
			} else{
				speed = speed_levels[1] - 500*((float)TIM1->CNT/en * 2 - 1);
			}
		}
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor1, htim2.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim2, L_Motor2, htim2.Init.Period - speed - total);
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor1, htim4.Init.Period);
		__HAL_TIM_SET_COMPARE(&htim4, R_Motor2, htim4.Init.Period - speed + total);
	}
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

