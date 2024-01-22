/*
 * temperature_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */
#include "main.h"
#include "stm32_seq.h"
#include "temperature_common.h"
#include "multiplex_common.h"

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

Temperature_t hTemp;

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

	gpioinitstruct.Pin = CS_T_Pin;
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(CS_T_Port, &gpioinitstruct);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(T_INT_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(T_INT_IRQn);

	COMMON_IO_Init();
}

void TemperatureSinkDeInit(void)
{
	HAL_NVIC_DisableIRQ(T_INT_IRQn);
	HAL_GPIO_DeInit(T_INT_PORT, T_INT_PIN);
	HAL_GPIO_DeInit(CS_T_Port, CS_T_Pin);
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
	uint8_t wReg[5] =
	{ 0x80, 0x00 };
	uint8_t config = 0;

	if (hTemperature == NULL)
	{
		hTemperature = &hTemp;
	}

	if (!bTemperatureSinkInit)
	{
		TemperatureSinkInit();


		bTemperatureSinkInit = true;
		hTemp.eMode = temperatureReceiver;

		config |= (1 << 7);	//D7 V bias
		config |= (1 << 6);	//D6 Conversion mode 0: Normally off, 1: Auto
//		config |= (1 << 5);	//D5 1: 1-Shot
		config |= (hTemperature->eWire << 4);	//D4 0: 3 Wire, 1: 2/4 Wire
		//		config |= (3 << 2);	//D2-D3 Fault detect cycle control
		config |= (1 << 1);	//D1 Fault status clear
		//		config |= (1 << 0);	//D0 Filter 0: 60 Hz, 1: 50 Hz
		//		config <<= 8;

		wReg[0] = 0x80;
		wReg[1] = config;

		CS_T_LOW();

		COMMON_IO_Write(wReg, 2);

		CS_T_HIGH();

		//Setting fault threshold
		wReg[0] = 0x03;
		wReg[1] = 0xfe;
		wReg[2] = 0xfe;
		wReg[3] = 0x00;
		wReg[4] = 0x00;
		CS_T_LOW();
		COMMON_IO_Write(wReg, 5);
		CS_T_HIGH();


		if (HAL_GPIO_ReadPin(T_INT_PORT, T_INT_PIN) == GPIO_PIN_RESET)
		{
			TemperatureSink_IRQHandler();
		}
	}
//	else
	{
		wReg[0] = 0x00;
		HAL_Delay(100);
		CS_T_LOW();

		COMMON_IO_Read(wReg, 2);

		CS_T_HIGH();
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
	uint8_t rReg[3] = { 0x00 };
	uint16_t le = 0;
	bool bFault = false;

	rReg[0] = 0x01;
	CS_T_LOW();

	COMMON_IO_Read(rReg, 3);

	CS_T_HIGH();

	le = (uint16_t)rReg[1] << 8 | rReg[2];

	HAL_Delay(10);

	hTemp.eState = bFault = (le & 0x0001);
	le >>= 1;
	le = 0x7fff - le;
	//R-ref 3KOhm with RTD2K @ 20C MAX:21844 (5554h) Ratio 0.666

	hTemp.iTemperature_Value =
			(int16_t) ((((float) le * 0.00130577f)
			+ 21.3234f) * 100.0f);

	if (bFault) {
		rReg[0] = 0x07;
		CS_T_LOW();

		COMMON_IO_Read(rReg, 2);

		CS_T_HIGH();
		hTemp.iTemperature_Level = rReg[1];
	} else {
		hTemp.iTemperature_Level = 0x00;
	}

	BSP_LED_Toggle(LED_BLUE);
	P2PS_Send_Notification();
}
