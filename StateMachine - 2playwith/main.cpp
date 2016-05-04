
#include <taskLib.h>
#include <stdio.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdLib.h>
#include "ifLib.h"
#include "systemManager.h"
#include "stateMachine.h"

SystemManager * mySystemManager;
StateMachine * myStateMachine;

void setLocalIp();

int main (void) {

	// Set tick to 5 ms. This is the time resolution!
	sysClkRateSet(200);
	
	// Set local IP address according to MAC table
	setLocalIp();

	// Create instance of top class
	mySystemManager = new SystemManager;

	// Start the state machine. This method blocks, so no while(1) is needed.
	myStateMachine->runToCompletion();
	
}

void noAction(){
	mySystemManager->noAction();
	return;
}

void setLocalModeTrue(){
	mySystemManager->setLocalModeTrue();
	return;
}

void keyPressed(){
	mySystemManager->keyPressed();
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

bool noNewKey(){
	return mySystemManager->noNewKey();
}

bool keyB(){
	return mySystemManager->keyB();
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
