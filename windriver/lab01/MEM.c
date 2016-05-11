/* MEM.c - Pilsan Timer Example Program */

/*
DESCRIPTION
This is a file containing timer and interrupt for use with RTOS
*/

#include "hwFunc.h"
#include "disp.h"
#include "FHV.h"
#include "hw.h"

int globalPoti;
int globalTemp;

SEM_ID analogInputs = NULL;

void my_ISR (void)
{
	/* Add your code here! */
	//printf(".");

	semGive (analogInputs);
	
	/* Clear AIO interrupts by writing anything to a specified address */
	sysOutByte (aioBase + intClearAddress, 0);
}

int readInputs (void)
{
	unsigned char tempOut;
	/* Connect interrupt service routine to vector and all stuff */
	intConnect (INUM_TO_IVEC(aioIntNum), my_ISR, aioIntNum);
	sysIntEnablePIC (aioIRQNum);
	/* Enable interrupts on the aio:
	* All interrupts and interrupt from counter 1 too */
	tempOut = 0x24;
	sysOutByte (aioBase + intEnAddress, tempOut);
	/* Start counter 1 as timer with 50 ms period 
	* It has a clock input of 1 MHz = 1 µs 
	* Therefore the load value is 0xC350 = 50000 */
	tempOut = 0x74;
	sysOutByte (aioBase + cntCntrlReg, tempOut);
	tempOut = 0x50;
	sysOutByte (aioBase + cnt1Address, tempOut);
	tempOut = 0xC3;
	sysOutByte (aioBase + cnt1Address, tempOut);
	
	int poti;
	int temp;
	int hyst = 15;
	
	while(1){
		semTake (analogInputs, WAIT_FOREVER);
		poti =  readAnalog (2, 0);
		temp =  readAnalog (5, 2);
		//printf ("Werte %d %d \n", poti, temp);
		if  ((poti > globalPoti+hyst) | (poti < globalPoti-hyst))
		{
			globalPoti = poti;
		}
		if  ((temp > globalTemp+hyst) | (temp < globalTemp-hyst))
		{
			globalTemp = temp;
		}
	}
}

int showInputs (void)
{
	char output[32];
	while(1){
		
		sprintf(output,"Temperature = %d     ", globalTemp);
		writeToDisplay (3, 2, output );
		
		sprintf(output,"Poti = %x     ", globalPoti);
		writeToDisplay (4, 2, output );
		
		beLazy (100);
		

	}
	
}

void main (void)
{
	

	/* Add your code here: create tasks, semaphores, ... */
	initHardware(0);
	
	analogInputs = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

	int readInputsID;
	readInputsID = taskSpawn ("readInputs", 150, 0,0x1000,(FUNCPTR) readInputs,0,0,0,0,0,0,0,0,0,0);
	
	int showInputsID;
	showInputsID = taskSpawn ("showInputs", 160, 0,0x1000,(FUNCPTR) showInputs,0,0,0,0,0,0,0,0,0,0);

	printf("Hello World");

	/* Suspend own task */
	taskSuspend (0);
} /* main */
