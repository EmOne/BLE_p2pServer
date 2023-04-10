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
	temperatureFAULT,
	temperatureUNKNOWN = 0xFF
} eTemperature_state;

typedef enum
{
	temperature3W, temperature2_4W
} eTemperature_wire;


typedef struct
{
	int16_t iTemperature_Value;
	uint16_t iTemperature_Level;
	uint8_t eWire;
	uint8_t eMode;
	uint8_t eState;
} Temperature_t;

void TemperatureSinkInit(void);
void TemperatureSinkDeInit(void);
void TemperatureSinkStart(void);
void TemperatureSinkStop(void);
void TemperatureSink_IRQHandler(void);

extern Temperature_t *hTemperature;
extern Temperature_t hTemp;
extern uint8_t TemperatureSink_timer_Id;

extern bool bTemperatureSinkInit;

#endif /* TEMPERATURE_COMMON_H_ */
