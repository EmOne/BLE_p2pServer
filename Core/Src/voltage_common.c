/*
 * voltage_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "voltage_common.h"

Voltage_t hVolt;
bool bVoltageSinkInit = false;

#if 1
#define FILT_PORT 	GPIOA
#define FILT_PIN	GPIO_PIN_4
#define FILT_LOW()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_RESET)
#define FILT_HIGH()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_SET)
#define REST_PORT	GPIOA
#define REST_PIN	GPIO_PIN_0
#define REST_LOW()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_RESET)
#define REST_HIGH()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_SET)
#define GAIN_PORT	GPIOA
#define GAIN_PIN	GPIO_PIN_8
#define GAIN_LOW()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_RESET)
#define GAIN_HIGH()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_SET)
#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define INT_PIN GPIO_PIN_6 //EN_R_Pin
#define INT_PORT GPIOC //EN_R_Port
#define INT_IRQn EXTI9_5_IRQn //EN_R_IRQn
#define INT_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE() //EN_R_GPIO_CLK_ENABLE()

#else
#define FILT_PORT 	GPIOA
#define FILT_PIN	GPIO_PIN_9
#define REST_PORT	GPIOA
#define REST_PIN	GPIO_PIN_1
#define GAIN_PORT	GPIOA
#define GAIN_PIN	GPIO_PIN_15
#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#endif

void VoltageSinkInit(void)
{

	V_CLK_ENABLE();
	GPIO_InitTypeDef gpioinitstruct = { 0 };

	gpioinitstruct.Pin = INT_PIN;
	gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;
	gpioinitstruct.Pull = GPIO_PULLUP;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(INT_PORT, &gpioinitstruct);

	gpioinitstruct.Pin = FILT_PIN;
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(FILT_PORT, &gpioinitstruct);
	FILT_HIGH();

	gpioinitstruct.Pin = REST_PIN;
	HAL_GPIO_Init(REST_PORT, &gpioinitstruct);
	REST_HIGH();

	gpioinitstruct.Pin = GAIN_PIN;
	HAL_GPIO_Init(GAIN_PORT, &gpioinitstruct);
	GAIN_HIGH();

	COMMON_IO_Init();
}
void VoltageSinkDeInit(void)
{
	HAL_NVIC_DisableIRQ(INT_IRQn);
	HAL_GPIO_DeInit(INT_PORT, INT_PIN);
	HAL_GPIO_DeInit(REST_PORT, REST_PIN);
	HAL_GPIO_DeInit(GAIN_PORT, GAIN_PIN);
	HAL_GPIO_DeInit(FILT_PORT, FILT_PIN);
}

void VoltageSinkStop(void)
{
	HW_TS_Stop(VoltageSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hVoltage->eMode = Stop;
	bVoltageSinkInit = false;
	VoltageSinkDeInit();
}

void VoltageSinkStart(void)
{

	if (hVoltage == NULL) {
		hVoltage = &hVolt;
	}

	if (!bVoltageSinkInit)
	{
		VoltageSinkInit();

		bVoltageSinkInit = true;
		hVoltage->eMode = voltageReceiver;

		HAL_NVIC_SetPriority(INT_IRQn, 0x0F, 0x00);
		HAL_NVIC_EnableIRQ(INT_IRQn);

		REST_LOW();
		HAL_Delay(10);
		REST_HIGH();
		/* Enable and set Button EXTI Interrupt to the lowest priority */

	}

//	if (hVoltage->eState == Reset && hVoltage->eMode != Stop)
//	{
//		hVoltage->eState = Busy;
//
//		COMMON_IO_Read(&rReg, 3);
////
//		le = (rReg[0]) & 0xff;
//		le |= (rReg[1] & 0xff) << 8;
//		le |= (rReg[2] & 0xff) << 16;
//		hVoltage->iVoltage_Level = le;
//		hVoltage->iVoltage_Value = (uint32_t) ((((float) le / (0XFFFFF >> 1)) * 2.048f)
//				* 100.0f) - 1;
////
		hVoltage->eState = voltageReset;
//
		HW_TS_Start(VoltageSink_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
////
////		BSP_LED_Toggle(LED_BLUE);
////
////		P2PS_Send_Notification();
////	}
		VoltageSink_IRQHandler();
}

void VoltageSink_IRQHandler(void)
{
	uint8_t rReg[3] = { 0x00 };
	uint32_t le = 0;

	HAL_NVIC_DisableIRQ(INT_IRQn);

	COMMON_IO_Read(&rReg, 3);

	HAL_NVIC_EnableIRQ(INT_IRQn);
	//
	le = (rReg[0]) & 0xff;
	le |= (rReg[1] & 0xff) << 8;
	le |= (rReg[2] & 0xff) << 16;
	hVoltage->iVoltage_Level = le;
	hVoltage->iVoltage_Value = (uint32_t) ((((float) le / (0XFFFFF >> 1))
			* 2.048f) * 100.0f) - 1;
	//
	hVoltage->eState = voltageReset;

	BSP_LED_Toggle(LED_BLUE);
	P2PS_Send_Notification();
}
