/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "path_optimization.h"
#include <vl53l0x_init.h>
#include "fill_maze.h"
#include "turn.h"
#include "definee.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern uint8_t maze[grid_size][grid_size];
extern const uint8_t starting_coordinates[2];
extern const uint8_t ending_coordinates[2];
extern const int32_t speed_levels[3][2];
extern double P_params[3][2];
extern bool visited[grid_size][grid_size];
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern volatile uint8_t Rmode;
extern VL53L0X_Dev_t *pMyDevice[n_vl53l0x];
extern uint16_t ts1, ts2, ts3, ts4, ts5, ts6;
extern int a, b, c, d, e;
extern UART_HandleTypeDef huart3;
extern uint8_t found_path;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define xSHUT_0_Pin GPIO_PIN_1
#define xSHUT_0_GPIO_Port GPIOA
#define xSHUT_1_Pin GPIO_PIN_2
#define xSHUT_1_GPIO_Port GPIOA
#define xSHUT_2_Pin GPIO_PIN_3
#define xSHUT_2_GPIO_Port GPIOA
#define xSHUT_3_Pin GPIO_PIN_4
#define xSHUT_3_GPIO_Port GPIOA
#define xSHUT_4_Pin GPIO_PIN_5
#define xSHUT_4_GPIO_Port GPIOA
#define xSHUT_5_Pin GPIO_PIN_6
#define xSHUT_5_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
