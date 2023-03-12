/*
 * voltage_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "voltage_common.h"

static Voltage_t hVolt;
bool bVoltageSinkInit = false;

void VoltageSinkInit(void)
{
	if (hVoltage == NULL)
	{
		hVoltage = &hVolt;
	}


}
void VoltageSinkDeInit(void)
{

}

void VoltageSinkStop(void)
{
	HW_TS_Stop(VoltageSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hVoltage->eMode = Stop;
	bVoltageSinkInit = false;
}

void VoltageSinkStart(void)
{
	uint16_t amplitude;
	uint16_t le = 0;

	if (!bVoltageSinkInit)
	{
		bVoltageSinkInit = true;
//		hVoltage->eMode = Receiver;
	}

//	if (hVoltage->eState == Reset && hVoltage->eMode != Stop)
//	{
//		hVoltage->eState = Busy;
//
//		COMMON_IO_Read(&amplitude);
//
//		le = (amplitude >> 8) & 0xff;
//		le |= (amplitude & 0xff) << 8;
//
//		hVoltage->iVoltage_Value = (uint16_t) ((((float) le / 4095.0f) / 2.048f)
//				* 2400.0f);
//
//		hVoltage->eState = Reset;
//
//		HW_TS_Start(VoltageSink_timer_Id,
//				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
//
//		BSP_LED_Toggle(LED_BLUE);
//
//		P2PS_Send_Notification();
//	}
}

