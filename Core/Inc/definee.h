/*
 * size.h
 *
 *  Created on: Aug 12, 2023
 *      Author: Admin
 */

#ifndef INC_DEFINEE_H_
#define INC_DEFINEE_H_

#define west       0
#define east       1
#define north      2
#define south      3
#define north_west 4
#define north_east 5
#define south_west 6
#define south_east 7

#define straight      -6
#define u_turn        -5
#define turn_left_90  -4
#define turn_left_45  -3
#define turn_right_45 -2
#define turn_right_90 -1

#define grid_size           10
#define square_size         300
#define halfSize_MicroMouse 34
#define wheels_radius       16
#define encoder_resolution  40
#define length_MicroMouse   97
#define d1                  28      //length between wheel and rear

#define leftWallValue 100
#define rightWallValue 100
#define frontWallValue 100

#define left_sensor45 adc_value[2]
#define right_sensor45 adc_value[1]
#define left_sensor_front adc_value[3]
#define right_sensor_front adc_value[0]

#define R_Motor1 TIM_CHANNEL_1
#define R_Motor2 TIM_CHANNEL_2
#define L_Motor1 TIM_CHANNEL_1
#define L_Motor2 TIM_CHANNEL_2


#endif /* INC_DEFINEE_H_ */
