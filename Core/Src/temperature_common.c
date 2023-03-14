/*
 * temperature_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */
#include "main.h"
#include "stm32_seq.h"
#include "temperature_common.h"

bool bTemperatureSinkInit = false;

#if 0
#define T_INT_PIN EN_R_Pin
#define T_INT_PORT EN_R_Port
#define T_INT_IRQn EN_R_IRQn
#define T_INT_CLK_ENABLE() EN_R_GPIO_CLK_ENABLE()
#else
#define T_INT_PIN GPIO_PIN_6 //EN_R_Pin
#define T_INT_PORT GPIOC //EN_R_Port
#define T_INT_IRQn EXTI9_5_IRQn //EN_R_IRQn
#define T_INT_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE() //EN_R_GPIO_CLK_ENABLE()
#endif
void TemperatureSinkInit(void)
{
	//TODO: INT pin configuration
	T_INT_CLK_ENABLE();
	GPIO_InitTypeDef gpioinitstruct =
	{ 0 };
	gpioinitstruct.Pin = T_INT_PIN;
	gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;
	gpioinitstruct.Pull = GPIO_PULLUP;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(T_INT_PORT, &gpioinitstruct);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(T_INT_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(T_INT_IRQn);

	COMMON_IO_Init();
}

void TemperatureSinkDeInit(void)
{
	HAL_NVIC_DisableIRQ(T_INT_IRQn);
	HAL_GPIO_DeInit(T_INT_PORT, T_INT_PIN);
}

void TemperatureSinkStop(void)
{
	HW_TS_Stop(TemperatureSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hTemperature->eMode = Stop;
	bTemperatureSinkInit = false;
	TemperatureSinkDeInit();
}

void TemperatureSinkStart(void)
{
	uint8_t wReg[2] =
	{ 0x80, 0x00 };
	uint8_t config = 0;

	if (!bTemperatureSinkInit)
	{
		bTemperatureSinkInit = true;
		hTemperature->eMode = temperatureReceiver;
		TemperatureSinkInit();

		config |= (1 << 7);	//D7 V bias
		//	config |= (1 << 6);	//D6 Conversion mode 0: Normally off, 1: Auto
		//		config |= (1 << 5);	//D5 1: 1-Shot
		config |= (hTemperature->eWire << 4);	//D4 0: 3 Wire, 1: 2/4 Wire
		//		config |= (3 << 2);	//D2-D3 Fault detect cycle control
		//		config |= (1 << 1);	//D1 Fault status clear
		//		config |= (1 << 0);	//D0 Filter 0: 60 Hz, 1: 50 Hz
		//		config <<= 8;
		wReg[1] = config;

		CS_T_LOW();

		COMMON_IO_Write(wReg, 2);

		CS_T_HIGH();

		wReg[0] = 0x00;
		HAL_Delay(100);
		CS_T_LOW();

		COMMON_IO_Read(wReg, 2);

		CS_T_HIGH();

		if (HAL_GPIO_ReadPin(T_INT_PORT, T_INT_PIN) == GPIO_PIN_RESET)
		{
			TemperatureSink_IRQHandler();
		}
	}




//	if (hTemperature->eState == Reset && hTemperature->eMode != Stop)
//	{
//		hTemperature->eState = Busy;
//
//		COMMON_IO_Read(&amplitude);
//
//		le = (amplitude >> 8) & 0xff;
//		le |= (amplitude & 0xff) << 8;
//
//		hTemperature->iTemperature_Value = (uint16_t) ((((float) le / 4095.0f)
//				/ 2.048f)
//				* 2400.0f);
//
//		hTemperature->eState = Reset;
//
	HW_TS_Start(TemperatureSink_timer_Id,
			(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
	TemperatureSink_IRQHandler();
//	}

}

void TemperatureSink_IRQHandler(void)
{
	uint16_t amplitude;
	uint16_t le = 0;

	CS_T_LOW();

	amplitude = 0x0001;

	COMMON_IO_Read(&amplitude, 2);

	le |= ((amplitude) & 0xff00);

	amplitude = 0x0002;

	COMMON_IO_Read(&amplitude, 2);

	le = (amplitude >> 8) & 0xff;

	CS_T_HIGH();
	
	hTemperature->iTemperature_Value =
			(uint16_t) ((((float) (le / 2.0f) / 32.0f)
			- 256.0f) * 100.0f);

	BSP_LED_Toggle(LED_BLUE);
	P2PS_Send_Notification();
}
