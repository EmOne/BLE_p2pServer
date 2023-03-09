/*
 * current_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */
#include "stdbool.h"
#include "stdint.h"

#include "main.h"
#include "stm32_seq.h"
#include "vibrate_common.h"

bool bVibrateSinkInit = false;

void VibrateSinkStop(void)
{
	HW_TS_Stop(VibrateSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hVibrate->eMode = Stop;
	bVibrateSinkInit = false;
}

void VibrateSinkStart(void)
{
	uint16_t amplitude;
	uint16_t le = 0;

	if (!bVibrateSinkInit)
	{
		bVibrateSinkInit = true;
//		hVibrate->eMode = Receiver;
	}

//	if (hVibrate->eState == Reset && hVibrate->eMode != Stop)
//	{
//		hVibrate->eState = Busy;
//
//		CURENT_IO_Read(&amplitude);
//
//		le = (amplitude >> 8) & 0xff;
//		le |= (amplitude & 0xff) << 8;
//
//		hVibrate->iVibrate_Value = (uint16_t) ((((float) le / 4095.0f) / 2.048f)
//				* 2400.0f);
//
//		hVibrate->eState = Reset;
//
//		HW_TS_Start(VibrateSink_timer_Id,
//				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
//
//		BSP_LED_Toggle(LED_BLUE);
//
//		P2PS_Send_Notification();
//	}
}

