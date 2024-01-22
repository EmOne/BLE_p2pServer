/*
 * voltage_common.h
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#ifndef MULTIPLEXER_COMMON_H_
#define MULTIPLEXER_COMMON_H_

#include "stdbool.h"
#include "stdint.h"

//typedef enum
//{
//	voltageStop, voltageReceiver, voltageRatio
//} eVoltage_mode;
//
//typedef enum
//{
//	voltageBusy, voltageReset
//} eVoltage_state;
//
//typedef struct
//{
//	float iVoltage_Value;
//	uint8_t iVoltage_Status;
//	uint8_t eMode;
//	uint8_t eState;
//} Voltage_t;
//
//void VoltageSinkInit(void);
//void VoltageSinkDeInit(void);
//
//void VoltageSinkStart(void);
//void VoltageSinkStop(void);
//
//void VoltageSink_IRQHandler(void);
//
//extern Voltage_t *hVoltage;
//extern Voltage_t hVolt;
//extern uint8_t VoltageSink_timer_Id;
//extern bool bVoltageSinkInit;

#define REG_OUPUT_CTL	0x00
#define REG_POWER_CFG	0x01

void multiplexer_init(void *_handler);
void multiplexer_io_channel(uint16_t io);
void multiplexer_reset(uint8_t state);
void multiplexer_latching_set(uint8_t state);
void multiplex_power_save_config(uint8_t config);
void multiplex_power_save_enable(uint8_t en);


#endif /* MULTIPLEXER_COMMON_H_ */
