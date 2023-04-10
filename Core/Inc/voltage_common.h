/*
 * voltage_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef VOLTAGE_COMMON_H_
#define VOLTAGE_COMMON_H_

#include "stdbool.h"
#include "stdint.h"

typedef enum
{
	voltageStop, voltageReceiver, voltageRatio
} eVoltage_mode;

typedef enum
{
	voltageBusy, voltageReset
} eVoltage_state;

typedef struct
{
	float iVoltage_Value;
	uint8_t iVoltage_Status;
	uint8_t eMode;
	uint8_t eState;
} Voltage_t;

void VoltageSinkInit(void);
void VoltageSinkDeInit(void);

void VoltageSinkStart(void);
void VoltageSinkStop(void);

void VoltageSink_IRQHandler(void);

extern Voltage_t *hVoltage;
extern Voltage_t hVolt;
extern uint8_t VoltageSink_timer_Id;
extern bool bVoltageSinkInit;

#endif /* VOLTAGE_COMMON_H_ */
