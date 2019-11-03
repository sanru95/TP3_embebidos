
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define APP (3)


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED state is off */
	Board_LED_Set(LED3, LED_OFF);
}




#if (APP == 1)

#define mainSW_INTERRUPT_ID		(0)
/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)
/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

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

	DEBUGOUT("Init application 1\r\n");

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

#if (APP == 2)

#define mainSW_INTERRUPT_ID		(0)
/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)
/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

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

        interrupt ++;

        /* Generate the interrupt, printing a message both before hand and
         * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Task1: Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
//        DEBUGOUT("Periodic task - Interrupt generated.\n\n");
    }
}



static void vTask2(void *pvParameters)
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
			DEBUGOUT("Task2: Received = %d\r\n", lReceivedValue);
		}
		else {
			DEBUGOUT("Task2: Could not receive from the queue.\r\n");
		}
		xSemaphoreGive(xBinarySemaphore);
	}
}

static void vTask3(void *pvParameters)
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

        DEBUGOUT("Task3: Semaphore taken\r\n");


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

	long var = interrupt;

	xQueueSendToBackFromISR(xQueue, &var, &xHigherPriorityTaskWoken);

    mainCLEAR_INTERRUPT();

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	DEBUGOUT("Init application 2\r\n");

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



#if (APP == 3)

const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\r\n";
const char *pcTextForTask3 = "Task 3 is running\r\n";

xSemaphoreHandle xMutex;

/* The task function. */
static void vTaskFunction(void *pvParameters);


/* UART (or output) & LED toggle thread */
static void vTaskFunction(void *pvParameters) {
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	 * character pointer. */
	pcTaskName = (char *) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		/* Print out the name of this task. */

		xSemaphoreTake(xMutex, portMAX_DELAY);

		DEBUGOUT(pcTaskName);

		DEBUGOUT("LED ON\r\n");
		Board_LED_Set(LED3, LED_ON);

		/* Delay for a period.  This time we use a call to vTaskDelay() which
		 * puts the task into the Blocked state until the delay period has expired.
		 * The delay period is specified in 'ticks'. */
		vTaskDelay(500 / portTICK_RATE_MS);

		Board_LED_Set(LED3, LED_OFF);
		DEBUGOUT("LED OFF\r\n");

		vTaskDelay(500 / portTICK_RATE_MS);


		xSemaphoreGive(xMutex);
		taskYIELD();
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 4 - Using the Blocked state to create delay
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	DEBUGOUT("Init application 3\r\n");

	xMutex = xSemaphoreCreateMutex();

	xTaskCreate(vTaskFunction, (char *) "Task1", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask1, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	xTaskCreate(vTaskFunction, (char *) "Task2", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask2, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	xTaskCreate(vTaskFunction, (char *) "Task3", configMINIMAL_STACK_SIZE,
				(void *) pcTextForTask3, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	 * running.  If we do reach here then it is likely that there was insufficient
	 * heap available for the idle task to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif
