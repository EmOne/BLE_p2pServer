/*
 * voltage_common.c
 *
 *  Created on: Jan 20, 2023
 *      Author: anol_
 */

#include "main.h"
#include "stm32_seq.h"
#include "voltage_common.h"

Voltage_t hVolt;
bool bVoltageSinkInit = false;

#if 1
#define FILT_PORT 	GPIOA
#define FILT_PIN	GPIO_PIN_4
#define FILT_LOW()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_RESET)
#define FILT_HIGH()	HAL_GPIO_WritePin(FILT_PORT, FILT_PIN, GPIO_PIN_SET)
#define REST_PORT	GPIOA
#define REST_PIN	GPIO_PIN_0
#define REST_LOW()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_RESET)
#define REST_HIGH()	HAL_GPIO_WritePin(REST_PORT, REST_PIN, GPIO_PIN_SET)
#define GAIN_PORT	GPIOA
#define GAIN_PIN	GPIO_PIN_8
#define GAIN_LOW()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_RESET)
#define GAIN_HIGH()	HAL_GPIO_WritePin(GAIN_PORT, GAIN_PIN, GPIO_PIN_SET)
#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define INT_PIN GPIO_PIN_6 //EN_R_Pin
#define INT_PORT GPIOC //EN_R_Port
#define INT_IRQn EXTI9_5_IRQn //EN_R_IRQn
#define INT_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE() //EN_R_GPIO_CLK_ENABLE()

#define AN_1_PORT	ADCx_CHANNELa_GPIO_PORT
#define AN_1_PIN	ADCx_CHANNELa_PIN
#define AN_1_CLK_ENABLE ADCx_CHANNELa_GPIO_CLK_ENABLE

#define AN_CS_PORT			CS_R_Port
#define AN_CS_PIN			CS_R_Pin
#define AN_CS_CLK_ENABLE	CS_R_GPIO_CLK_ENABLE
#define AN_CS_LOW()	HAL_GPIO_WritePin(AN_CS_PORT, AN_CS_PIN, GPIO_PIN_RESET)
#define AN_CS_HIGH()	HAL_GPIO_WritePin(AN_CS_PORT, AN_CS_PIN, GPIO_PIN_SET)

/* Variables for ADC conversion data */
__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE; /* ADC group regular conversion data */

/* Variables for ADC conversion data computation to physical values */
uint16_t uhADCxConvertedData_Voltage_mVolt = 0; /* Value of voltage calculated from ADC conversion data (unit: mV) */

#else
#define FILT_PORT 	GPIOA
#define FILT_PIN	GPIO_PIN_9
#define REST_PORT	GPIOA
#define REST_PIN	GPIO_PIN_1
#define GAIN_PORT	GPIOA
#define GAIN_PIN	GPIO_PIN_15
#define V_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#endif

void VoltageSinkInit(void)
{
	GPIO_InitTypeDef gpioinitstruct = { 0 };

	if (hVolt.eMode == voltageRatio)
	{
		V_CLK_ENABLE();
		gpioinitstruct.Pin = INT_PIN;
		gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;
		gpioinitstruct.Pull = GPIO_PULLUP;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(INT_PORT, &gpioinitstruct);

		gpioinitstruct.Pin = FILT_PIN;
		gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
		gpioinitstruct.Pull = GPIO_NOPULL;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(FILT_PORT, &gpioinitstruct);
		FILT_HIGH();

		gpioinitstruct.Pin = REST_PIN;
		HAL_GPIO_Init(REST_PORT, &gpioinitstruct);
		REST_HIGH();

		gpioinitstruct.Pin = GAIN_PIN;
		HAL_GPIO_Init(GAIN_PORT, &gpioinitstruct);
		GAIN_HIGH();
	}
	else if (hVolt.eMode == voltageReceiver)
	{
			AN_CS_CLK_ENABLE();
			gpioinitstruct.Pin = AN_CS_PIN;
			gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
			gpioinitstruct.Pull = GPIO_PULLUP;
			gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(AN_CS_PORT, &gpioinitstruct);

	}

	COMMON_IO_Init();
}
void VoltageSinkDeInit(void)
{
	COMMON_IO_DeInit();
	if (hVolt.eMode == voltageRatio) {
		HAL_NVIC_DisableIRQ(INT_IRQn);
		HAL_GPIO_DeInit(INT_PORT, INT_PIN);
		HAL_GPIO_DeInit(REST_PORT, REST_PIN);
		HAL_GPIO_DeInit(GAIN_PORT, GAIN_PIN);
		HAL_GPIO_DeInit(FILT_PORT, FILT_PIN);
	} else if (hVolt.eMode == voltageReceiver)
	{
		HAL_GPIO_DeInit(AN_CS_PORT, AN_CS_PIN);
	}
}

void VoltageSinkStop(void)
{
	HW_TS_Stop(VoltageSink_timer_Id);
	BSP_LED_Off(LED_BLUE);

//	hVoltage->eMode = Stop;
	bVoltageSinkInit = false;
	VoltageSinkDeInit();
}

void VoltageSinkStart(void)
{

	if (hVoltage == NULL) {
		hVoltage = &hVolt;
	}

	if (!bVoltageSinkInit)
	{
		VoltageSinkInit();

		bVoltageSinkInit = true;
//		hVoltage->eMode = voltageReceiver;

//		HAL_NVIC_SetPriority(INT_IRQn, 0x0F, 0x00);
//		HAL_NVIC_EnableIRQ(INT_IRQn);

		REST_LOW();
		HAL_Delay(10);
		REST_HIGH();
		/* Enable and set Button EXTI Interrupt to the lowest priority */

	}

//	if (hVoltage->eState == Reset && hVoltage->eMode != Stop)
//	{
//		hVoltage->eState = Busy;
//
//		COMMON_IO_Read(&rReg, 3);
////
//		le = (rReg[0]) & 0xff;
//		le |= (rReg[1] & 0xff) << 8;
//		le |= (rReg[2] & 0xff) << 16;
//		hVoltage->iVoltage_Level = le;
//		hVoltage->iVoltage_Value = (uint32_t) ((((float) le / (0XFFFFF >> 1)) * 2.048f)
//				* 100.0f) - 1;
////
//		hVoltage->eState = voltageReset;
//
		HW_TS_Start(VoltageSink_timer_Id,
				(uint32_t) (0.500 * 1000 * 1000 / CFG_TS_TICK_VAL));
////
////		BSP_LED_Toggle(LED_BLUE);
////
////		P2PS_Send_Notification();
////	}
		VoltageSink_IRQHandler();
}

uint8_t rReg[4] = { 0x00 };
void VoltageSink_IRQHandler(void)
{
	int32_t le = 0;

//	HAL_NVIC_DisableIRQ(INT_IRQn);
	if (hVoltage->eMode == voltageRatio)
	{
		COMMON_IO_Read(rReg, 4);
		le = (rReg[2]) & 0xff;
		le |= (rReg[1] & 0xff) << 8;
		le |= (rReg[0] & 0xff) << 16;
		le -= 0x800000;
		//    le -= 0x800000;
		hVoltage->iVoltage_Status = rReg[3];
		hVoltage->iVoltage_Value = ((((float) le / (float) (0X7FFFFF)))
				* 100000000.0f);

	}
	else if (hVoltage->eMode == voltageReceiver)
	{
		AN_CS_LOW();
		COMMON_IO_Read(rReg, 2);
		AN_CS_HIGH();

		le = (rReg[1]) & 0xff;
		le |= (rReg[0] & 0xff) << 8;

		hVoltage->iVoltage_Value = (((((float) le) - 4000) / 226.685f) * 100.0f);

		/* Note: At this step, a voltage can be supplied to ADC channel input     */
		/*       (by connecting an external signal voltage generator to the       */
		/*       analog input pin) to perform a ADC conversion on a determined    */
		/*       voltage level.                                                   */
		/*       Otherwise, ADC channel input can be let floating, in this case   */
		/*       ADC conversion data will be undetermined.                        */

		/*## Enable peripherals ####################################################*/

		/* Start ADC group regular conversion */
		if (HAL_ADC_Start(&hadc1) != HAL_OK) {
			/* ADC conversion start error */
			Error_Handler();
		}

		/* Wait till conversion is done */
		if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
			/* End Of Conversion flag not set on time */
			Error_Handler();
		} else {
			/* Retrieve ADC conversion data */
			uhADCxConvertedData = (HAL_ADC_GetValue(&hadc1) / 2) * 33.3405f;

			/* Computation of ADC conversions raw data to physical values           */
			/* using helper macro.                                                  */
			uhADCxConvertedData_Voltage_mVolt = __ADC_CALC_DATA_VOLTAGE(
					VDDA_APPLI, uhADCxConvertedData);

			//		HAL_Delay(100);

			/* Toggle LED2 as heart beat */
			//		BSP_LED_Toggle(LED2);
		}
	}
//	HAL_NVIC_EnableIRQ(INT_IRQn);
	//


	hVoltage->eState = voltageReset;

	BSP_LED_Toggle(LED_BLUE);
	P2PS_Send_Notification();
}
