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
#include "stm32h7xx_hal.h"

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
#define MCO1_Pin GPIO_PIN_8
#define MCO1_GPIO_Port GPIOA
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH

/* USER CODE BEGIN Private defines */
/* I2C1 / CTP Touchscreen GPIO pins */
#define I2C1_SCL_CTP_HAPTIC_Pin       GPIO_PIN_6
#define I2C1_SCL_CTP_HAPTIC_GPIO_Port GPIOB
#define I2C1_SDA_CTP_HAPTIC_Pin       GPIO_PIN_7
#define I2C1_SDA_CTP_HAPTIC_GPIO_Port GPIOB
#define CTP_RST_Pin                   GPIO_PIN_14
#define CTP_RST_GPIO_Port             GPIOC
#define CTP_INT_Pin                   GPIO_PIN_3
#define CTP_INT_GPIO_Port             GPIOH
#define CTP_INT_EXTI_IRQn             EXTI3_IRQn
/* USER CODE END Private defines */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_I2C1_Init(void);
void MX_DMA2D_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
