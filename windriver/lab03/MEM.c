/* MEM.c - Pilsan Timer Example Program */


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

#include "hwFunc.h"
#include "disp.h"
#include "FHV.h"
#include "hw.h"
#include "time.h"
#include "stdlib.h"
#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"
//#include "tcpExample.h"

int globalPoti;
int globalTemp;
char globalKey;

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
	float potiVolt;

	/* read client request, display message */
	
	writeToDisplay (14, 2, "tcpServerWorkTask" );
	
	write(sFd, replyMsg, sizeof(replyMsg));
	
	while ((nRead = fioRdString(sFd, (char *) &clientRequest, sizeof(clientRequest)))> 0) {
		
		//printf("MESSAGE FROM CLIENT (Internet Address %s, port %d):\n%s\n",address, port, clientRequest.message);
		char output[64];
		switch (clientRequest[0]) {
		case '1':
			sprintf(output,"Poti Dezimal = %d               ", globalPoti);
			write(sFd, output, 64);
			break;
		case '2':
			potiVolt=(float)(globalPoti*5)/4095;
			sprintf(output,"Poti Volt = %f        ", potiVolt);
			write(sFd, output, 64);
			break;
		case '3':
			sprintf(output,"Poti Hex = %x             ", globalPoti);
			write(sFd, output, 64);
			break;
		default:
			write(sFd, "Wrong input", 11);
		}

	}
	
	
	
	if (nRead == ERROR) /* error from read() */
		perror("read");

	close(sFd); /* close server socket connection */
}

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
		writeToDisplay (16, 2, "tcpServer" );
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
	
	int tcpServerID;
	tcpServerID = taskSpawn ("tcpServer", 200, 0,0x1000,(FUNCPTR) tcpServer,0,0,0,0,0,0,0,0,0,0);

	printf("Hello World");

	/* Suspend own task */
	taskSuspend (0);
} /* main */
