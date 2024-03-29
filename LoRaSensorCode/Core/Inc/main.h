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
#ifndef STM32L071xx
#define STM32L071xx
#endif
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "sx126x.h"
#include <stdbool.h>
#include <stdint.h>
#include "sx126x.h"
#include "stm32l0xx_hal.h"
#include "sx126x_hal.h"
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
void ProcessCommands(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NRESET_Pin GPIO_PIN_0
#define NRESET_GPIO_Port GPIOA
#define NS_Pin GPIO_PIN_4
#define NS_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_0
#define BUSY_GPIO_Port GPIOB
#define DIO1_Pin GPIO_PIN_1
#define DIO1_GPIO_Port GPIOB
#define ANT_SW_Pin GPIO_PIN_11
#define ANT_SW_GPIO_Port GPIOA
#define DIO2_Pin GPIO_PIN_5
#define DIO2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define STDBY_RC    0
#define STDBY_XOSC  1

#define MAX_PACKET_LENGTH  16
#define TCXO_BOOT_TIME_MS 4 //from Table 5 of the TG-5006CJ-47L 32.000000 MHz_X1G0041310042 SEIKO EPSON  datasheet
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
