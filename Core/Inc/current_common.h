/*
 * current_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef CURRENT_COMMON_H_
#define CURRENT_COMMON_H_


typedef enum
{
	Receiver, Transmitter, Stop
} eCurrent_mode;

typedef enum
{
	Busy, Reset
} eCurrent_state;

typedef enum
{
	UP, DOWN
} eCurrent_step_dir;

typedef struct
{
	uint8_t iCurrent_Level;
	uint16_t iCurrent_Value;
	eCurrent_step_dir Step_Dir;
	eCurrent_mode eMode;
	eCurrent_state eState;
} Current_t;

void CurrentProcess(void);
void CurrentPCT(void);
void CurrentStep(void);
void CurrentRamp(void);
void CurrentStop(void);

extern Current_t *hCurrent;

#endif /* CURRENT_COMMON_H_ */
