/**
 ******************************************************************************
 *
 * @file       openpilot.c 
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Sets up and runs main OpenPilot tasks.
 * @see        The GNU Public License (GPL) Version 3
 * 
 *****************************************************************************/
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


/* OpenPilot Includes */
#include "openpilot.h"
#include "uavobjectsinit.h"
#include "telemetry.h"
#include "GPS.h"
#include "systemmod.h"


/* Task Priorities */
#define PRIORITY_TASK_HOOKS             (tskIDLE_PRIORITY + 3)

/* Global Variables */

/* Local Variables */
#define INCLUDE_TEST_TASKS 0
#if INCLUDE_TEST_TASKS
static uint8_t sdcard_available;
#endif
FILEINFO File;
char Buffer[1024];
uint32_t Cache;

/* Function Prototypes */
#if INCLUDE_TEST_TASKS
static void TaskTick(void *pvParameters);
static void TaskTesting(void *pvParameters);
static void TaskHIDTest(void *pvParameters);
static void TaskServos(void *pvParameters);
static void TaskSDCard(void *pvParameters);
#endif
int32_t CONSOLE_Parse(COMPortTypeDef port, char c);
void OP_ADC_NotifyChange(uint32_t pin, uint32_t pin_value);

/* Prototype of generated InitModules() function */
extern void InitModules(void);

/**
* OpenPilot Main function
*/
int main()
{
	/* NOTE: Do NOT modify the following start-up sequence */
	/* Any new initialization functions should be added in OpenPilotInit() */

	/* Brings up System using CMSIS functions, enables the LEDs. */
	PIOS_SYS_Init();

	/* Initialize the system thread */
	SystemModInitialize();

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be running. */
	/* If we do get here, it will most likely be because we ran out of heap space. */
	PIOS_LED_Off(LED1);
	PIOS_LED_Off(LED2);
	for(;;) {
		PIOS_LED_Toggle(LED1);
		PIOS_LED_Toggle(LED2);
		PIOS_DELAY_WaitmS(100);
	}

	return 0;
}

/**
 * Initialize the hardware, libraries and modules (called by the System thread in systemmod.c)
 */
void OpenPilotInit()
{
	/* Delay system */
	PIOS_DELAY_Init();

	/* SPI Init */
	PIOS_SPI_Init();

	/* Enable and mount the SDCard */
	PIOS_SDCARD_Init();
	PIOS_SDCARD_MountFS(0);

	/* Initialize UAVObject libraries */
	EventDispatcherInitialize();
	UAVObjInitialize();
	UAVObjectsInitializeAll();

	/* Initialize the alarms library */
	AlarmsInitialize();

	/* Initialize the PiOS library */
	PIOS_COM_Init();
	PIOS_Servo_Init();
	PIOS_ADC_Init();
	PIOS_GPIO_Init();
#if defined(PIOS_INCLUDE_SPEKTRUM)
	PIOS_SPEKTRUM_Init();
#endif
#if defined(PIOS_INCLUDE_PWM)
	PIOS_PWM_Init();
#endif
#if defined(PIOS_INCLUDE_PPM)
	PIOS_PPM_Init();
#endif
	PIOS_USB_Init(0);
	PIOS_I2C_Init();

	/* Initialize modules */
	InitModules();

	/* Create test tasks */
	//xTaskCreate(TaskTesting, (signed portCHAR *)"Testing", configMINIMAL_STACK_SIZE , NULL, 4, NULL);
	//xTaskCreate(TaskHIDTest, (signed portCHAR *)"HIDTest", configMINIMAL_STACK_SIZE , NULL, 3, NULL);
	//xTaskCreate(TaskServos, (signed portCHAR *)"Servos", configMINIMAL_STACK_SIZE , NULL, 3, NULL);
	//xTaskCreate(TaskSDCard, (signed portCHAR *)"SDCard", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2), NULL);
}

#if INCLUDE_TEST_TASKS
static void TaskTesting(void *pvParameters)
{
	portTickType xDelay = 250 / portTICK_RATE_MS;
	portTickType xTimeout = 10 / portTICK_RATE_MS;

	//PIOS_BMP085_Init();

	for(;;)
	{
		/* This blocks the task until the BMP085 EOC */
		/*
		PIOS_BMP085_StartADC(TemperatureConv);
		xSemaphoreTake(PIOS_BMP085_EOC, xTimeout);
		PIOS_BMP085_ReadADC();
		PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "%u\r", PIOS_BMP085_GetTemperature());

		PIOS_BMP085_StartADC(PressureConv);
		xSemaphoreTake(PIOS_BMP085_EOC, xTimeout);
		PIOS_BMP085_ReadADC();
		PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "%u\r", PIOS_BMP085_GetPressure());
		*/

#if defined(PIOS_INCLUDE_SPEKTRUM)
		PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "%u,%u,%u,%u,%u,%u,%u,%u\r", PIOS_SPEKTRUM_Get(0), PIOS_SPEKTRUM_Get(1), PIOS_SPEKTRUM_Get(2), PIOS_SPEKTRUM_Get(3), PIOS_SPEKTRUM_Get(4), PIOS_SPEKTRUM_Get(5), PIOS_SPEKTRUM_Get(6), PIOS_SPEKTRUM_Get(7));
#endif
#if defined(PIOS_INCLUDE_PWM)
		PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "%u,%u,%u,%u,%u,%u,%u,%u uS\r", PIOS_PWM_Get(0), PIOS_PWM_Get(1), PIOS_PWM_Get(2), PIOS_PWM_Get(3), PIOS_PWM_Get(4), PIOS_PWM_Get(5), PIOS_PWM_Get(6), PIOS_PWM_Get(7));
#endif
#if defined(PIOS_INCLUDE_PPM)
		PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "%u,%u,%u,%u,%u,%u,%u,%u uS\r", PIOS_PPM_Get(0), PIOS_PPM_Get(1), PIOS_PPM_Get(2), PIOS_PPM_Get(3), PIOS_PPM_Get(4), PIOS_PPM_Get(5), PIOS_PPM_Get(6), PIOS_PPM_Get(7));
#endif

		/* This blocks the task until there is something on the buffer */
		/*xSemaphoreTake(PIOS_USART1_Buffer, portMAX_DELAY);
		int32_t len = PIOS_COM_ReceiveBufferUsed(COM_USART1);
		for(int32_t i = 0; i < len; i++) {
			PIOS_COM_SendFormattedString(COM_DEBUG_USART, ">%c\r", PIOS_COM_ReceiveBuffer(COM_USART1));
		}*/

		//int32_t state = PIOS_USB_CableConnected();
		//PIOS_COM_SendFormattedStringNonBlocking(COM_DEBUG_USART, "State: %d\r", state);

		//PIOS_I2C_Transfer(I2C_Write_WithoutStop, 0x57, (uint8_t *)50, 1);

		/* Test ADC pins */
		//temp = ((1.43 - ((Vsense / 4096) * 3.3)) / 4.3) + 25;
		//uint32_t vsense = PIOS_ADC_PinGet(0);
		//uint32_t Temp = (1.42 -  vsense * 3.3 / 4096) * 1000 / 4.35 + 25;
		//PIOS_COM_SendFormattedString(COM_DEBUG_USART, "Temp: %d, CS_I: %d, CS_V: %d, 5v: %d\r", PIOS_ADC_PinGet(0), PIOS_ADC_PinGet(1), PIOS_ADC_PinGet(2), PIOS_ADC_PinGet(3));
		//PIOS_COM_SendFormattedString(COM_DEBUG_USART, "AUX1: %d, AUX2: %d, AUX3: %d\r", PIOS_ADC_PinGet(4), PIOS_ADC_PinGet(5), PIOS_ADC_PinGet(6));

		vTaskDelay(xDelay);
	}
}
#endif

#if INCLUDE_TEST_TASKS
static void TaskHIDTest(void *pvParameters)
{
	uint8_t byte;
	uint8_t line_buffer[128];
	uint16_t line_ix = 0;

	for(;;)
	{
		/* HID Loopback Test */
#if 0
		if(PIOS_COM_ReceiveBufferUsed(COM_USB_HID) != 0) {
			byte = PIOS_COM_ReceiveBuffer(COM_USB_HID);
			if(byte == '\r' || byte == '\n' || byte == 0) {
				PIOS_COM_SendFormattedString(COM_USB_HID, "RX: %s\r", line_buffer);
				PIOS_COM_SendFormattedString(COM_DEBUG_USART, "RX: %s\r", line_buffer);
				line_ix = 0;
			} else if(line_ix < sizeof(line_buffer)) {
				line_buffer[line_ix++] = byte;
				line_buffer[line_ix] = 0;
			}
		}
#endif

		/* HID Loopback Test */
		if(PIOS_COM_ReceiveBufferUsed(COM_USART2) != 0) {
			byte = PIOS_COM_ReceiveBuffer(COM_USART2);
#if 0
			if(byte == '\r' || byte == '\n' || byte == 0) {
				PIOS_COM_SendFormattedString(COM_DEBUG_USART, "RX: %s\r", line_buffer);
				line_ix = 0;
			} else if(line_ix < sizeof(line_buffer)) {
				line_buffer[line_ix++] = byte;
				line_buffer[line_ix] = 0;
			}
#endif
			PIOS_COM_SendChar(COM_DEBUG_USART, (char)byte);
		}
	}
}
#endif

#if INCLUDE_TEST_TASKS
static void TaskServos(void *pvParameters)
{
	/* For testing servo outputs */
	portTickType xDelay;

	/* Used to test servos, cycles all servos from one side to the other */
	for(;;) {
		/*xDelay = 250 / portTICK_RATE_MS;
		PIOS_Servo_Set(1, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(2, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(3, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(4, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(5, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(6, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(7, 2000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(8, 2000);
		vTaskDelay(xDelay);

		PIOS_Servo_Set(8, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(7, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(6, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(5, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(4, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(3, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(2, 1000);
		vTaskDelay(xDelay);
		PIOS_Servo_Set(1, 1000);
		vTaskDelay(xDelay);*/

		xDelay = 1 / portTICK_RATE_MS;
		for(int i = 1000; i < 2000; i++) {
			PIOS_Servo_Set(1, i);
			PIOS_Servo_Set(2, i);
			PIOS_Servo_Set(3, i);
			PIOS_Servo_Set(4, i);
			PIOS_Servo_Set(5, i);
			PIOS_Servo_Set(6, i);
			PIOS_Servo_Set(7, i);
			PIOS_Servo_Set(8, i);
			vTaskDelay(xDelay);
		}
		for(int i = 2000; i > 1000; i--) {
			PIOS_Servo_Set(1, i);
			PIOS_Servo_Set(2, i);
			PIOS_Servo_Set(3, i);
			PIOS_Servo_Set(4, i);
			PIOS_Servo_Set(5, i);
			PIOS_Servo_Set(6, i);
			PIOS_Servo_Set(7, i);
			PIOS_Servo_Set(8, i);
			vTaskDelay(xDelay);
		}
	}
}
#endif

#if INCLUDE_TEST_TASKS
static void TaskSDCard(void *pvParameters)
{
	uint16_t second_delay_ctr = 0;
	portTickType xLastExecutionTime;

	/* Initialise the xLastExecutionTime variable on task entry */
	xLastExecutionTime = xTaskGetTickCount();

	for(;;) {
		vTaskDelayUntil(&xLastExecutionTime, 1 / portTICK_RATE_MS);

		/* Each second: */
		/* Check if SD card is available */
		/* High-speed access if SD card was previously available */
		if(++second_delay_ctr >= 1000) {
			second_delay_ctr = 0;

			uint8_t prev_sdcard_available = sdcard_available;
			sdcard_available = PIOS_SDCARD_CheckAvailable(prev_sdcard_available);

			if(sdcard_available && !prev_sdcard_available) {
				/* SD Card has been connected! */
				/* Switch to mass storage device */
				MSD_Init(0);
			} else if(!sdcard_available && prev_sdcard_available) {
				/* Re-init USB for HID */
				PIOS_USB_Init(1);
				/* SD Card disconnected! */
			}
		}

		/* Each millisecond: */
		/* Handle USB access if device is available */
		if(sdcard_available) {
			MSD_Periodic_mS();
		}
	}
}
#endif

