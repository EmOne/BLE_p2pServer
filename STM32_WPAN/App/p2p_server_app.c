/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    p2p_server_app.c
  * @author  MCD Application Team
  * @brief   Peer to peer Server Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "p2p_server_app.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "current_common.h"
#include "voltage_common.h"
#include "vibrate_common.h"
#include "temperature_common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
 typedef struct{
    uint8_t             Device_Led_Selection;
    uint8_t             Led1;
 }P2P_LedCharValue_t;

 typedef struct{
    uint8_t             Device_Button_Selection;
    uint8_t             ButtonStatus;
 }P2P_ButtonCharValue_t;

typedef struct
{
  uint8_t               Notification_Status; /* used to check if P2P Server is enabled to Notify */
  P2P_LedCharValue_t    LedControl;
	P2P_ButtonCharValue_t ButtonControl; //TODO: Apply to Digital sensor
	Current_t CurrentControl;
//	Voltage_t VoltageControl;
//	Temperature_t *TemperatureControl;
//	Vibrate_t VibrationControl;
  uint16_t              ConnectionHandle;
} P2P_Server_App_Context_t;
/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t CurrentSource_Ramp_timer_Id;
uint8_t CurrentSource_Step_timer_Id;
uint8_t CurrentSink_timer_Id;
uint8_t VoltageSink_timer_Id;
uint8_t VibrateSink_timer_Id;
uint8_t TemperatureSink_timer_Id;
Current_t *hCurrent;
Voltage_t *hVoltage;
Vibrate_t *hVibrate;
Temperature_t *hTemperature;
/**
 * START of Section BLE_APP_CONTEXT
 */

P2P_Server_App_Context_t P2P_Server_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void P2PS_Send_Notification(void);
static void P2PS_APP_LED_BUTTON_context_Init(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2PS_STM_App_Notification(P2PS_STM_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_STM_App_Notification_1 */

/* USER CODE END P2PS_STM_App_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_STM_App_Notification_P2P_Evt_Opcode */
#if(BLE_CFG_OTA_REBOOT_CHAR != 0)
    case P2PS_STM_BOOT_REQUEST_EVT:
      APP_DBG_MSG("-- P2P APPLICATION SERVER : BOOT REQUESTED\n");
      APP_DBG_MSG(" \n\r");

      *(uint32_t*)SRAM1_BASE = *(uint32_t*)pNotification->DataTransfered.pPayload;
      NVIC_SystemReset();
      break;
#endif
/* USER CODE END P2PS_STM_App_Notification_P2P_Evt_Opcode */

    case P2PS_STM__NOTIFY_ENABLED_EVT:
/* USER CODE BEGIN P2PS_STM__NOTIFY_ENABLED_EVT */
      P2P_Server_App_Context.Notification_Status = 1;
      APP_DBG_MSG("-- P2P APPLICATION SERVER : NOTIFICATION ENABLED\n"); 
      APP_DBG_MSG(" \n\r");
/* USER CODE END P2PS_STM__NOTIFY_ENABLED_EVT */
      break;

    case P2PS_STM_NOTIFY_DISABLED_EVT:
/* USER CODE BEGIN P2PS_STM_NOTIFY_DISABLED_EVT */
      P2P_Server_App_Context.Notification_Status = 0;
      APP_DBG_MSG("-- P2P APPLICATION SERVER : NOTIFICATION DISABLED\n");
      APP_DBG_MSG(" \n\r");
/* USER CODE END P2PS_STM_NOTIFY_DISABLED_EVT */
      break;

    case P2PS_STM_WRITE_EVT:
/* USER CODE BEGIN P2PS_STM_WRITE_EVT */
		if (pNotification->DataTransfered.pPayload[0] == 0x00)
		{ /* ALL Device selected - may be necessary as LB Routeur informs all connection */

			if (pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER  : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
			else if (pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER  : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#if(P2P_SERVER1 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x01){ /* end device 1 selected - may be necessary as LB Routeur informs all connection */
			if (pNotification->DataTransfered.pPayload[1] == 0x0B)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Temperature sink enable\n");
				APP_DBG_MSG(" \n\r");
				//TODO: NTC/PT100 Init and Start read sensor
				if (pNotification->DataTransfered.pPayload[2] == 0x01)
				{
					APP_DBG_MSG(
							"-- P2P APPLICATION SERVER  : Temperature 2/4 Wire\n");
					APP_DBG_MSG(" \n\r");

					hTemperature->eWire = 0x01;

				}
				else if (pNotification->DataTransfered.pPayload[2] == 0x00)
				{
					APP_DBG_MSG(
							"-- P2P APPLICATION SERVER  : Temperature 3 Wire\n");
					APP_DBG_MSG(" \n\r");
					hTemperature->eWire = 0x00;
				}
				bTemperatureSinkInit = false;
				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_TEMPERATURE_SINK_ON_ID,
						CFG_SCH_PRIO_0);

			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x0A)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Temperature sink disable\n");
				APP_DBG_MSG(" \n\r");
				//TODO: NTC/PT100 Deinit

				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_TEMPERATURE_SINK_OFF_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x09)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Vibrate sink enable\n");
				APP_DBG_MSG(" \n\r");

				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_VIBRATE_SINK_ON_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x08)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Vibrate sink disable\n");
				APP_DBG_MSG(" \n\r");

				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_VIBRATE_SINK_OFF_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x07)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Voltage sink enable\n");
				APP_DBG_MSG(" \n\r");
				if (pNotification->DataTransfered.pPayload[2] == 0x01)
				{
					APP_DBG_MSG("-- P2P APPLICATION SERVER  : Ratio meter\n");
					APP_DBG_MSG(" \n\r");

					hVoltage->eMode = voltageRatio;

				}
				else if (pNotification->DataTransfered.pPayload[2] == 0x00)
				{
					APP_DBG_MSG("-- P2P APPLICATION SERVER  : Voltage meter\n");
					APP_DBG_MSG(" \n\r");
					hVoltage->eMode = voltageReceiver;
				}
				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_VOLTAGE_SINK_ON_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x06)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Voltage sink disable\n");
				APP_DBG_MSG(" \n\r");

				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_VOLTAGE_SINK_OFF_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x05)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Current source enable and configure\n");
				APP_DBG_MSG(" \n\r");
				P2P_Server_App_Context.CurrentControl.eMode = Transmitter;
				if (pNotification->DataTransfered.pPayload[2] == 0x00)
				{
					APP_DBG_MSG(
							"-- P2P APPLICATION SERVER  : Current source mode PCT\n");
					APP_DBG_MSG(" \n\r");
					// Execute current percent process with notify

					uint16_t lvl =
							(uint16_t) (pNotification->DataTransfered.pPayload[3]
									<< 8
									| pNotification->DataTransfered.pPayload[4]);
					P2P_Server_App_Context.CurrentControl.iCurrent_Level = lvl;

					UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_PCT_ID,
							CFG_SCH_PRIO_0);
				}
				else if (pNotification->DataTransfered.pPayload[2] == 0x01)
				{
					APP_DBG_MSG(
							"-- P2P APPLICATION SERVER  : Current source mode RAMP\n");
					APP_DBG_MSG(" \n\r");
					// Execute current step process with notify
					UTIL_SEQ_SetTask(
							1 << CFG_TASK_MODULE_CURRENT_SOURCE_RAMP_ID,
							CFG_SCH_PRIO_0);

				}
				else if (pNotification->DataTransfered.pPayload[2] == 0x02)
				{
					APP_DBG_MSG(
							"-- P2P APPLICATION SERVER  : Current source mode STEP\n");
					APP_DBG_MSG(" \n\r");
					// Execute current ramp process with notify
					UTIL_SEQ_SetTask(
							1 << CFG_TASK_MODULE_CURRENT_SOURCE_STEP_ID,
							CFG_SCH_PRIO_0);
				}
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x04)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Current source stop\n");
				APP_DBG_MSG(" \n\r");
				P2P_Server_App_Context.CurrentControl.eMode = Stop;
				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_OFF_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x03)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Current sink enable\n");
				APP_DBG_MSG(" \n\r");
				P2P_Server_App_Context.CurrentControl.eMode = Receiver;
				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SINK_ON_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x02)
			{
				APP_DBG_MSG(
						"-- P2P APPLICATION SERVER  : Current sink disable\n");
				APP_DBG_MSG(" \n\r");
				P2P_Server_App_Context.CurrentControl.eMode = Stop;
				UTIL_SEQ_SetTask(1 << CFG_TASK_MODULE_CURRENT_SINK_OFF_ID,
						CFG_SCH_PRIO_0);
			}
			else if (pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 1 : LED1 ON\n");
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
			else if (pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 1 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER2 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x02){ /* end device 2 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif      
#if(P2P_SERVER3 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x03){ /* end device 3 selected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 3 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 3 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER4 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x04){ /* end device 4 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 2 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif     
#if(P2P_SERVER5 != 0)  
      if(pNotification->DataTransfered.pPayload[0] == 0x05){ /* end device 5 selected - may be necessary as LB Routeur informs all connection */
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 5 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 5 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }
      }
#endif
#if(P2P_SERVER6 != 0)
      if(pNotification->DataTransfered.pPayload[0] == 0x06){ /* end device 6 selected */ 
        if(pNotification->DataTransfered.pPayload[1] == 0x01)
        {
          BSP_LED_On(LED_BLUE);
           APP_DBG_MSG("-- P2P APPLICATION SERVER 6 : LED1 ON\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x01; /* LED1 ON */
        }
        if(pNotification->DataTransfered.pPayload[1] == 0x00)
        {
          BSP_LED_Off(LED_BLUE);
          APP_DBG_MSG("-- P2P APPLICATION SERVER 6 : LED1 OFF\n"); 
          APP_DBG_MSG(" \n\r");
          P2P_Server_App_Context.LedControl.Led1=0x00; /* LED1 OFF */
        }   
      }
#endif 
/* USER CODE END P2PS_STM_WRITE_EVT */
      break;

    default:
/* USER CODE BEGIN P2PS_STM_App_Notification_default */
      
/* USER CODE END P2PS_STM_App_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_STM_App_Notification_2 */

/* USER CODE END P2PS_STM_App_Notification_2 */
  return;
}

void P2PS_APP_Notification(P2PS_APP_ConnHandle_Not_evt_t *pNotification)
{
/* USER CODE BEGIN P2PS_APP_Notification_1 */

/* USER CODE END P2PS_APP_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2PS_APP_Notification_P2P_Evt_Opcode */

/* USER CODE END P2PS_APP_Notification_P2P_Evt_Opcode */
  case PEER_CONN_HANDLE_EVT :
/* USER CODE BEGIN PEER_CONN_HANDLE_EVT */
          
/* USER CODE END PEER_CONN_HANDLE_EVT */
    break;

    case PEER_DISCON_HANDLE_EVT :
/* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
       P2PS_APP_LED_BUTTON_context_Init();       
/* USER CODE END PEER_DISCON_HANDLE_EVT */
    break;

    default:
/* USER CODE BEGIN P2PS_APP_Notification_default */

/* USER CODE END P2PS_APP_Notification_default */
      break;
  }
/* USER CODE BEGIN P2PS_APP_Notification_2 */

/* USER CODE END P2PS_APP_Notification_2 */
  return;
}

void P2PS_APP_Init(void)
{
/* USER CODE BEGIN P2PS_APP_Init */
	if (hCurrent == NULL)
		hCurrent = &P2P_Server_App_Context.CurrentControl;

	if (hVoltage == NULL)
		hVoltage = &hVolt;

	if (hVibrate == NULL)
		hVibrate = &hVib;

	if (hTemperature == NULL)
		hTemperature = &hTemp;

	P2P_Server_App_Context.CurrentControl.eState = Reset;

	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SINK_OFF_ID, UTIL_SEQ_RFU,
			CurrentSinkStop);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SINK_ON_ID, UTIL_SEQ_RFU,
			CurrentSinkStart);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_PCT_ID, UTIL_SEQ_RFU,
			CurrentSourcePCT);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_RAMP_ID, UTIL_SEQ_RFU,
			CurrentSourceRamp);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_STEP_ID, UTIL_SEQ_RFU,
			CurrentSourceStep);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_CURRENT_SOURCE_OFF_ID, UTIL_SEQ_RFU,
			CurrentSourceStop);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_VOLTAGE_SINK_OFF_ID, UTIL_SEQ_RFU,
			VoltageSinkStop);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_VOLTAGE_SINK_ON_ID, UTIL_SEQ_RFU,
			VoltageSinkStart);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_VIBRATE_SINK_OFF_ID, UTIL_SEQ_RFU,
			VibrateSinkStop);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_VIBRATE_SINK_ON_ID, UTIL_SEQ_RFU,
			VibrateSinkStart);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_TEMPERATURE_SINK_OFF_ID, UTIL_SEQ_RFU,
			TemperatureSinkStop);
	UTIL_SEQ_RegTask(1 << CFG_TASK_MODULE_TEMPERATURE_SINK_ON_ID, UTIL_SEQ_RFU,
			TemperatureSinkStart);
	UTIL_SEQ_RegTask(1 << CFG_TASK_SW1_BUTTON_PUSHED_ID, UTIL_SEQ_RFU,
			P2PS_Send_Notification);

	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(CurrentSource_Step_timer_Id),
			hw_ts_SingleShot, CurrentSourceStep);
	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(CurrentSource_Ramp_timer_Id),
			hw_ts_SingleShot, CurrentSourceRamp);
	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(CurrentSink_timer_Id), hw_ts_SingleShot,
			CurrentSinkStart);
	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(VibrateSink_timer_Id), hw_ts_SingleShot,
			VibrateSinkStart);
	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(VoltageSink_timer_Id), hw_ts_SingleShot,
			VoltageSinkStart);
	HW_TS_Create(CFG_TIM_PROC_ID_ISR, &(TemperatureSink_timer_Id),
			hw_ts_SingleShot,
			TemperatureSinkStart);
  /**
   * Initialize LedButton Service
   */
  P2P_Server_App_Context.Notification_Status=0; 
  P2PS_APP_LED_BUTTON_context_Init();
/* USER CODE END P2PS_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void P2PS_APP_LED_BUTTON_context_Init(void){
  
  BSP_LED_Off(LED_BLUE);
  APP_DBG_MSG("LED BLUE OFF\n");
  
  #if(P2P_SERVER1 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x01; /* Device1 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x01;/* Device1 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
	P2P_Server_App_Context.CurrentControl.Step_Dir = UP;
	P2P_Server_App_Context.CurrentControl.eMode = Stop;
	P2P_Server_App_Context.CurrentControl.eState = Reset;
	P2P_Server_App_Context.CurrentControl.iCurrent_Level = 0x00;
	P2P_Server_App_Context.CurrentControl.iCurrent_Value = 0x00;
#endif
#if(P2P_SERVER2 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x02; /* Device2 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x02;/* Device2 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
#if(P2P_SERVER3 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x03; /* Device3 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x03; /* Device3 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif
#if(P2P_SERVER4 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x04; /* Device4 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x04; /* Device4 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
 #if(P2P_SERVER5 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x05; /* Device5 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x05; /* Device5 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif
#if(P2P_SERVER6 != 0)
  P2P_Server_App_Context.LedControl.Device_Led_Selection=0x06; /* device6 */
  P2P_Server_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Server_App_Context.ButtonControl.Device_Button_Selection=0x06; /* Device6 */
  P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
#endif  
}

void P2PS_APP_SW1_Button_Action(void)
{
  UTIL_SEQ_SetTask( 1<<CFG_TASK_SW1_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);

  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
void P2PS_Send_Notification(void)
{
	uint8_t data[16] =
	{ };
	uint8_t len = 0;
  if(P2P_Server_App_Context.ButtonControl.ButtonStatus == 0x00){
    P2P_Server_App_Context.ButtonControl.ButtonStatus=0x01;
  } else {
    P2P_Server_App_Context.ButtonControl.ButtonStatus=0x00;
  }

	memcpy(&data[0], &P2P_Server_App_Context.ButtonControl,
			sizeof(P2P_ButtonCharValue_t));
	if (bCurrentSinkInit || bCurrentSourceStepInit || bCurrentSourceRampInit)
	{
		memcpy(&data[sizeof(P2P_ButtonCharValue_t)],
				&P2P_Server_App_Context.CurrentControl, sizeof(Current_t));
		len = sizeof(P2P_ButtonCharValue_t) + sizeof(Current_t);
	}
	else if (bTemperatureSinkInit)
	{
		memcpy(&data[sizeof(P2P_ButtonCharValue_t)],
				hTemperature,
				sizeof(Temperature_t));
		len = sizeof(P2P_ButtonCharValue_t) + sizeof(Temperature_t);
	}
	else if (bVoltageSinkInit)
	{
		memcpy(&data[sizeof(P2P_ButtonCharValue_t)],
				hVoltage,
				sizeof(Voltage_t));
		len = sizeof(P2P_ButtonCharValue_t) + sizeof(Voltage_t);
	}
	else if (bVibrateSinkInit)
	{
		memcpy(&data[sizeof(P2P_ButtonCharValue_t)],
				hVibrate,
				sizeof(Vibrate_t));
		len = sizeof(P2P_ButtonCharValue_t) + sizeof(Vibrate_t);
	}
	else
	{
		len = sizeof(P2P_ButtonCharValue_t);
	}

	if (P2P_Server_App_Context.Notification_Status)
	{ 
    APP_DBG_MSG("-- P2P APPLICATION SERVER  : INFORM CLIENT BUTTON 1 PUSHED \n ");
    APP_DBG_MSG(" \n\r");
		P2PS_STM_App_Update_Char(P2P_NOTIFY_CHAR_UUID,
				(uint8_t*) &data,
				len
				);
   } else {
    APP_DBG_MSG("-- P2P APPLICATION SERVER : CAN'T INFORM CLIENT -  NOTIFICATION DISABLED\n "); 
   }

  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
