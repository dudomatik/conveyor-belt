
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

	// Start the state machine. This method blocks, so no while(1) is needed.
	myStateMachine->runToCompletion();
	
}

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

void myActionStart(){
	mySystemManager->actionStart();
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

void myActionUpdate2(){
	mySystemManager->actionUpdate2();
		return;
}
void myActionModeLom(){
	mySystemManager->actionModeLom();
		return;
}
void myActionModeCom(){
	mySystemManager->actionModeCom();
		return;
}

void myAction00(){
//	mySystemManager->action00();
	return;
}

void myAction01(){
	mySystemManager->action01();
	return;
}

void myAction02(){
	mySystemManager->action02();
	return;
}

void myAction10(){
	mySystemManager->action10();
	return;
}

void myAction11(){
	mySystemManager->action11();
	return;
}

void myAction12(){
	mySystemManager->action12();
	return;
}

void myAction13(){
	mySystemManager->action13();
	return;
}

void myAction20(){
	mySystemManager->action20();
	return;
}

bool myConditionTrue(){
	return mySystemManager->conditionTrue();
}

bool myCondition00(){
	return mySystemManager->condition00();
}

bool myCondition01(){
	return mySystemManager->condition01();
}

bool myCondition02(){
	return mySystemManager->conditionTrue();
}

bool myCondition10(){
	return mySystemManager->conditionTrue();
}

bool myCondition11(){
	return mySystemManager->condition11();
}

bool myCondition12(){
	return mySystemManager->condition12();
}

bool myCondition13(){
	return mySystemManager->conditionTrue();
}

bool myCondition20(){
	return mySystemManager->conditionTrue();
}
