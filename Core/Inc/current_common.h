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
	Stop, Receiver, Transmitter
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
	uint16_t iCurrent_Value;
	uint16_t iCurrent_Level;
	uint8_t Step_Dir;
	uint8_t eMode;
	uint8_t eState;
} Current_t;

void CurrentSinkStart(void);
void CurrentSinkStop(void);
void CurrentSourcePCT(void);
void CurrentSourceStep(void);
void CurrentSourceRamp(void);
void CurrentSourceStop(void);

extern Current_t *hCurrent;
extern uint8_t CurrentSource_Ramp_timer_Id;
extern uint8_t CurrentSource_Step_timer_Id;
extern uint8_t CurrentSink_timer_Id;

#endif /* CURRENT_COMMON_H_ */
