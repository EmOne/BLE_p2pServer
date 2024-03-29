/*
 * vibrate_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef VIBRATE_COMMON_H_
#define VIBRATE_COMMON_H_

#include "stdbool.h"
#include "stdint.h"

typedef enum
{
	vibrateStop, vibrateReceiver
} eVibrate_mode;

typedef enum
{
	vibrateBusy, vibrateReset
} eVibrate_state;

typedef struct
{
	float iVibrate_Value;
	uint8_t iVibrate_Status;
	uint8_t eMode;
	uint8_t eState;
} Vibrate_t;

void VibrateSinkInit(void);
void VibrateSinkDeInit(void);
void VibrateSinkStart(void);
void VibrateSinkStop(void);
void VibrateSink_IRQHandler(void);

extern Vibrate_t *hVibrate;
extern Vibrate_t hVib;
extern uint8_t VibrateSink_timer_Id;
extern bool bVibrateSinkInit;

#endif /* VIBRATE_COMMON_H_ */
