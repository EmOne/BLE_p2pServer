/*
 * current_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "current_common.h"
#include "multiplex_common.h"

bool bCurrentSourceStepInit = false;
bool bCurrentSourceRampInit = false;
bool bCurrentSinkInit = false;

/* Current IO function */

void CurrentSinkInit(void)
{
	GPIO_InitTypeDef gpioinitstruct =
	{ 0 };

	/* CS_GPIO and DC_GPIO Periph clock enable */
	CS_R_GPIO_CLK_ENABLE();

	/* Configure CS_PIN pin: CS pin */
	gpioinitstruct.Pin = CS_R_Pin;
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(CS_R_Port, &gpioinitstruct);
	gpioinitstruct.Pin = EN_R_Pin;
	HAL_GPIO_Init(EN_R_Port, &gpioinitstruct);

	EN_R_LOW();

	/* chip select high */
	CS_R_HIGH();

	COMMON_IO_Init();

}

void CurrentSinkDeInit(void)
{
	HAL_GPIO_DeInit(CS_R_Port, CS_R_Pin);
	HAL_GPIO_Init(EN_R_Port, EN_R_Pin);
}

void CurrentSinkStop(void)
{
	HW_TS_Stop(CurrentSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

	CS_R_HIGH();

	hCurrent->eMode = Stop;
	bCurrentSinkInit = false;
	EN_R_LOW();

	CurrentSinkDeInit();
}

void CurrentSinkStart(void)
{
	uint16_t amplitude;
	uint16_t le = 0;
	if (!bCurrentSinkInit)
	{
		bCurrentSinkInit = true;
		hCurrent->eMode = Receiver;
		CurrentSinkInit();

		EN_R_HIGH();
	}

	if (hCurrent->eState == Reset && hCurrent->eMode != Stop)
	{
		hCurrent->eState = Busy;

		CS_R_LOW();

		COMMON_IO_Read(&amplitude, 2);

		CS_R_HIGH();

		le = (amplitude >> 8) & 0xff;
		le |= (amplitude & 0xff) << 8;

		hCurrent->iCurrent_Value = (uint16_t) ((((float) le / 4095.0f) / 2.048f)
				* 2400.0f);

		hCurrent->eState = Reset;

		HW_TS_Start(CurrentSink_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));

		BSP_LED_Toggle(LED_BLUE);

		P2PS_Send_Notification();
	}
}

void CurrentSourceInit(void)
{

	GPIO_InitTypeDef gpioinitstruct =
	{ 0 };

	/* CS_GPIO and DC_GPIO Periph clock enable */
	CS_T_GPIO_CLK_ENABLE();

	/* Configure CS_PIN pin: CS pin */
	gpioinitstruct.Pin = CS_T_Pin;
	gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(CS_T_Port, &gpioinitstruct);

	/* chip select high */
	CS_T_HIGH();

	COMMON_IO_Init();
}
void CurrentSourceDeInit(void)
{
	HAL_GPIO_Init(CS_T_Port, CS_T_Pin);
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

		COMMON_IO_Write(&le, 2);

		CS_T_HIGH();

		hCurrent->eState = Reset;
		hCurrent->eMode = Stop;

		CurrentSourceDeInit();

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

		CurrentSourceInit();
//		hCurrent->iCurrent_Level =
//				(hCurrent->iCurrent_Level > 100) ? 100 :
//				(hCurrent->iCurrent_Level < 0) ? 0 : hCurrent->iCurrent_Level;
		percent_to_amplitude |= (1 << 12); //Enable
		percent_to_amplitude &= ~(1 << 13); //Gain 1:2x 0:1x
		percent_to_amplitude |= (1 << 14); //Buffering
		percent_to_amplitude &= ~(1 << 15);  //Channel A:0 B:1
		percent_to_amplitude |= hCurrent->iCurrent_Level & 0x0FFF; //((hCurrent->iCurrent_Level * 4095) / 100) //12 Bits Interpolation

		le |= (percent_to_amplitude >> 8) & 0xff;
		le |= (percent_to_amplitude & 0xff) << 8;

		CS_T_LOW();

		COMMON_IO_Write(&le, 2);

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
