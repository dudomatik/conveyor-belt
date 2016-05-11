
#include <taskLib.h>
#include <stdio.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdLib.h>
#include "ifLib.h"
#include "systemManager.h"
#include "stateMachine.h"

extern "C" 
{
#include "hardware/hwFunc.h"
}
char output[64];
SystemManager * mySystemManager;
StateMachine * myStateMachine;

void setLocalIp();

int main (void) {

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

		
	// Start the state machine. This method blocks, so no while(1) is needed.
	myStateMachine->runToCompletion();
	

	
}

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

