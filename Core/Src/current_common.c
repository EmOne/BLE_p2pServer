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
	uint16_t le = 0;
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

		CS_R_HIGH();

		le = (amplitude >> 8) & 0xff;
		le |= (amplitude & 0xff) << 8;

		hCurrent->iCurrent_Value = (le) / 2;

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
	uint16_t le = 0;
//	bCurrentSourceStepInit = false;
//	bCurrentSourceRampInit = false;

	if (hCurrent->eState == Reset)
	{

		CS_R_HIGH();

		hCurrent->eMode = Transmitter;
		hCurrent->eState = Busy;

		percent_to_amplitude &= ~(1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude &= ~(1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15); //Channel A:0 B:1
		percent_to_amplitude &= ~(0x0FFF);

		le |= (percent_to_amplitude >> 8) & 0xff;
		le |= (percent_to_amplitude & 0xff) << 8;

		CS_T_LOW();

		CURENT_IO_Write(le);

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
	uint16_t percent_to_amplitude = 0;
	uint16_t le = 0;
	if (hCurrent->eState == Reset)
	{
		CS_R_HIGH();

		CurrentSourceStop();

		hCurrent->eMode = Transmitter;
		hCurrent->eState = Busy;

//		hCurrent->iCurrent_Level =
//				(hCurrent->iCurrent_Level > 100) ? 100 :
//				(hCurrent->iCurrent_Level < 0) ? 0 : hCurrent->iCurrent_Level;
		percent_to_amplitude |= (1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude &= ~(1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15);  //Channel A:0 B:1
		percent_to_amplitude |= hCurrent->iCurrent_Level & 0x0FFF; //((hCurrent->iCurrent_Level * 4095) / 100) //12 Bits Interpolation

		le |= (percent_to_amplitude >> 8) & 0xff;
		le |= (percent_to_amplitude & 0xff) << 8;

		CS_T_LOW();

		CURENT_IO_Write(le);

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


	if (hCurrent->iCurrent_Level >= 2400)
	{
		bCurrentSourceStepInit = false;
		CurrentSourceStop();
	}
	else
	{
		if (hCurrent->eState == Reset)
		{
			if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 2400)
			{
				hCurrent->iCurrent_Level += 100;
			}

			hCurrent->iCurrent_Level =
					hCurrent->iCurrent_Level > 2400 ?
							2400 : hCurrent->iCurrent_Level;
			CurrentSourcePCT();

		}

		HW_TS_Start(CurrentSource_Step_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
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

	if (hCurrent->eState == Reset)
	{
		if (hCurrent->Step_Dir == UP && hCurrent->iCurrent_Level < 2400)
			hCurrent->iCurrent_Level += 100;
		else
			hCurrent->Step_Dir = DOWN;

		if (hCurrent->Step_Dir == DOWN && hCurrent->iCurrent_Level > 0)
		{
			hCurrent->iCurrent_Level -= 100;
		}
		
		if (hCurrent->Step_Dir == DOWN && hCurrent->iCurrent_Level <= 0)
		{
			bCurrentSourceRampInit = false;
			hCurrent->Step_Dir = UP;
			return;
		}

		CurrentSourcePCT();
		HW_TS_Start(CurrentSource_Ramp_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
		
	}



}
