
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>

#define APP 1


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED state is off */
	Board_LED_Set(LED3, LED_OFF);
}




#if (APP == 1)		/* Re-writing vPrintString() to use a gatekeeper task */

#define mainSW_INTERRUPT_ID		(0)
/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)
/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);
static void vTask3(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

#define vSoftwareInterruptHandler (DAC_IRQHandler)


xSemaphoreHandle xBinarySemaphore;
xQueueHandle xQueue;


volatile long interrupt=0;


static void vTask1(void *pvParameters)
{
	while (1) {
		/* This task is just used to 'simulate' an interrupt.  This is done by
         * periodically generating a software interrupt. */
        vTaskDelay(500 / portTICK_RATE_MS);

        /* Generate the interrupt, printing a message both before hand and
         * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Task1: Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
//        DEBUGOUT("Periodic task - Interrupt generated.\n\n");
    }
}

/* Take Semaphore, UART (or output) & LED toggle thread */
static void vTask2(void *pvParameters)
{

	long lValueToSend;
	portBASE_TYPE xStatus;
    xSemaphoreTake(xBinarySemaphore, (portTickType) 0);

	while (1) {
		Board_LED_Toggle(LED3);

        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        /* To get here the event must have occurred.  Process the event (in this
         * case we just print out a message). */

        lValueToSend = interrupt;

        DEBUGOUT("Task2: Semaphore taken, sending %d to queue\r\n",lValueToSend);

        xStatus = xQueueSendToBack(xQueue, &lValueToSend, (portTickType)0);

		if (xStatus != pdPASS) {
			DEBUGOUT("Could not send to the queue.\r\n");
		}

    }
}

static void vTask3(void *pvParameters)
{
	/* Declare the variable that will hold the values received from the queue. */
	long lReceivedValue;
	portBASE_TYPE xStatus;

	while (1) {
		Board_LED_Set(LED3, LED_OFF);


		if (uxQueueMessagesWaiting(xQueue) != 0) {
			DEBUGOUT("Queue should have been empty!\r\n");
		}

		xStatus = xQueueReceive(xQueue, &lReceivedValue, portMAX_DELAY);

		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received
			 * value. */
			DEBUGOUT("Task3: Received = %d\r\n", lReceivedValue);
		}
		else {
			DEBUGOUT("Task3: Could not receive from the queue.\r\n");
		}
	}
}



static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' the semaphore to unblock the task. */
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    interrupt++;

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

    vSemaphoreCreateBinary(xBinarySemaphore);
    xQueue = xQueueCreate(5, sizeof(long));


    /* Check the semaphore was created successfully. */
    if (xBinarySemaphore != (xSemaphoreHandle) NULL && xQueue != (xQueueHandle)NULL) {
    	/* Enable the software interrupt and set its priority. */
    	prvSetupSoftwareInterrupt();

        xTaskCreate(vTask3, (char *) "Task3", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY ), (xTaskHandle *) NULL);

        xTaskCreate(vTask2, (char *) "Task2", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY ), (xTaskHandle *) NULL);

        xTaskCreate(vTask1, (char *) "Task1", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}

#endif

