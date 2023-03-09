/*
 * vibrate_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef VIBRATE_COMMON_H_
#define VIBRATE_COMMON_H_


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
	uint16_t iVibrate_Value;
	uint16_t iVibrate_Level;
	uint8_t eMode;
	uint8_t eState;
} Vibrate_t;

void VibrateSinkStart(void);
void VibrateSinkStop(void);

extern Vibrate_t *hVibrate;
extern uint8_t VibrateSink_timer_Id;

#endif /* VIBRATE_COMMON_H_ */
