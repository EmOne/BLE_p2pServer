/*
 * temperature_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef TEMPERATURE_COMMON_H_
#define TEMPERATURE_COMMON_H_


typedef enum
{
	temperatureStop, temperatureReceiver
} eTemperature_mode;

typedef enum
{
	temperatureDRDY,
} eTemperature_state;

typedef struct
{
	uint16_t iTemperature_Value;
	uint16_t iTemperature_Level;
	uint8_t eMode;
	uint8_t eState;
} Temperature_t;

void TemperatureSinkStart(void);
void TemperatureSinkStop(void);

extern Temperature_t *hTemperature;
extern uint8_t TemperatureSink_timer_Id;

#endif /* TEMPERATURE_COMMON_H_ */
