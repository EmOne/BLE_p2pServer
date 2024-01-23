/*
 * voltage_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "multiplex_common.h"

//Voltage_t hVolt;
//bool bVoltageSinkInit = false;
//
//#if 1
//#define FILT_PORT 	GPIOA
//#define FILT_PIN	GPIO_PIN_4
//#define FILT_LOW()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_RESET)
//#define FILT_HIGH()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_SET)
//#define REST_PORT	GPIOA
//#define REST_PIN	GPIO_PIN_0
//#define REST_LOW()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_RESET)
//#define REST_HIGH()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_SET)
//#define GAIN_PORT	GPIOA
//#define GAIN_PIN	GPIO_PIN_8
//#define GAIN_LOW()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_RESET)
//#define GAIN_HIGH()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_SET)
//#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
//
//#define INT_PIN GPIO_PIN_6 //EN_R_Pin
//#define INT_PORT GPIOC //EN_R_Port
//#define INT_IRQn EXTI9_5_IRQn //EN_R_IRQn
//#define INT_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE() //EN_R_GPIO_CLK_ENABLE()
//
//#define AN_1_PORT	ADCx_CHANNELa_GPIO_PORT
//#define AN_1_PIN	ADCx_CHANNELa_PIN
//#define AN_1_CLK_ENABLE ADCx_CHANNELa_GPIO_CLK_ENABLE
//
//#define AN_CS_PORT			CS_R_Port
//#define AN_CS_PIN			CS_R_Pin
//#define AN_CS_CLK_ENABLE	CS_R_GPIO_CLK_ENABLE
//#define AN_CS_LOW()	HAL_GPIO_WritePin(AN_CS_PORT, AN_CS_PIN, GPIO_PIN_RESET)
//#define AN_CS_HIGH()	HAL_GPIO_WritePin(AN_CS_PORT, AN_CS_PIN, GPIO_PIN_SET)

///* Variables for ADC conversion data */
//__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE; /* ADC group regular conversion data */

///* Variables for ADC conversion data computation to physical values */
//uint16_t uhADCxConvertedData_Voltage_mVolt = 0; /* Value of voltage calculated from ADC conversion data (unit: mV) */

//#else
//#define FILT_PORT 	GPIOA
//#define FILT_PIN	GPIO_PIN_9
//#define REST_PORT	GPIOA
//#define REST_PIN	GPIO_PIN_1
//#define GAIN_PORT	GPIOA
//#define GAIN_PIN	GPIO_PIN_15
//#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
//#endif

#define MP_LATCHING_PORT 	GPIOB
#define MP_LATCHING_PIN		GPIO_PIN_14
#define MP_LATCHING_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()
#define MP_LATCHING_CLK_DISABLE()               __HAL_RCC_GPIOB_CLK_DISABLE()
#define MP_LATCHING_ON()	HAL_GPIO_WritePin(MP_LATCHING_PORT, MP_LATCHING_PIN, GPIO_PIN_RESET)
#define MP_LATCHING_OFF()	HAL_GPIO_WritePin(MP_LATCHING_PORT, MP_LATCHING_PIN, GPIO_PIN_SET)

#define MP_RESET_PORT		GPIOB
#define MP_RESET_PIN		GPIO_PIN_12
#define MP_RESET_CLK_ENABLE()                   __HAL_RCC_GPIOB_CLK_ENABLE()
#define MP_RESET_ENABLE()	HAL_GPIO_WritePin(MP_RESET_PORT, MP_RESET_PIN, GPIO_PIN_RESET)
#define MP_RESET_DISABLE()	HAL_GPIO_WritePin(MP_RESET_PORT, MP_RESET_PIN, GPIO_PIN_SET)

#define MP_PSAVE_PORT		GPIOB
#define MP_PSAVE_PIN		GPIO_PIN_4
#define MP_PSAVE_CLK_ENABLE()                   __HAL_RCC_GPIOB_CLK_ENABLE()
#define MP_PSAVE_ENABLE()	HAL_GPIO_WritePin(MP_PSAVE_PORT, MP_PSAVE_PIN, GPIO_PIN_SET)
#define MP_PSAVE_DISABLE()	HAL_GPIO_WritePin(MP_PSAVE_PORT, MP_PSAVE_PIN, GPIO_PIN_RESET)

#define MP_SPI_CS_PORT		GPIOB
#if 0
#define MP_SPI_CS_PIN		GPIO_PIN_3 //GPIO_PIN_3
#else
#define MP_SPI_CS_PIN		GPIO_PIN_13 //GPIO_PIN_3
#endif
#define MP_SPI_CS_CLK_ENABLE()                   __HAL_RCC_GPIOB_CLK_ENABLE()
#define MP_SPI_CS_ENABLE()	HAL_GPIO_WritePin(MP_SPI_CS_PORT, MP_SPI_CS_PIN, GPIO_PIN_RESET)
#define MP_SPI_CS_DISABLE()	HAL_GPIO_WritePin(MP_SPI_CS_PORT, MP_SPI_CS_PIN, GPIO_PIN_SET)

#define MP_SPI_SCK_PORT		GPIOA
#define MP_SPI_SCK_PIN		GPIO_PIN_9
#define MP_SPI_SCK_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define MP_SPI_DO_PORT		GPIOC
#define MP_SPI_DO_PIN		GPIO_PIN_3
#define MP_SPI_DO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

#define MP_SPI				SPI2
#define MP_SPI_CLK_ENABLE	__HAL_RCC_SPI2_CLK_ENABLE

SPI_HandleTypeDef *_mpHandler;

void multiplexer_init(void *_handler)
{
	GPIO_InitTypeDef gpioinitstruct =
	{ 0 };
	
	if (_handler != NULL)
	{
		MP_LATCHING_CLK_ENABLE();
		gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
		gpioinitstruct.Pull = GPIO_PULLUP;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		gpioinitstruct.Pin = MP_LATCHING_PIN;
		HAL_GPIO_Init(MP_LATCHING_PORT, &gpioinitstruct);
		MP_LATCHING_OFF();

		MP_RESET_CLK_ENABLE();
		gpioinitstruct.Pin = MP_RESET_PIN;
		HAL_GPIO_Init(MP_RESET_PORT, &gpioinitstruct);
		MP_RESET_DISABLE();

		MP_PSAVE_CLK_ENABLE();
		gpioinitstruct.Pin = MP_PSAVE_PIN;
		gpioinitstruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(MP_PSAVE_PORT, &gpioinitstruct);
		MP_PSAVE_DISABLE();

		_mpHandler = (SPI_HandleTypeDef*) _handler;
		_mpHandler->Instance = MP_SPI;
		/* SPI baudrate is set to 8 MHz maximum (PCLK2/SPI_BaudRatePrescaler = 32/4 = 8 MHz)
		 to verify these constraints:
		 - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
		 Since the provided driver doesn't use read capability from LCD, only constraint
		 on write baudrate is considered.
		 - SD card SPI interface max baudrate is 25MHz for write/read
		 - PCLK2 max frequency is 32 MHz
		 */
		_mpHandler->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		_mpHandler->Init.Direction = SPI_DIRECTION_2LINES;
		_mpHandler->Init.CLKPhase = SPI_PHASE_1EDGE;
		_mpHandler->Init.CLKPolarity = SPI_POLARITY_HIGH;
		_mpHandler->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		_mpHandler->Init.CRCPolynomial = 7;
		_mpHandler->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
		_mpHandler->Init.DataSize = SPI_DATASIZE_8BIT;
		_mpHandler->Init.FirstBit = SPI_FIRSTBIT_MSB;
		_mpHandler->Init.NSS = SPI_NSS_SOFT;
		_mpHandler->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
		_mpHandler->Init.TIMode = SPI_TIMODE_DISABLE;
		_mpHandler->Init.Mode = SPI_MODE_MASTER;

		MP_SPI_CS_CLK_ENABLE();
		gpioinitstruct.Pin = MP_SPI_SCK_PIN;
		gpioinitstruct.Mode = GPIO_MODE_AF_PP;
		gpioinitstruct.Pull = GPIO_PULLUP;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		gpioinitstruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(MP_SPI_SCK_PORT, &gpioinitstruct);

		/* Configure SPI MISO and MOSI */
		gpioinitstruct.Pin = MP_SPI_DO_PIN;
		gpioinitstruct.Alternate = GPIO_AF5_SPI2;
		gpioinitstruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(MP_SPI_DO_PORT, &gpioinitstruct);

		MP_SPI_CS_CLK_ENABLE();
		gpioinitstruct.Pin = MP_SPI_CS_PIN;
		gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
		gpioinitstruct.Pull = GPIO_PULLUP;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(MP_SPI_CS_PORT, &gpioinitstruct);
		MP_SPI_CS_DISABLE();
		/*** Configure the SPI peripheral ***/
		/* Enable SPI clock */
		MP_SPI_CLK_ENABLE();
		HAL_SPI_Init(_mpHandler);

		multiplex_power_save_config(0);

		multiplexer_latching_set();
		HAL_Delay(50);
		multiplexer_reset();
		HAL_Delay(50);

//		multiplex_power_save_enable(1);

	}
}

void multiplexer_io_channel(uint16_t io)
{
	uint8_t data[5] =
	{ 0 };
	data[0] = REG_OUPUT_CTL;
	data[1] = io >> 0;
	data[2] = REG_OUPUT_CTL;
	data[3] = io >> 8;
	MP_SPI_CS_ENABLE();
	HAL_SPI_Transmit(_mpHandler, (uint8_t*) data, 4, 1000);
	MP_SPI_CS_DISABLE();
}

void multiplexer_reset(void)
{
	MP_RESET_ENABLE();
	MP_RESET_DISABLE();
}

void multiplexer_latching_set(void)
{
	MP_LATCHING_ON();
	MP_LATCHING_OFF();
}

void multiplex_power_save_config(uint8_t config)
{
	uint8_t data[2] =
	{ 0 };
	data[0] = REG_POWER_CFG;
	data[1] = config & 0x7;
	MP_SPI_CS_ENABLE();
	HAL_SPI_Transmit(_mpHandler, (uint8_t*) data, 2, 100);
	MP_SPI_CS_DISABLE();
}

void multiplex_power_save_enable(uint8_t en)
{
	if (en)
	{
		MP_PSAVE_ENABLE();
	}
	else
	{
		MP_PSAVE_DISABLE();
	}
}
