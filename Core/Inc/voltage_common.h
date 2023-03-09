/*
 * voltage_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef VOLTAGE_COMMON_H_
#define VOLTAGE_COMMON_H_


typedef enum
{
	voltageStop, voltageReceiver
} eVoltage_mode;

typedef enum
{
	voltageBusy, voltageReset
} eVoltage_state;

typedef struct
{
	uint16_t iVoltage_Value;
	uint16_t iVoltage_Level;
	uint8_t eMode;
	uint8_t eState;
} Voltage_t;

void VoltageSinkStart(void);
void VoltageSinkStop(void);

extern Voltage_t *hVoltage;
extern uint8_t VoltageSink_timer_Id;

#endif /* VOLTAGE_COMMON_H_ */
