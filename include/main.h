/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define BOARD_LED_Pin GPIO_PIN_13
#define BOARD_LED_GPIO_Port GPIOC
#define BUTTON_KEY_Pin GPIO_PIN_0
#define BUTTON_KEY_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define BUTTON_UP_Pin GPIO_PIN_12
#define BUTTON_UP_GPIO_Port GPIOB
#define BUTTON_RIGHT_Pin GPIO_PIN_13
#define BUTTON_RIGHT_GPIO_Port GPIOB
#define BUTTON_DOWN_Pin GPIO_PIN_14
#define BUTTON_DOWN_GPIO_Port GPIOB
#define BUTTON_LEFT_Pin GPIO_PIN_8
#define BUTTON_LEFT_GPIO_Port GPIOA
#define BUTTON_CANCEL_Pin GPIO_PIN_9
#define BUTTON_CANCEL_GPIO_Port GPIOA
#define BUTTON_CONFIRM_Pin GPIO_PIN_10
#define BUTTON_CONFIRM_GPIO_Port GPIOA
#define SDIO_DETECT_Pin GPIO_PIN_8
#define SDIO_DETECT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
