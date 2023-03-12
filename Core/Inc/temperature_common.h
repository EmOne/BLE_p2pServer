/*
 * temperature_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef TEMPERATURE_COMMON_H_
#define TEMPERATURE_COMMON_H_

#include "stdbool.h"
#include "stdint.h"

typedef enum
{
	temperatureStop, temperatureReceiver
} eTemperature_mode;

typedef enum
{
	temperatureDRDY,
	temperatureUNKNOWN
} eTemperature_state;

typedef struct
{
	uint16_t iTemperature_Value;
	uint16_t iTemperature_Level;
	uint8_t eMode;
	uint8_t eState;
} Temperature_t;

void TemperatureSinkInit(void);
void TemperatureSinkDeInit(void);
void TemperatureSinkStart(void);
void TemperatureSinkStop(void);
void TemperatureSink_IRQHandler(void);

extern Temperature_t *hTemperature;
extern uint8_t TemperatureSink_timer_Id;

extern bool bTemperatureSinkInit;

#endif /* TEMPERATURE_COMMON_H_ */
