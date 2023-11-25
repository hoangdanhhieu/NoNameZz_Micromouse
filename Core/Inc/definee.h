/*
 * size.h
 *
 *  Created on: Aug 12, 2023
 *      Author: Admin
 */

#ifndef INC_DEFINEE_H_
#define INC_DEFINEE_H_

#define debug 0

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

#define grid_size            10
#define square_size         300
#define turning_radius      58
#define dbtWheels_r 		  96
#define dbtWheels_c		  35
#define wheels_radius       29
#define encoder_resolution 1232

#define HasleftWallValue_45  320
#define HasrightWallValue_45  320
#define HasfrontWallValue  400

#define HasleftWallValue_90  200
#define HasrightWallValue_90  200

#define leftWallValue 255
#define rightWallValue 255

#define n_vl53l0x 6

#define rightSensor90 pMyDevice[0]
#define rightSensor0 pMyDevice[1]
#define leftSensor45 pMyDevice[2]
#define rightSensor45 pMyDevice[3]
#define leftSensor0 pMyDevice[4]
#define leftSensor90 pMyDevice[5]


const static float WidthOESide = (float)(square_size - dbtWheels_r)/2;


#endif /* INC_DEFINEE_H_ */
