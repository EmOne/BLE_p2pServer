/*
 * current_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "vibrate_common.h"

Vibrate_t hVib;
bool bVibrateSinkInit = false;
#define SENSOR_ADDR 0x4D
void VibrateSinkInit(void)
{
	I2Cx_Init();
}
void VibrateSinkDeInit(void)
{

}

void VibrateSinkStop(void)
{
	HW_TS_Stop(VibrateSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hVibrate->eMode = Stop;
	bVibrateSinkInit = false;
	VibrateSinkDeInit();
}

void VibrateSinkStart(void)
{
	if (hVibrate == NULL) {
		hVibrate = &hVib;
	}

	if (!bVibrateSinkInit)
	{
		VibrateSinkInit();
		bVibrateSinkInit = true;
		hVibrate->eMode = vibrateReceiver;
	}

//	if (hVibrate->eState == Reset && hVibrate->eMode != Stop)
//	{
//		hVibrate->eState = Busy;
//
//		COMMON_IO_Read(&amplitude);
//
//		le = (amplitude >> 8) & 0xff;
//		le |= (amplitude & 0xff) << 8;
//
//		hVibrate->iVibrate_Value = (uint16_t) ((((float) le / 4095.0f) / 2.048f)
//				* 2400.0f);
//
//		hVibrate->eState = Reset;
//
		HW_TS_Start(VibrateSink_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
//
//		BSP_LED_Toggle(LED_BLUE);
//
//		P2PS_Send_Notification();
//	}
	VibrateSink_IRQHandler();
}

void VibrateSink_IRQHandler(void){
	uint16_t amplitude;
		uint16_t le = 0;

	I2Cx_Read(SENSOR_ADDR << 1, &amplitude);

	le = (amplitude >> 8) & 0xff;
	le |= (amplitude & 0x0f) << 8;

	hVibrate->iVibrate_Value = le * 10;

	BSP_LED_Toggle(LED_BLUE);

	P2PS_Send_Notification();
}
