
#include "systemManager.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "myFunctions.h"
extern "C" 
{
#include "hardware/hwFunc.h"
}

int n, m; 
int speed = 500;
char output[64];
bool dir; // 0 = left, 1 = right
bool mode; // 0 = lom, 1 = com
StateMachine * myStateMachine;
Keyboard * myKeyboard;

SystemManager :: SystemManager() {
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!

	tab[0][0] = new TableEntry ("StateIdleLocal","StateIdleLocal","leftKey",0,myActionDirLeft,myConditionTrue);
	tab[0][1] = new TableEntry ("StateIdleLocal","StateIdleLocal","rightKey",0,myActionDirRight,myConditionTrue);
	tab[0][2] = new TableEntry ("StateIdleLocal","StateIdleLocal","incKey",0,myActionSpeedInc,myConditionTrue);
	tab[0][3] = new TableEntry ("StateIdleLocal","StateIdleLocal","decKey",0,myActionSpeedDec,myConditionTrue);
	tab[0][4] = new TableEntry ("StateIdleLocal","StateMovingLocal","startKey",0,myActionStart,myConditionTrue);		//myActionStartMovingLocal
	tab[0][5] = new TableEntry ("StateMovingLocal","StateIdleLocal","TrigMovementDone",0,myActionDoNothing,myConditionTrue); 
	
	
	tab[1][0] = new TableEntry ("StateUpdate1","StateUpdate2","TrigStartMovement",0,myActionDoNothing,myConditionTrue);
	tab[1][1] = new TableEntry ("StateUpdate2","StateUpdate2","modeLomKey",0,myActionModeLom,myConditionTrue);
	tab[1][2] = new TableEntry ("StateUpdate2","StateUpdate2","modeComKey",0,myActionModeCom,myConditionTrue);
	tab[1][3] = new TableEntry ("StateUpdate2","StateUpdate1","TrigMovementDone",0,myActionDoNothing,myConditionTrue);
	//tab[1][4] = new TableEntry ("StateUpdate2","StateUpdate2","getRequest",0,myActionGetRequest,myConditionTrue); 	sp�ter machen

	tab[2][0] = new TableEntry ("StateK","StateK","Timer2",50,myAction20,myCondition20);
	
	/*
	tab[3][0] = new TableEntry ("StateMotorIdle","StateCustomProfile","TrigStartProfile",0,myActionRunCustomProfile,myConditionLOM); // define condition and action
	tab[3][1] = new TableEntry ("StateMotorIdle","StateStandardProfile","TrigStartProfile",0,myActionRunStandardProfile,myConditionCOM); // define condition and action
	tab[3][2] = new TableEntry ("StateCustomProfile","StateMotorIdle","Timer8s",8000,myActionCustomProfileDone,myConditionTrue); //timer definieren, aktion definieren
	tab[3][3] = new TableEntry ("StateStandardProfile","StateMotorIdle","Timer8s",8000,myActionStandardProfileDone,myConditionTrue); // aktion definieren	
	tab[3][4] = new TableEntry ("StateMotorIdle","StateSlowMovement","TrigStartSlowMovement",0,myActionRunSlowMovement,myConditionTrue);//define action, 
	tab[3][5] = new TableEntry ("StateSlowMovement","StateMotorIdle","TrigStopSlowMovement",0,myActionSlowMovementDone,myConditionTrue); //define action,
	*/

	/*
	tab[4][0] = new TableEntry ("StateIdleChain","StateMovingChain","getRequest",0,myActionRequest,myConditionTrue); // define condition and action
	tab[4][1] = new TableEntry ("StateMovingChain","StateIdleChain","getRelease",0,myActionRelease,myConditionTrue); // define condition and action
	*/

	/*
	tab[5][0] = new TableEntry ("StateIdleMovingChain","StateSlowMovement","TrigStartMovement",0,myActionStartSlowMovement1,myConditionTrue); // define action
	tab[5][1] = new TableEntry ("StateSlowMovement","StateStandardProfile","Timer1s",1000,myActionRunStandardProfile,myConditionTrue); // define timer and action
	tab[5][2] = new TableEntry ("StateStandardProfile","StateWaitForAnswer","TrigMotorDone",0,myActionStandardProfileDone,myConditionTrue); //aktion definieren
	tab[5][3] = new TableEntry ("StateWaitForAnswer","StateWaitForAnswer","getWait",0,myActionDoNothing,myConditionTrue);
	tab[5][4] = new TableEntry ("StateWaitForAnswer","StateWaitForRelease","getReady",0,myActionStartSlowMovement2,myConditionTrue);//define action, 
	tab[5][5] = new TableEntry ("StateWaitForRelease","StateIdleMovingChain","getRelease",0,myActionMovementDone,myConditionTrue); //define action
	*/
	



	/*
	tab[1][0] = new TableEntry ("StateUpdateMode0","StateUpdateMode1","Trig0",0,myActionUpdate1,myConditionTrue);
	tab[1][1] = new TableEntry ("StateUpdateMode1","StateUpdateMode1","modeLomKey",0,myActionModeLom,myConditionTrue);
	tab[1][2] = new TableEntry ("StateUpdateMode1","StateUpdateMode1","modeComKey",0,myActionModeCom,myConditionTrue);
	tab[1][3] = new TableEntry ("StateUpdateMode1","StateUpdateMode0","Trig8s",8000,myActionUpdate2,myConditionTrue);
	*/
	//tab[0][1] = new TableEntry ("StateA","StateB","Timer0",2000,myAction01,myCondition01);
	//tab[0][2] = new TableEntry ("StateB","StateA","Trigg0",0,myAction02,myCondition02);

	//tab[1][0] = new TableEntry ("StateC","StateD","Trigg1",0,myAction10,myCondition10);
	//tab[1][1] = new TableEntry ("StateD","StateD","Timer1",4000,myAction11,myCondition11);
	//tab[1][2] = new TableEntry ("StateD","StateE","Timer1",4000,myAction12,myCondition12);
	//tab[1][3] = new TableEntry ("StateE","StateC","Timer1",3000,myAction13,myCondition13);
	
	//tab[2][0] = new TableEntry ("StateK","StateK","Timer2",200,myAction20,myCondition20);

	
	timerNames[1] = "Timer0";
	timerNames[2] = "Timer2";
	
	// Initialize timer names for all diagrams
	// Timer names are always Timer followed by the diagram number
	//timerNames[0] = "Timer0";
	//timerNames[1] = "Timer1";
	//timerNames[2] = "Timer2";

	// Initialize line numbers for all diagrams
	lines[0] = 6; //local mode	
	lines[1] = 4; //updating mode (5 lines (request))
	lines[2] = 1; //Keyboard	
	//lines[3] = 6; //Motor

	// Initialize first state for all diagrams
	actualState[0] = "StateIdleLocal";
	actualState[1] = "StateUpdate1";
	actualState[2] = "StateK";
	actualState[3] = "StateMotorIdle";
	
	//actualState[0] = "StateA";
	//actualState[1] = "StateC";
	//actualState[2] = "StateK";
	
	
	// Set the actual number of diagrams
	diagrams = 3;
	
	// Create instance of my Keyboard
	myKeyboard = new Keyboard;

	// Create instance of state machine
	myStateMachine = new StateMachine;

	// Start timer for each diagram which needs one in the first state!
	// In my case these are diagram 0 and 2
	myStateMachine->diaTimerTable[1]->startTimer(tab[1][3]->eventTime);
	myStateMachine->diaTimerTable[2]->startTimer(tab[2][0]->eventTime);

	// Initial actions can be done here, if needed!
	n = 0;
	m = 0;
	mode = 0;
	dir = 1;

	return;
}

SystemManager :: ~SystemManager() {
	return;
}



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

void SystemManager :: actionStart(){ // Start of custom profle
	printf("Start Key pressed \n\r"); 
	// custom profil (speed, direction)
	myStateMachine->sendEvent("TrigStartMovement");
	myStateMachine->sendEvent("TrigStartProfile");
	return;
}
void SystemManager :: actionDoNothing(){ // do nothing 
	//printf("no mode updates \n\r"); 
	return;
}

void SystemManager :: actionUpdate2(){ // wait 8 seconds, than go back to update 1
	myStateMachine->sendEvent("TrigMovementDone");
	return;
}
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






void SystemManager :: action01(){
	printf(" StateA -> Transition01 -> StateB\n\r"); 
	myStateMachine->sendEvent("Trigg1");
	return;
}

void SystemManager :: action02(){
	printf(" StateB -> Transition02 -> StateA\n\r"); 
	n = 0;
	return;
}

void SystemManager :: action10(){
	printf(" StateC -> Transition10 -> StateD\n\r"); 
	m = 0;
	return;
}

void SystemManager ::action11(){
	printf(" StateD -> Transition11 -> StateD\n\r"); 
	m++;
	return;
}

void SystemManager :: action12(){
	printf(" StateD -> Transition12 -> StateE\n\r"); 
	return;
}

void SystemManager :: action13(){
	printf(" StateE -> Transition13 -> StateC\n\r"); 
	myStateMachine->sendEvent("Trigg0");
	return;
}

void SystemManager :: action20(){
	myKeyboard->getPressedKey();
	return;
}

bool SystemManager :: conditionTrue(){
	return TRUE;
}

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
