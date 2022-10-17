/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"
/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

#define ButtonsMSG_LEN  6
#define MaxMSG_LEN      20

#define B1_RisingFlag_Position   (1<<0)
#define B1_FallingFlag_Position  (1<<2)
#define B2_RisingFlag_Position   (1<<2)
#define B2_FallingFlag_Position  (1<<3)

#define B1_Rising_msg  "B1_RI"
#define B1_Falling_msg "B1_FL"
#define B2_Rising_msg  "B2_RI"
#define B2_Falling_msg "B1_FL"

#define msgQueLength 10 
#define xTicksToWaitQueTxRx  (TickType_t )10
/*-----------------------------------------------------------*/

TaskHandle_t t1;
TaskHandle_t t2;
TaskHandle_t t3; 
TaskHandle_t t4;
TaskHandle_t t5;
TaskHandle_t t6;

EventGroupHandle_t EventGrHandle;
QueueHandle_t msgQueueHandle;

unsigned long cpuIdleTme = 0;
volatile unsigned int hperPeriodTick =100 ;
float cpuLoad =0;
/*-----------------------------------------------------------*/


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */

static void prvSetupHardware( void );
static void B1_monitorTask( void );
static void B2_monitorTask( void );
static void PerTxUART( void );
static void RxUART( void );
static void Load_1_Simulation( void );
static void Load_2_Simulation( void );
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */

int main( void )
{
	  /* Setup the hardware for use with the Keil demo board. */
		prvSetupHardware();
	  xTaskPeriodicCreate( (TaskFunction_t) B1_monitorTask,
																		"B1_Mntr",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t1, 
																		50);
	  xTaskPeriodicCreate( (TaskFunction_t) B2_monitorTask,
																		"B2_Mntr",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t2, 
																		50);
		xTaskPeriodicCreate( (TaskFunction_t) PerTxUART,
																		"PerTx",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t3, 
																		100);
		xTaskPeriodicCreate( (TaskFunction_t) RxUART,
																		"Uart_Rx",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t4, 
																		20);
	  
		xTaskPeriodicCreate( (TaskFunction_t) Load_2_Simulation,
																		"Ld2_Sim",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t6, 
																		100);																
		xTaskPeriodicCreate(  (TaskFunction_t)Load_1_Simulation,
																		"Ld1_Sim",
																		configMINIMAL_STACK_SIZE,
																		(void*) 1,
																		1,
																		&t5, 
																		10);
	 
		
																		
		vTaskSetApplicationTaskTag(
														t1,
														(TaskHookFunction_t) 1);
		vTaskSetApplicationTaskTag(
														t2,
														(TaskHookFunction_t) 2);
		vTaskSetApplicationTaskTag(
														t3,
														(TaskHookFunction_t) 3); 
		vTaskSetApplicationTaskTag(
														t4,
														(TaskHookFunction_t) 4);
	  vTaskSetApplicationTaskTag(
														t5,
														(TaskHookFunction_t) 5);
	  vTaskSetApplicationTaskTag(
														t6,
														(TaskHookFunction_t) 6);
											
																															
	
   	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
		
		/*crate an eventGroup*/
		EventGrHandle  = xEventGroupCreate();
		/* Create a queue capable of containing 10 pointers of strings.*/
		msgQueueHandle = xQueueCreate( ( UBaseType_t )msgQueLength, ( UBaseType_t )sizeof( char * ) );
		/*reset the timer to start counting*/
		timer1Reset();
		/*Start...........*/
		vTaskStartScheduler();
		/* Should never reach here!  If you do then there was not enough heap
		available for the idle task to be created. */
		for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
  T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}
/* function to save timer busy time and reset it*/
void saveCPU_Time(void)
{
	cpuIdleTme += T1TC ;
	timer1Reset();
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/
/*Hyper period 100 ms*/
/*About 14.35 us execution time*/
static void B1_monitorTask( void )
{
		TickType_t xLastWakeTime;
		uint8_t b1State = 0;
		while (1)
		{
			/*save the time where the task started execution*/
			xLastWakeTime = xTaskGetTickCount ();
			if     (b1State==0 && GPIO_read(PORT_0 , PIN0) == PIN_IS_HIGH)
			{
				/*The new state is high and a rising edge detected*/
				b1State = 1;
				/*Setting the risingEdge flag of B1*/
				xEventGroupSetBits  ( EventGrHandle ,B1_RisingFlag_Position );
				/*Clearing the fallingEdge flag of B1*/
				xEventGroupClearBits( EventGrHandle ,B1_FallingFlag_Position );
			}
			else if (b1State==1 && GPIO_read(PORT_0 , PIN0) == PIN_IS_LOW)
			{
				/*The new state is low and a failling edge detected*/
				b1State = 0;
				/*Setting the fallingEdge flag of B1*/
				xEventGroupSetBits  ( EventGrHandle ,B1_FallingFlag_Position );
				/*Clearing the risingEdge flag of B1*/
				xEventGroupClearBits( EventGrHandle ,B1_RisingFlag_Position );
			}
			/*Else, no change happened, task finished*/
			xTaskDelayUntil( &xLastWakeTime, 50 );
		}
}
/*About 14.8 us execution time*/
static void B2_monitorTask( void )
{
	uint8_t b2State = 0;
	TickType_t xLastWakeTime;
	while(1)
	{
		/*save the time where the task started execution*/
		xLastWakeTime = xTaskGetTickCount ();
		if     (b2State==0 && GPIO_read(PORT_0 , PIN1) == PIN_IS_HIGH)
		{
			/*The new state is high and a rising edge detected*/
			b2State = 1;
			/*Setting the risingEdge flag of B2*/
			xEventGroupSetBits  ( EventGrHandle ,B2_RisingFlag_Position );
			/*Clearing the fallingEdge flag of B2*/
			xEventGroupClearBits( EventGrHandle ,B2_FallingFlag_Position );
		}
		else if (b2State==1 && GPIO_read(PORT_0 , PIN1) == PIN_IS_LOW)
		{
			/*The new state is low and a failling edge detected*/
			b2State = 0;
			/*Setting the fallingEdge flag of B2*/
			xEventGroupSetBits  ( EventGrHandle ,B2_FallingFlag_Position );
			/*Clearing the risingEdge flag of B1*/
			xEventGroupClearBits( EventGrHandle ,B2_RisingFlag_Position );
		}
		/*Else, no change happened*/
		xTaskDelayUntil( &xLastWakeTime, 50 );
	}
}
/*About 19.58 us execution time*/
static void PerTxUART( void )
{
	TickType_t xLastWakeTime;
	char myMsg[] = "MyMsgStrToConsumer\r\n";
	while (1)
	{
		/*save the time where the task started execution*/
		xLastWakeTime = xTaskGetTickCount ();

		/*OverWrite to the back of the queue */
		xQueueSendToBack( msgQueueHandle,&myMsg,xTicksToWaitQueTxRx );
	
		xTaskDelayUntil( &xLastWakeTime, 100 );
	}
}
/*About 22 us execution time*/
static void RxUART( void )
{
	TickType_t xLastWakeTime;
	uint16_t i;
	EventBits_t flagsBuff;
	char  strbuff[MaxMSG_LEN];
	/*This is the consumer task which will write on UART any received string from other tasks*/
	while (1)
	{
		i=0;
		/*save the time where the task started execution*/
		xLastWakeTime = xTaskGetTickCount();
		/*Chech the que of massages*/
		if (uxQueueSpacesAvailable( msgQueueHandle  )< (UBaseType_t )msgQueLength)
		{
			/*Recieve the string pointer*/
			xQueueReceive(msgQueueHandle, &strbuff,xTicksToWaitQueTxRx);
			do
			{
				//I don't know string length
				xSerialPutChar(strbuff[i++]);
			}while(strbuff[i]!= '\0');
		}
		/*Read buttons flags*/
		flagsBuff = xEventGroupGetBits( EventGrHandle );
		if (flagsBuff & B1_RisingFlag_Position)  vSerialPutString(( signed char * )B1_Rising_msg, ButtonsMSG_LEN);
		if (flagsBuff & B1_FallingFlag_Position) vSerialPutString(( signed char * )B1_Falling_msg, ButtonsMSG_LEN);
		if (flagsBuff & B2_RisingFlag_Position)  vSerialPutString(( signed char * )B2_Rising_msg, ButtonsMSG_LEN);
		if (flagsBuff & B2_FallingFlag_Position) vSerialPutString(( signed char * )B2_Falling_msg, ButtonsMSG_LEN);
		xTaskDelayUntil( &xLastWakeTime, 20 );
	}
}
static void Load_1_Simulation( void )
{
	TickType_t xLastWakeTime;
	uint16_t i;
	while (1)
	{
		/*save the time where the task started execution*/
		xLastWakeTime = xTaskGetTickCount ();
		/*Execution time: 5ms*/
		for(i=0;i<0xEF90>>1;)
		{
			i++;
		}
		xTaskDelayUntil( &xLastWakeTime, 10 );
	}
}
static void Load_2_Simulation( void )
{
	TickType_t xLastWakeTime;
	uint16_t i;
	while (1)
	{
		/*save the time where the task started execution*/
		xLastWakeTime = xTaskGetTickCount ();
		/*Execution time: 12ms*/
		for(i=0;i<0xFFFF;)
		{
			i++;
		}
		for(i=0;i<0x1F50;)
		{
			i++;
		}
		xTaskDelayUntil( &xLastWakeTime, 100 );
	}
}


void vApplicationTickHook(void)
 {
	 GPIO_write( PORT_1, PIN0, (pinState_t)(1^GPIO_read(PORT_1 , PIN0 )));
	/* in hyper period tick, stop here to see the counter value*/
	if (xTaskGetTickCount ()%hperPeriodTick == 0 )
	{
		/*every one os tick is 1 ms and timer tick is 1/50 ms*/
		cpuIdleTme /=50.0;
		cpuLoad = (float)(hperPeriodTick-cpuIdleTme)/hperPeriodTick;
		cpuIdleTme	=0;
  	timer1Reset();
	}
 }
