/* MEM.c - Pilsan Timer Example Program */

/*
DESCRIPTION
This is a file containing timer and interrupt for use with RTOS
*/

#include "hwFunc.h"
#include "disp.h"
#include "FHV.h"
#include "hw.h"
#include "time.h"
#include "stdlib.h"

int globalPoti;
int globalTemp;
char globalKey;

SEM_ID analogInputs = NULL;
SEM_ID writeDisplay = NULL;

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
	char output[64];
	float potiVolt;
	float tempVolt;
	while(1){
		semTake (writeDisplay, WAIT_FOREVER);
		
		sprintf(output,"GlobalKey = %c          ", globalKey);
		writeToDisplay (3, 2, output );
		
		switch(globalKey){
		case '1': writeToDisplay (8, 2, "                         ");
			sprintf(output,"Temperature = %d          ", globalTemp);
			writeToDisplay (5, 2, output );
			sprintf(output,"Poti = %d          ", globalPoti);
			writeToDisplay (6, 2, output );
			break;
		case '2': writeToDisplay (8, 2, "                    ");
			potiVolt=(float)(globalPoti*5)/4095;
			tempVolt=(float)(globalTemp*10)/4095;
			sprintf(output,"Temperature = %f             ", tempVolt);
			writeToDisplay (5, 2, output );
			sprintf(output,"Poti = %f        ", potiVolt);
			writeToDisplay (6, 2, output );
			break;
		case '3': writeToDisplay (8, 2, "                       ");
			sprintf(output,"Temperature = %x          ", globalTemp);
			writeToDisplay (5, 2, output );
			sprintf(output,"Poti = %x           ", globalPoti);
			writeToDisplay (6, 2, output );
			break;
		default: writeToDisplay (8, 2, "Wrong User Input" );
		}
		semGive (writeDisplay);
		beLazy (100);
		
		

	}
	
}

int readKeyboard (void)
{
	
	
	while(1){
		
		char key;
		key = getKey();
		if(key){
			globalKey=key;
		}
		beLazy (100);
		
	}
	
}
int timer (void)
{
	
	int t;
	char output[32];

	while(1){
		
		time_t t;
		struct tm * timeinfo;

		time ( &t );
		timeinfo = localtime ( &t );
		
		semTake (writeDisplay, WAIT_FOREVER);
		sprintf(output,"Current local time and date: %s", asctime (timeinfo));
		writeToDisplay (10, 2, output );
		
		
		semGive (writeDisplay);
		beLazy (3000);
		
	}
	
}
void main (void)
{
	

	/* Add your code here: create tasks, semaphores, ... */
	initHardware(0);
	
	analogInputs = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
	writeDisplay = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

	int readInputsID;
	readInputsID = taskSpawn ("readInputs", 150, 0,0x1000,(FUNCPTR) readInputs,0,0,0,0,0,0,0,0,0,0);
	
	int showInputsID;
	showInputsID = taskSpawn ("showInputs", 160, 0,0x1000,(FUNCPTR) showInputs,0,0,0,0,0,0,0,0,0,0);
	
	int readKeyboardID;
	readKeyboardID = taskSpawn ("readKeyboard", 170, 0,0x1000,(FUNCPTR) readKeyboard,0,0,0,0,0,0,0,0,0,0);

	int timerID;
	timerID = taskSpawn ("timer", 140, 0,0x1000,(FUNCPTR) timer,0,0,0,0,0,0,0,0,0,0);

	printf("Hello World");

	/* Suspend own task */
	taskSuspend (0);
} /* main */
