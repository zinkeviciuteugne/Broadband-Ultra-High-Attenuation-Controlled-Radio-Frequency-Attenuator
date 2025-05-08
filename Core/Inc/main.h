/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern SPI_HandleTypeDef hspi2;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CS_SC_Pin GPIO_PIN_0
#define CS_SC_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_1
#define RST_GPIO_Port GPIOA
#define SIGNAL_Pin GPIO_PIN_2
#define SIGNAL_GPIO_Port GPIOA
#define PHASE_Pin GPIO_PIN_3
#define PHASE_GPIO_Port GPIOA
#define LE_3_Pin GPIO_PIN_4
#define LE_3_GPIO_Port GPIOA
#define A3_B1_Pin GPIO_PIN_5
#define A3_B1_GPIO_Port GPIOA
#define A3_B2_Pin GPIO_PIN_6
#define A3_B2_GPIO_Port GPIOA
#define A3_B3_Pin GPIO_PIN_7
#define A3_B3_GPIO_Port GPIOA
#define A3_B4_Pin GPIO_PIN_0
#define A3_B4_GPIO_Port GPIOB
#define A3_B5_Pin GPIO_PIN_1
#define A3_B5_GPIO_Port GPIOB
#define DC_Pin GPIO_PIN_2
#define DC_GPIO_Port GPIOB
#define A3_B0_Pin GPIO_PIN_10
#define A3_B0_GPIO_Port GPIOB
#define LE_2_Pin GPIO_PIN_12
#define LE_2_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_13
#define CLK_GPIO_Port GPIOB
#define MISO_Pin GPIO_PIN_14
#define MISO_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_15
#define MOSI_GPIO_Port GPIOB
#define A2_B1_Pin GPIO_PIN_8
#define A2_B1_GPIO_Port GPIOA
#define A2_B2_Pin GPIO_PIN_9
#define A2_B2_GPIO_Port GPIOA
#define A2_B3_Pin GPIO_PIN_10
#define A2_B3_GPIO_Port GPIOA
#define A2_B4_Pin GPIO_PIN_11
#define A2_B4_GPIO_Port GPIOA
#define A2_B5_Pin GPIO_PIN_12
#define A2_B5_GPIO_Port GPIOA
#define A2_B0_Pin GPIO_PIN_15
#define A2_B0_GPIO_Port GPIOA
#define A1_B1_Pin GPIO_PIN_3
#define A1_B1_GPIO_Port GPIOB
#define A1_B2_Pin GPIO_PIN_4
#define A1_B2_GPIO_Port GPIOB
#define A1_B3_Pin GPIO_PIN_5
#define A1_B3_GPIO_Port GPIOB
#define A1_B4_Pin GPIO_PIN_6
#define A1_B4_GPIO_Port GPIOB
#define A1_B5_Pin GPIO_PIN_7
#define A1_B5_GPIO_Port GPIOB
#define A1_B0_Pin GPIO_PIN_8
#define A1_B0_GPIO_Port GPIOB
#define LE_1_Pin GPIO_PIN_9
#define LE_1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
