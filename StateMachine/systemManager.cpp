
#include "systemManager.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "myFunctions.h"
extern "C" 
{
#include "hardware/hwFunc.h"
}

bool request; 
int speed = 500;
double binspeed;
char output[64];
bool dir = 1; // 0 = left, 1 = right
bool mode = 0; // 0 = lom, 1 = com
StateMachine * myStateMachine;
Keyboard * myKeyboard;

SystemManager :: SystemManager() {
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!

	tab[0][0] = new TableEntry ("StateIdleLOM","StateIdleLOM","leftKey",0,myActionDirLeft,myConditionTrue);
	tab[0][1] = new TableEntry ("StateIdleLOM","StateIdleLOM","rightKey",0,myActionDirRight,myConditionTrue);
	tab[0][2] = new TableEntry ("StateIdleLOM","StateIdleLOM","incKey",0,myActionSpeedInc,myConditionTrue);
	tab[0][3] = new TableEntry ("StateIdleLOM","StateIdleLOM","decKey",0,myActionSpeedDec,myConditionTrue);
	tab[0][4] = new TableEntry ("StateIdleLOM","StateMovingLOM","startKey",0,myActionStartMovingLocal,myConditionTrue);
	//tab[0][5] = new TableEntry ("StateMovingLocal","StateIdleLocal","TrigMovementDone",0,myActionDoNothing,myConditionTrue); 
	tab[0][5] = new TableEntry ("StateMovingLOM","StateIdleLOM","TrigMovementDone",0,myActionStayLOM,myConditionLOM); 
	tab[0][6] = new TableEntry ("StateMovingLOM","StatePauseLOM","TrigMovementDone",0,myActionStartCOM,myConditionCOM);
	tab[0][7] = new TableEntry ("StateIdleLOM","StatePauseLOM","modeComKey",0,myActionStartCOM,myConditionTrue);
	tab[0][8] = new TableEntry ("StateIdleLOM","StateIdleLOM","modeLomKey",0,myActionStayLOM,myConditionTrue);
	tab[0][9] = new TableEntry ("StatePauseLOM","StateIdleLOM","TrigStartLOM",0,myActionDoNothing,myConditionTrue);
	
	
	tab[1][0] = new TableEntry ("StateUpdate1","StateUpdate2","TrigStartMovementLOM",0,myActionDoNothing,myConditionTrue);
	tab[1][1] = new TableEntry ("StateUpdate2","StateUpdate2","modeLomKey",0,myActionModeLom,myConditionTrue);
	tab[1][2] = new TableEntry ("StateUpdate2","StateUpdate2","modeComKey",0,myActionModeCom,myConditionTrue);
	tab[1][3] = new TableEntry ("StateUpdate2","StateUpdate1","TrigMovementDone",0,myActionDoNothing,myConditionTrue);
	tab[1][4] = new TableEntry ("StateUpdate2","StateUpdate2","RequestUpdate",0,myActionRequestUpdate,myConditionTrue); 	//später machen
	tab[1][5] = new TableEntry ("StateUpdate1","StateUpdate2","TrigStartMovementCOM",0,myActionDoNothing,myConditionTrue);
	
	tab[2][0] = new TableEntry ("StateK","StateK","Timer2",50,myActionKeyboard,myConditionTrue);
	
	
	tab[3][0] = new TableEntry ("StateMotorIdle","StateCustomProfile","TrigStartProfile",0,myActionRunCustomProfile,myConditionLOM);
	tab[3][1] = new TableEntry ("StateMotorIdle","StateStandardProfile","TrigStartProfile",0,myActionRunStandardProfile,myConditionCOM);
	tab[3][2] = new TableEntry ("StateCustomProfile","StateMotorIdle","Timer8s",8000,myActionCustomProfileDone,myConditionTrue); 
	tab[3][3] = new TableEntry ("StateStandardProfile","StateMotorIdle","Timer8s",8000,myActionStandardProfileDone,myConditionTrue); 
	
	tab[3][4] = new TableEntry ("StateMotorIdle","StateSlowMovement","TrigStartSlowMovement",0,myActionStartSlowMovement,myConditionTrue); 
	tab[3][5] = new TableEntry ("StateSlowMovement","StateMotorIdle","TrigStopSlowMovement",0,myActionStopSlowMovement,myConditionTrue); 

	
	tab[4][0] = new TableEntry ("StatePauseCOM","StateIdleCOM","TrigStartCOM",0,myActionDoNothing,myConditionTrue); 
	tab[4][1] = new TableEntry ("StateIdleCOM","StateIdleCOM","modeComKey",0,myActionStayCOM,myConditionTrue);	//action com
	tab[4][2] = new TableEntry ("StateIdleCOM","StatePauseCOM","modeLomKey",0,myActionStartLOM,myConditionTrue); // action lom
	tab[4][3] = new TableEntry ("StateIdleCOM","StateMovingCOM","RequestUpdate",0,myActionRequest,myConditionTrue);
	tab[4][4] = new TableEntry ("StateMovingCOM","StateIdleCOM","TrigMovementDone",0,myActionStayCOM,myConditionCOMRequest0);
	tab[4][5] = new TableEntry ("StateMovingCOM","StatePauseCOM","TrigMovementDone",0,myActionStartLOM,myConditionLOM);
	tab[4][6] = new TableEntry ("StateMovingCOM","StateMovingCOM","TrigMovementDone",0,myActionRequest,myConditionCOMRequest1);
	
	
	
	tab[5][0] = new TableEntry ("StateIdleMovingCOM","StateSlowMovement","TrigStartMovementCOM",0,myActionStartSlowMovement1,myConditionTrue); 
	tab[5][1] = new TableEntry ("StateSlowMovement","StateStandardProfile","Timer1s",1000,myActionStartStandardProfile,myConditionTrue);	//action com
	tab[5][2] = new TableEntry ("StateStandardProfile","StateWaitForAnswer","TrigMotorDone",0,myActionStopStandardProfile,myConditionTrue); // action lom
	tab[5][3] = new TableEntry ("StateWaitForAnswer","StateWaitForAnswer","getWait",0,myActionDoNothing,myConditionTrue);
	tab[5][4] = new TableEntry ("StateWaitForAnswer","StateWaitForRelease","getReady",0,myActionStartSlowMovement2,myConditionTrue);
	tab[5][5] = new TableEntry ("StateWaitForRelease","StateIdleMovingCOM","getRelease",0,myActionMovementDone,myConditionTrue);
	
	
		
	
	
	
	
	//tab[0][1] = new TableEntry ("StateA","StateB","Timer0",2000,myAction01,myCondition01);
	//tab[0][2] = new TableEntry ("StateB","StateA","Trigg0",0,myAction02,myCondition02);

	//tab[1][0] = new TableEntry ("StateC","StateD","Trigg1",0,myAction10,myCondition10);
	//tab[1][1] = new TableEntry ("StateD","StateD","Timer1",4000,myAction11,myCondition11);
	//tab[1][2] = new TableEntry ("StateD","StateE","Timer1",4000,myAction12,myCondition12);
	//tab[1][3] = new TableEntry ("StateE","StateC","Timer1",3000,myAction13,myCondition13);
	
	//tab[2][0] = new TableEntry ("StateK","StateK","Timer2",200,myAction20,myCondition20);

	// Initialize timer names for all diagrams
	// Timer names are always Timer followed by the diagram number
	
	timerNames[2] = "Timer2";
	timerNames[3] = "Timer8s";
	timerNames[5] = "Timer1s";
	

	//timerNames[0] = "Timer0";
	//timerNames[1] = "Timer1";
	//timerNames[2] = "Timer2";

	// Initialize line numbers for all diagrams
	lines[0] = 10;
	lines[1] = 6;
	lines[2] = 1;
	lines[3] = 6;
	lines[4] = 7;
	lines[5] = 6;

	// Initialize first state for all diagrams
	actualState[0] = "StateIdleLOM";
	actualState[1] = "StateUpdate1";
	actualState[2] = "StateK";
	actualState[3] = "StateMotorIdle";
	actualState[4] = "StatePauseCOM";
	actualState[5] = "StateIdleMovingCOM";
	
	
	// Set the actual number of diagrams
	diagrams = 6;
	
	// Create instance of my Keyboard
	myKeyboard = new Keyboard;

	// Create instance of state machine
	myStateMachine = new StateMachine;

	// Start timer for each diagram which needs one in the first state!
	// In my case these are diagram 0 and 2
	myStateMachine->diaTimerTable[1]->startTimer(tab[1][3]->eventTime);
	myStateMachine->diaTimerTable[2]->startTimer(tab[2][0]->eventTime);

	// Initial actions can be done here, if needed!
	
	mode = 0;
	dir = 1;

	return;
}

SystemManager :: ~SystemManager() {
	return;
}


//Diagram 0
void SystemManager :: actionDirLeft(){
	printf("direction = left \n\r"); 
	dir = 0;
	sprintf(output,"Direction = left  ");
	writeToDisplay (3, 2, output );
	return;
}

void SystemManager :: actionDirRight(){
	printf("direction = right \n\r"); 
	dir = 1;
	sprintf(output,"Direction = right  ");
	writeToDisplay (3, 2, output );
	return;
}

void SystemManager :: actionSpeedInc(){
	if (speed >= 2200){
			printf ("maximum Speed reached \n");
			sprintf(output,"maximum Speed reached");
			writeToDisplay (2, 30, output );
		}
	else {
		speed = speed + 100;
		sprintf(output,"                         ");
		writeToDisplay (2, 30, output );
		printf("Speed = %d\n", speed);
		sprintf(output,"Speed = %d             ", speed);
		writeToDisplay (2, 2, output );
	}
	return;
}

void SystemManager :: actionSpeedDec(){
	if (speed <= 200){
		printf ("minimum Speed reached \n");
		sprintf(output,"minimum Speed reached");
		writeToDisplay (2, 30, output );
	}
	else {
		speed = speed - 100;
		sprintf(output,"                         ");
		writeToDisplay (2, 30, output );
		printf("Speed = %i \n", speed);
		sprintf(output,"Speed = %d              ", speed);
		writeToDisplay (2, 2, output );
	}
	
	return;
}

void SystemManager :: actionStartMovingLocal(){ // Start of custom profle
	printf("Start Key pressed \n\r"); 
	// custom profil (speed, direction)
	myStateMachine->sendEvent("TrigStartMovementLOM");
	myStateMachine->sendEvent("TrigStartProfile");
	return;
}

void SystemManager :: actionStartCOM(){ // do nothing 
	myStateMachine->sendEvent("TrigStartCOM");
	printf("Entering Chain Operation mode --> MODE =1 \n\r");
	sprintf(output,"Mode = Chain Operation Mode");
	writeToDisplay (1, 2, output );
	mode=1;
	return;
}
void SystemManager :: actionStayLOM(){ // do nothing 
	//myStateMachine->sendEvent("TrigStartCOM");
	printf("Stay in Local Operation mode --> MODE =0 \n\r");
	sprintf(output,"Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	mode=0;
	return;
}

void SystemManager :: actionDoNothing(){ // do nothing 
	//printf("no mode updates \n\r"); 
	return;
}


//Diagram 1
void SystemManager :: actionModeLom(){ // local operation mode selected
	mode = 0;
	printf("mode = LOM \n");
	sprintf(output,"Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	return;
}
void SystemManager :: actionModeCom(){ // chain operation mode selected
	mode = 1;
	printf("mode = COM \n");
	sprintf(output,"Mode = Chain Operation Mode");
	writeToDisplay (1, 2, output );
	return;
}


void SystemManager :: actionRequestUpdate(){ // chain operation mode selected
	request = 1;
	printf("request = 1 ++++\n");
	return;
}

//Diagram 2
void SystemManager :: actionKeyboard(){
	myKeyboard->getPressedKey();
	return;
}

//Diagram 3
void SystemManager :: actionRunCustomProfile(){
	printf("Motor on\n");
	
	switch (dir){
	case 1:
		binspeed = 2610-speed*(2610.0/2200); //2610 no movement 1 rechts
		break;
	case 0:
		binspeed = 2610+speed*((4095.0-2610.0)/2200); // o links
		break;
	}
	
	writeAnalog (0, (int)binspeed);
	motorOn();
	return;
}

void SystemManager :: actionCustomProfileDone(){
	printf("Motor off\n");
	motorOff();
	myStateMachine->sendEvent("TrigMovementDone");	
	return;
}

void SystemManager :: actionRunStandardProfile(){
	printf("Motor on\n");
	binspeed = 2610-1800*(2610.0/2200); //2610 no movement
	writeAnalog (0, (int)binspeed);
	motorOn();
	return;
}

void SystemManager :: actionStandardProfileDone(){
	printf("Motor off\n");
	motorOff();
	myStateMachine->sendEvent("TrigMotorDone");	
	return;
}

void SystemManager :: actionStartSlowMovement(){
	printf("StartSlowMovement\n");
	binspeed = 2610-200*(2610.0/2200);
	writeAnalog (0, (int)binspeed);
	motorOn();	
	return;
}

void SystemManager :: actionStopSlowMovement(){
	printf("StopSlowMovement\n");
	binspeed = 2610*(2610.0/2200);
	writeAnalog (0, (int)binspeed);
	motorOff();
	//myStateMachine->sendEvent("TrigMovementDone");	
	return;
}
//Diagram 4

void SystemManager :: actionStartLOM(){ // do nothing 
	myStateMachine->sendEvent("TrigStartLOM");
	printf("Entering Local Operation mode --> MODE =0 \n\r");
	sprintf(output,"Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	mode=0;
	return;
}
void SystemManager :: actionStayCOM(){ // do nothing 
	//myStateMachine->sendEvent("TrigStartCOM");
	printf("Stay in Chain Operation mode --> MODE =1 \n\r");
	sprintf(output,"Mode = Chain Operation Mode");
	writeToDisplay (1, 2, output );
	mode=1;
	return;
}

void SystemManager :: actionRequest(){ // do nothing 
	request = 1;
	printf("Request received++++");
	writeToDisplay (6, 1, output );
	//myStateMachine->sendEvent("TrigStartProfile");
	myStateMachine->sendEvent("TrigStartMovementCOM");
	return;
}
//Diagram 5
void SystemManager :: actionStartSlowMovement1(){
	request=0;
	
	printf("First Slow Movement Started \n");
	myStateMachine->sendEvent("TrigStartSlowMovement");
	//sendReady
	
	return;
}
void SystemManager :: actionStartStandardProfile(){  
	printf("Standard Movement Started \n");
	myStateMachine->sendEvent("TrigStopSlowMovement");
	//sendReady
	myStateMachine->sendEvent("TrigStartProfile");
	return;
}
void SystemManager :: actionStopStandardProfile(){ 
	printf("Standard Movement Stopped \n");
	//sendRequest
	return;
}
void SystemManager :: actionStartSlowMovement2(){ 
	printf("Second Slow Movement Started /n");
	myStateMachine->sendEvent("TrigStartSlowMovement");
	return;
}
void SystemManager :: actionMovementDone(){ // do nothing 
	myStateMachine->sendEvent("TrigStopSlowMovement");
	myStateMachine->sendEvent("TrigMovementDone");
	printf("moevement done /n");
	return;
}


//Conditions
bool SystemManager :: conditionTrue(){
	return TRUE;
}

bool SystemManager :: conditionCOM(){
	if (mode == 1) {
		return TRUE;
	}
	else return FALSE;
}

bool SystemManager :: conditionLOM(){
	if (mode == 0) {
		return TRUE;
	}
	else return FALSE;
}
//Diagram4
bool SystemManager :: conditionCOMRequest0(){
	if (mode == 1 && request == 0) {
		printf("conditionCOMRequest0 %i /n", request);
		return TRUE;
	}
	else return FALSE;
}
bool SystemManager :: conditionCOMRequest1(){
	if (mode == 1 && request == 1) {
		printf("conditionCOMRequest1 /n");
		return TRUE;
	}
	else return FALSE;
}
/*
bool SystemManager :: condition00(){
	if (n < 5) {
		return TRUE;
	}
	else return FALSE;
}

bool SystemManager :: condition01(){
	if (n >= 5) return TRUE;
	else return FALSE;
}

bool SystemManager :: condition11(){
	if (m < 4) return TRUE;
	else return FALSE;
}

bool SystemManager :: condition12(){
	if (m >= 4) return TRUE;
	else return FALSE;
}
 */
