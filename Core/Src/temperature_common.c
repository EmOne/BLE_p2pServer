/*
 * temperature_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */
#include "stdbool.h"
#include "stdint.h"

#include "main.h"
#include "stm32_seq.h"
#include "temperature_common.h"

bool bTemperatureSinkInit = false;

void TemperatureSinkStop(void)
{
	HW_TS_Stop(TemperatureSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hTemperature->eMode = Stop;
	bTemperatureSinkInit = false;
}

void TemperatureSinkStart(void)
{
	uint16_t amplitude;
	uint16_t le = 0;

	if (!bTemperatureSinkInit)
	{
		bTemperatureSinkInit = true;
//		hTemperature->eMode = Receiver;
	}

//	if (hTemperature->eState == Reset && hTemperature->eMode != Stop)
//	{
//		hTemperature->eState = Busy;
//
//		CURENT_IO_Read(&amplitude);
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
//		HW_TS_Start(TemperatureSink_timer_Id,
//				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
//
//		BSP_LED_Toggle(LED_BLUE);
//
//		P2PS_Send_Notification();
//	}
}

