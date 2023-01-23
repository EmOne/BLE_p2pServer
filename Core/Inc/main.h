/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
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
#include "stm32wbxx_hal.h"
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbxx_nucleo.h"
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
void   MX_LPUART1_UART_Init(void);
void   MX_USART1_UART_Init(void);

/* USER CODE BEGIN EFP */
/* Current IO function */
void CURENT_IO_Init(void);
void CURENT_IO_Read(uint16_t *pData);
void CURENT_IO_Write(uint16_t setting);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
#define CS_T_Port GPIOA
#define CS_T_Pin	GPIO_PIN_4
#define CS_T_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define CS_T_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOA_CLK_DISABLE()

#define CS_T_LOW()      HAL_GPIO_WritePin(CS_T_Port, CS_T_Pin, GPIO_PIN_RESET)
#define CS_T_HIGH()     HAL_GPIO_WritePin(CS_T_Port, CS_T_Pin, GPIO_PIN_SET)

#define CS_R_Port GPIOA
#define CS_R_Pin 	GPIO_PIN_9
#define CS_R_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define CS_R_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOA_CLK_DISABLE()

#define CS_R_LOW()      HAL_GPIO_WritePin(CS_R_Port, CS_R_Pin, GPIO_PIN_RESET)
#define CS_R_HIGH()     HAL_GPIO_WritePin(CS_R_Port, CS_R_Pin, GPIO_PIN_SET)

#define EN_R_Port GPIOA
#define EN_R_Pin	GPIO_PIN_10
#define EN_R_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define EN_R_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOA_CLK_DISABLE()

#define EN_R_LOW()      HAL_GPIO_WritePin(EN_R_Port, EN_R_Pin, GPIO_PIN_RESET)
#define EN_R_HIGH()     HAL_GPIO_WritePin(EN_R_Port, EN_R_Pin, GPIO_PIN_SET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
