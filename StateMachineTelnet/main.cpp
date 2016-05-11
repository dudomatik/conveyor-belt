
//Defines For Server
#define SERVER_PORT_NUM 4444 /* server's port number for bind() */
#define SERVER_WORK_PRIORITY 180 /* priority of server's work task */
#define SERVER_STACK_SIZE 10000 /* stack size of server's work task */
#define SERVER_MAX_CONNECTIONS 4 /* max clients connected at a time */
#define REQUEST_MSG_SIZE 1024 /* max size of request message */
#define REPLY_MSG_SIZE 500 /* max size of reply message */
/* structure for requests from clients to server */
//struct request {
	//int reply; /* TRUE = request reply from server */
	//int msgLen; /* length of message text */
	//char message[REQUEST_MSG_SIZE]; /* message buffer */
//};

//Includes for Statemachine
#include <taskLib.h>
#include <stdio.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdLib.h>
#include "ifLib.h"
#include "systemManager.h"
#include "stateMachine.h"
#include "fioLib.h"

//#include "tcpExample.h"



extern "C" 
{
#include "hardware/hwFunc.h"
//Includes for Server

//#include "disp.h"
#include "FHV.h"
//#include "hw.h"
#include "time.h"
#include "stdlib.h"
#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"

}
char output[64];
SystemManager * mySystemManager;
StateMachine * myStateMachine;

void setLocalIp();






/* MEM.c - Pilsan Timer Example Program */










SEM_ID analogInputs = NULL;
SEM_ID writeDisplay = NULL;

VOID tcpServerWorkTask(int sFd, /* server's socket file descriptor */
char * address, /* client's socket address */
u_short port /* client's socket port */
) 
{	
	char clientRequest[200];
	//struct request clientRequest; /* request/message from client */
	int nRead; /* number of bytes read */
	static char replyMsg[] = "Gruess Gott in Vorarlberg\n";
	

	/* read client request, display message */
	/*
	writeToDisplay (14, 2, "tcpServerWorkTask" );
	*/
	write(sFd, replyMsg, sizeof(replyMsg));
	
	while ((nRead = fioRdString(sFd, (char *) &clientRequest, sizeof(clientRequest)))> 0) {
		
		//printf("MESSAGE FROM CLIENT (Internet Address %s, port %d):\n%s\n",address, port, clientRequest.message);
		
			
		switch(clientRequest[0]){
		case '1': printf("**Start Key Pressed \n"); //start
		myStateMachine->sendEvent("startKey");
		sprintf(replyMsg,"Start Key Pressed              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case '2': printf("**Direction: Left\n"); //left
		myStateMachine->sendEvent("leftKey");
		sprintf(replyMsg,"Direction = left              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case '3': printf("**Direction: Right\n"); //rigth
		myStateMachine->sendEvent("rightKey");
		sprintf(replyMsg,"Direction = right              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case '4': printf("**Speed Increased\n"); //increase
		myStateMachine->sendEvent("incKey");
		sprintf(replyMsg,"Speed Increased              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case '5': printf("**Speed Decreased\n"); //decrease
		myStateMachine->sendEvent("decKey");
		sprintf(replyMsg,"Speed Decreased              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'A': printf("**Local Operation Mode selected\n"); //Local op
		myStateMachine->sendEvent("modeLomKey");
		sprintf(replyMsg,"Mode = Local Operation Mode              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'B': printf("**Chain Operation Mode selected\n"); //chain op
		myStateMachine->sendEvent("modeComKey");
		sprintf(replyMsg,"Mode = Chain Operation Mode              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'F': printf("**Request received\n"); //chain op
		myStateMachine->sendEvent("RequestUpdate");
		sprintf(replyMsg,"Request Received              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'E': printf("**Ready received\n");
		myStateMachine->sendEvent("getReady");
		sprintf(replyMsg,"Ready received              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'D': printf("**Wait received\n");
		myStateMachine->sendEvent("getWait");
		sprintf(replyMsg,"Wait received              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'C': printf("**Release received\n");
		myStateMachine->sendEvent("getRelease");
		sprintf(replyMsg,"Release received              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		}

	}
	
	
	
	if (nRead == ERROR) /* error from read() */
		perror("read");

	close(sFd); /* close server socket connection */
}

//Sever
STATUS tcpServer(void) {
	struct sockaddr_in serverAddr; /* server's socket address */
	struct sockaddr_in clientAddr; /* client's socket address */
	int sockAddrSize; /* size of socket address structure */
	int sFd; /* socket file descriptor */
	int newFd; /* socket descriptor from accept */
	int ix = 0; /* counter for work task names */
	char workName[16]; /* name of work task */
	
	/* set up the local address */
	
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* create a TCP-based socket */
	
	if ((sFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
		return (ERROR);
	}
	
	/* bind socket to local address */
	
	if (bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("bind");
		close(sFd);
		return (ERROR);
	}
	
	/* create queue for client connection requests */
	
	if (listen(sFd, 4) == ERROR) {
		perror("listen");
		close(sFd);
		return (ERROR);
	}
	
	/* accept new connect requests and spawn tasks to process them */
	
	FOREVER {
		writeToDisplay (22, 2, "tcpServer established " );
		socklen_t len = sizeof(clientAddr);
		if ((newFd = accept(sFd, (struct sockaddr *) &clientAddr, &len))
				== ERROR) {

			perror("accept");
			close(sFd);
			return (ERROR);
		}
		sprintf(workName, "tTcpWork%d", ix++);
		if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
				(FUNCPTR) tcpServerWorkTask, newFd,
				(int) inet_ntoa(clientAddr.sin_addr),
				ntohs(clientAddr.sin_port), 0, 0, 0, 0, 0, 0, 0) == ERROR) {
			/* if taskSpawn fails, close fd and return to top of loop */
			perror("taskSpawn");
			close(newFd);
		}
	}
}


/*

void my_ISR (void)
{
	// Add your code here! 
	//printf(".");

	semGive (analogInputs);
	
	// Clear AIO interrupts by writing anything to a specified address 
	sysOutByte (aioBase + intClearAddress, 0);
}

int readInputs (void)
{
	unsigned char tempOut;
	// Connect interrupt service routine to vector and all stuff 
	intConnect (INUM_TO_IVEC(aioIntNum), my_ISR, aioIntNum);
	sysIntEnablePIC (aioIRQNum);
	// Enable interrupts on the aio:
	// All interrupts and interrupt from counter 1 too 
	tempOut = 0x24;
	sysOutByte (aioBase + intEnAddress, tempOut);
	// Start counter 1 as timer with 50 ms period 
	// It has a clock input of 1 MHz = 1 µs 
	// Therefore the load value is 0xC350 = 50000 
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
*/
/*
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
*/
/*
//Bruachen wir nicht --> state machine
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
*/

/*
// brauchen wir nicht -> schreibt nur datum aufs display
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
*/

//Starts Main
void main (void) {

	// Set tick to 5 ms. This is the time resolution!
	sysClkRateSet(200);

	initHardware(0);
	
	// Set local IP address according to MAC table
	setLocalIp();

	// Create instance of top class
	mySystemManager = new SystemManager;
	
	sprintf(output,"Direction = right  ");
	writeToDisplay (4, 2, output );
	sprintf(output,"Speed = 500             ");
	writeToDisplay (3, 2, output );
	sprintf(output,"Selected Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	sprintf(output,"[1] Start");
	writeToDisplay (1, 55, output );
	sprintf(output,"[2] Direction = left");
	writeToDisplay (2, 55, output );
	sprintf(output,"[3] Direction = right");
	writeToDisplay (3, 55, output );
	sprintf(output,"[4] Speed +100");
	writeToDisplay (4, 55, output );
	sprintf(output,"[5] Speed -100");
	writeToDisplay (5, 55, output );
	sprintf(output,"[A] Local Operation Mode");
	writeToDisplay (6, 55, output );
	sprintf(output,"[B] Chain Operation Mode");
	writeToDisplay (7, 55, output );

	sprintf(output,"Motor: idle                 ");
	writeToDisplay (20, 2, output );
	
	//debugg
	sprintf(output,"[F] Request");
	writeToDisplay (9, 55, output );
	sprintf(output,"[E] Ready");
	writeToDisplay (10, 55, output );
	sprintf(output,"[D] Wait");
	writeToDisplay (11, 55, output );
	sprintf(output,"[C] Release");
	writeToDisplay (12, 55, output );
	

	/* Add your code here: create tasks, semaphores, ... */
	
	analogInputs = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
	writeDisplay = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

	//int readInputsID;
	//readInputsID = taskSpawn ("readInputs", 150, 0,0x1000,(FUNCPTR) readInputs,0,0,0,0,0,0,0,0,0,0);
	
	//int showInputsID;
	//showInputsID = taskSpawn ("showInputs", 160, 0,0x1000,(FUNCPTR) showInputs,0,0,0,0,0,0,0,0,0,0);
	
	//int readKeyboardID;
	//readKeyboardID = taskSpawn ("readKeyboard", 170, 0,0x1000,(FUNCPTR) readKeyboard,0,0,0,0,0,0,0,0,0,0);

	//int timerID;
	//timerID = taskSpawn ("timer", 140, 0,0x1000,(FUNCPTR) timer,0,0,0,0,0,0,0,0,0,0);
	
	int tcpServerID;
	tcpServerID = taskSpawn ("tcpServer", 200, 0,0x1000,(FUNCPTR) tcpServer,0,0,0,0,0,0,0,0,0,0);


	/* Suspend own task */
	//taskSuspend (0);
		
	// Start the state machine. This method blocks, so no while(1) is needed.
	myStateMachine->runToCompletion();
		
}










//Statemachine
//Diagram0
void myActionDirLeft(){
	mySystemManager->actionDirLeft();
		return;
}
void myActionDirRight(){
	mySystemManager->actionDirRight();
		return;
}
void myActionSpeedInc(){
	mySystemManager->actionSpeedInc();
		return;
}
void myActionSpeedDec(){
	mySystemManager->actionSpeedDec();
		return;
}

void myActionStartMovingLocal(){
	mySystemManager->actionStartMovingLocal();
		return;
}

void myActionLeaveStateMovingLocal(){
//	mySystemManager->actionLeaveStateMovingLocal();
		return;
}

void myActionDoNothing(){
	mySystemManager->actionDoNothing();
		return;
}

void myActionStartCOM(){
	mySystemManager->actionStartCOM();
		return;
}
void myActionStayLOM(){
	mySystemManager->actionStayLOM();
		return;
}


//Diagram1
void myActionModeLom(){
	mySystemManager->actionModeLom();
		return;
}
void myActionModeCom(){
	mySystemManager->actionModeCom();
		return;
}

void myActionRequestUpdate(){
	mySystemManager->actionRequestUpdate();
		return;
}

//Diagram2
void myActionKeyboard(){
	mySystemManager->actionKeyboard();
	return;
}

//Diagram 3 
void myActionRunCustomProfile(){
	mySystemManager->actionRunCustomProfile();
		return;
}

void myActionCustomProfileDone(){
	mySystemManager->actionCustomProfileDone();
		return;
}

void myActionRunStandardProfile(){
	mySystemManager->actionRunStandardProfile();
		return;
}

void myActionStandardProfileDone(){
	mySystemManager->actionStandardProfileDone();
		return;
}
void myActionStartSlowMovement(){
	mySystemManager->actionStartSlowMovement();
		return;
}
void myActionStopSlowMovement(){
	mySystemManager->actionStopSlowMovement();
		return;
}
void actionStartSlowMovement();

//Diagram 4
void myActionStartLOM(){
	mySystemManager->actionStartLOM();
		return;
}
void myActionStayCOM(){
	mySystemManager->actionStayCOM();
		return;
}
void myActionRequest(){
	mySystemManager->actionRequest();
		return;
}

//Diagram 5

void myActionStartSlowMovement1(){
	mySystemManager->actionStartSlowMovement1();
		return;
}
void myActionStartStandardProfile(){
	mySystemManager->actionStartStandardProfile();
		return;
}
void myActionStopStandardProfile(){
	mySystemManager->actionStopStandardProfile();
		return;
}
void myActionStartSlowMovement2(){
	mySystemManager->actionStartSlowMovement2();
		return;
}
void myActionMovementDone(){
	mySystemManager->actionMovementDone();
		return;
}
void myActionGetWait(){
	mySystemManager->actionGetWait();
		return;
}
//Diagram 6

void myActionStartRampUp(){
	mySystemManager->actionStartRampUp();
		return;
}
void myActionUpdateRampUp(){
	mySystemManager->actionUpdateRampUp();
		return;
}
void myActionStartConstantMovement(){
	mySystemManager->actionStartConstantMovement();
		return;
}
void myActionUpdateConstantMovement(){
	mySystemManager->actionUpdateConstantMovement();
		return;
}
void myActionStartRampDown(){
	mySystemManager->actionStartRampDown();
		return;
}
void myActionUpdateRampDown(){
	mySystemManager->actionUpdateRampDown();
		return;
}
void myActionStopMotor(){
	mySystemManager->actionStopMotor();
		return;
}
void myActionMoveSlow(){
	mySystemManager->actionMoveSlow();
		return;
}
void myActionUpdateSlowMovement(){
	mySystemManager->actionUpdateSlowMovement();
		return;
}


//Conditions
bool myConditionTrue(){
	return mySystemManager->conditionTrue();
}
//Diagram0
bool myConditionCOM(){
	return mySystemManager->conditionCOM();
}
bool myConditionLOM(){
	return mySystemManager->conditionLOM();
}
bool myConditionCOMRequest0(){
	return mySystemManager->conditionCOMRequest0();
}
bool myConditionCOMRequest1(){
	return mySystemManager->conditionCOMRequest1();
}

//Diagram6

bool myConditionU1s(){
	return mySystemManager->conditionU1s();
}
bool myConditionO1s(){
	return mySystemManager->conditionO1s();
}
bool myConditionU6s(){
	return mySystemManager->conditionU6s();
}
bool myConditionO6s(){
	return mySystemManager->conditionO6s();
}

