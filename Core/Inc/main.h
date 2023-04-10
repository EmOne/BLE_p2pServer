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
extern ADC_HandleTypeDef hadc1;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void   MX_LPUART1_UART_Init(void);
void   MX_USART1_UART_Init(void);

/* USER CODE BEGIN EFP */
/* Current IO function */
void COMMON_IO_Init(void);
void COMMON_IO_DeInit(void);
void COMMON_IO_Init_RxOnly(void);
void COMMON_IO_Read(uint8_t *pData, uint16_t len);
void COMMON_IO_Write(uint8_t *pData, uint16_t len);
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

#define EN_R_Port	GPIOA
#define EN_R_Pin	GPIO_PIN_10
#define EN_R_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()
#define EN_R_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOA_CLK_DISABLE()
#define EN_R_IRQn	EXTI15_10_IRQn
#define EN_R_LOW()      HAL_GPIO_WritePin(EN_R_Port, EN_R_Pin, GPIO_PIN_RESET)
#define EN_R_HIGH()     HAL_GPIO_WritePin(EN_R_Port, EN_R_Pin, GPIO_PIN_SET)

/* User can use this section to tailor ADCx instance under use and associated
 resources */

/* ## Definition of ADC related resources ################################### */
/* Definition of ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition of ADCx channels */
#define ADCx_CHANNELa                   ADC_CHANNEL_1

/* Definition of ADCx NVIC resources */
#define ADCx_IRQn                       ADC1_IRQn
#define ADCx_IRQHandler                 ADC1_IRQHandler

/* Definition of ADCx channels pins */
#define ADCx_CHANNELa_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADCx_CHANNELa_GPIO_PORT         GPIOC
#define ADCx_CHANNELa_PIN               GPIO_PIN_0

/* Definitions of environment analog values */
/* Value of analog reference voltage (Vref+), connected to analog voltage   */
/* supply Vdda (unit: mV).                                                  */
#define VDDA_APPLI                       ((uint32_t)3300)

/* Definitions of data related to this example */
/* Full-scale digital value with a resolution of 12 bits (voltage range     */
/* determined by analog voltage references Vref+ and Vref-,                 */
/* refer to reference manual).                                              */
#define DIGITAL_SCALE_12BITS             ((uint32_t) 0xFFF)

/* Init variable out of ADC expected conversion data range */
#define VAR_CONVERTED_DATA_INIT_VALUE    (DIGITAL_SCALE_12BITS + 1)

/* Private macro -------------------------------------------------------------*/

/**
 * @brief  Macro to calculate the voltage (unit: mVolt)
 *         corresponding to a ADC conversion data (unit: digital value).
 * @note   ADC measurement data must correspond to a resolution of 12bits
 *         (full scale digital value 4095). If not the case, the data must be
 *         preliminarily rescaled to an equivalent resolution of 12 bits.
 * @note   Analog reference voltage (Vref+) must be known from
 *         user board environment.
 * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
 * @param  __ADC_DATA__ ADC conversion data (resolution 12 bits)
 *                       (unit: digital value).
 * @retval ADC conversion data equivalent voltage value (unit: mVolt)
 */
#define __ADC_CALC_DATA_VOLTAGE(__VREFANALOG_VOLTAGE__, __ADC_DATA__)       \
  ((__ADC_DATA__) * (__VREFANALOG_VOLTAGE__) / DIGITAL_SCALE_12BITS)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
