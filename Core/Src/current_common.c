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
#include "current_common.h"

bool bCurrentSourceStepInit = false;
bool bCurrentSourceRampInit = false;
bool bCurrentSinkInit = false;

void CurrentSinkStop(void)
{
	HW_TS_Stop(CurrentSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

	CS_R_HIGH();

	hCurrent->eMode = Stop;
	bCurrentSinkInit = false;
	EN_R_LOW();

}

void CurrentSinkStart(void)
{
	uint16_t amplitude;

	if (!bCurrentSinkInit)
	{
		bCurrentSinkInit = true;
		hCurrent->eMode = Receiver;
		EN_R_HIGH();
	}

	if (hCurrent->eState == Reset && hCurrent->eMode != Stop)
	{
		hCurrent->eState = Busy;

		CS_R_LOW();

		CURENT_IO_Read(&amplitude);

		hCurrent->iCurrent_Value = amplitude & 0x0FFF;

		hCurrent->eState = Reset;

		HW_TS_Start(CurrentSink_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));

		BSP_LED_Toggle(LED_BLUE);

		P2PS_Send_Notification();
	}
}

void CurrentSourceStop(void)
{
	uint32_t percent_to_amplitude = 0;
	bCurrentSourceStepInit = false;
	bCurrentSourceRampInit = false;

	if (hCurrent->eState == Reset)
	{

		CS_R_HIGH();

		hCurrent->eMode = Transmitter;
		hCurrent->eState = Busy;

		percent_to_amplitude &= ~(1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude &= ~(1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15); //Channel A:0 B:1
		percent_to_amplitude |= 0x0FFF;

		CS_T_LOW();

		CURENT_IO_Write(percent_to_amplitude);

		CS_T_HIGH();

		hCurrent->eState = Reset;
		hCurrent->eMode = Stop;

		if (bCurrentSinkInit)
		{
			CurrentSinkStart();
		}
	}
}

void CurrentSourcePCT(void)
{
	uint16_t percent_to_amplitude;
	if (hCurrent->eState == Reset)
	{
		CS_R_HIGH();

		CurrentSourceStop();

		hCurrent->eMode = Transmitter;
		hCurrent->eState = Busy;

//		hCurrent->iCurrent_Level =
//				(hCurrent->iCurrent_Level > 100) ? 100 :
//				(hCurrent->iCurrent_Level < 0) ? 0 : hCurrent->iCurrent_Level;
		percent_to_amplitude = (1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude &= ~(1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15); //Channel A:0 B:1
		percent_to_amplitude |= hCurrent->iCurrent_Level & 0x0FFF; //((hCurrent->iCurrent_Level * 4095) / 100) //12 Bits Interpolation

		CS_T_LOW();

		CURENT_IO_Write(percent_to_amplitude);

		CS_T_HIGH();
		hCurrent->eState = Reset;

		if (bCurrentSinkInit)
		{
			CurrentSinkStart();
		}
	}
}

void CurrentSourceStep(void)
{
	if (!bCurrentSourceStepInit)
	{
		CurrentSourceStop();

		bCurrentSourceStepInit = true;
		hCurrent->Step_Dir = UP;
		hCurrent->iCurrent_Level = 0;
	}


	if (hCurrent->iCurrent_Level >= 4095 || hCurrent->eMode == Stop)
	{
		CurrentSourceStop();
	}
	else
	{
		if (hCurrent->eState == Reset)
		{
			if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 4095)
			{
				hCurrent->iCurrent_Level += 100;
			}

			hCurrent->iCurrent_Level =
					hCurrent->iCurrent_Level > 4095 ?
							4095 : hCurrent->iCurrent_Level;
			CurrentSourcePCT();

			HAL_Delay(500);
		}

		UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_STEP_ID,
				CFG_SCH_PRIO_0);
	}

}

void CurrentSourceRamp(void)
{
	if (!bCurrentSourceRampInit)
	{
		CurrentSourceStop();

		bCurrentSourceRampInit = true;
		hCurrent->Step_Dir = UP;
		hCurrent->iCurrent_Level = 0;
	}

	if (hCurrent->eMode == Stop)
	{
		CurrentSourceStop();
	}
	else
	{
		if (hCurrent->eState == Reset)
		{
			if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 4095)
				hCurrent->iCurrent_Level += 10;
			else
				hCurrent->Step_Dir = DOWN;

			if (hCurrent->Step_Dir == DOWN && hCurrent->iCurrent_Level > 0)
				hCurrent->iCurrent_Level -= 10;
			else
				hCurrent->Step_Dir = UP;

			CurrentSourcePCT();

			HAL_Delay(500);
		}
		UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_RAMP_ID,
				CFG_SCH_PRIO_0);
	}

}
