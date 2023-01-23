/*
 * current_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */
#include "main.h"
#include "stm32_seq.h"
#include "current_common.h"

void CurrentProcess(void)
{
	uint16_t amplitude;
	if (hCurrent->eState == Reset && hCurrent->eMode == Stop)
	{
		hCurrent->eMode = Receiver;

		hCurrent->eState = Busy;

		CS_T_HIGH();

		EN_R_HIGH();

		CS_R_LOW();

		CURENT_IO_Read(&amplitude);

		hCurrent->iCurrent_Value = amplitude & 0x0FFF;

		CS_R_HIGH();

		EN_R_LOW();

		HAL_Delay(100);

		hCurrent->eState = Reset;

		hCurrent->eMode = Stop;

		UTIL_SEQ_SetTask(1 << CFG_TASK_SW1_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);

	}
}

void CurrentStop(void)
{
	hCurrent->eMode = Stop;
	hCurrent->eState = Reset;
}

void CurrentPCT(void)
{
	uint16_t percent_to_amplitude;
	if (hCurrent->eState == Reset)
	{
		hCurrent->iCurrent_Level =
				(hCurrent->iCurrent_Level > 100) ? 100 :
				(hCurrent->iCurrent_Level < 0) ? 0 : hCurrent->iCurrent_Level;
		percent_to_amplitude = (1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude &= ~(1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15); //Channel A:0 B:1
		percent_to_amplitude |= ((hCurrent->iCurrent_Level * 4095) / 100) //12 Bits Interpolation
		& 0x0FFF;
		hCurrent->eState = Busy;
		CS_T_LOW();

		CURENT_IO_Write(percent_to_amplitude);

		CS_T_HIGH();
		hCurrent->eState = Reset;
	}
}

void CurrentStep(void)
{
	while (hCurrent->eMode == Transmitter)
	{
		hCurrent->Step_Dir = UP;

		if (hCurrent->eState == Reset)
		{
			if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 100)
			{
				hCurrent->iCurrent_Level += 10;
				hCurrent->iCurrent_Level =
						hCurrent->iCurrent_Level > 100 ?
								100 : hCurrent->iCurrent_Level;
			}

			UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_PCT_ID,
					CFG_SCH_PRIO_0);

			HAL_Delay(1000);
		}
		else
		{

		}
		if (hCurrent->iCurrent_Level >= 100 || hCurrent->eMode == Stop)
		{
			break;
		}
	}
}

void CurrentRamp(void)
{
	while (hCurrent->eMode == Transmitter)
	{
		if (hCurrent->eState == Reset)
		{
			if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 100)
				hCurrent->iCurrent_Level += 10;
			else
				hCurrent->Step_Dir = DOWN;

			if (hCurrent->Step_Dir == DOWN && hCurrent->iCurrent_Level > 0)
				hCurrent->iCurrent_Level -= 10;
			else
				hCurrent->Step_Dir = UP;

			UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_PCT_ID,
					CFG_SCH_PRIO_0);

			HAL_Delay(100);
		}
		else
		{

		}
		if (hCurrent->eMode == Stop)
		{
			break;
		}
	}
}

