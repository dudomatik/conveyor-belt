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
 
#include <stdio.h>
#include <sysLib.h>
#include <stdLib.h>
#include "telnet.h"
#include "fioLib.h"
#include "ifLib.h"
#include "systemManager.h"
#include "stateMachine.h"
#include "time.h"
#include "stdlib.h"
#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "FHV.h"

extern "C" 
{
	#include "hardware/hwFunc.h"
	//Includes for Server
	

}

StateMachine * myStateMachine;

//Variablen
char output[64];
SEM_ID analogInputs = NULL;
SEM_ID writeDisplay = NULL;

void setLocalIp();

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
		
		case '5': printf("**Speed Decreased\n");
		myStateMachine->sendEvent("decKey");
		sprintf(replyMsg,"Speed Decreased              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'A': printf("**Local Operation Mode selected\n"); 
		myStateMachine->sendEvent("modeLomKey");
		sprintf(replyMsg,"Mode = Local Operation Mode              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'B': printf("**Chain Operation Mode selected\n"); 
		myStateMachine->sendEvent("modeComKey");
		sprintf(replyMsg,"Mode = Chain Operation Mode              \n");
		write(sFd, replyMsg, sizeof(replyMsg));
		break;
		
		case 'F': printf("**Request received\n"); 
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


Telnet :: Telnet() {
	
	printf("telnet Konstruktor!\n\r");	
	
	analogInputs = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
	writeDisplay = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

		
	int tcpServerID;
	tcpServerID = taskSpawn ("tcpServer", 200, 0,0x1000,(FUNCPTR) tcpServer,0,0,0,0,0,0,0,0,0,0);
	return;
}

Telnet :: ~Telnet() {
	//pressedKey = 0;
	return;
}


			


