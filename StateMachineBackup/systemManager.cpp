
#include "systemManager.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "myFunctions.h"
extern "C" 
{
#include "hardware/hwFunc.h"
}

#define controlTime 50
#define profileTime 8000.0
#define rampTime 1000.0
#define slowTime 1000.0
#define zeroSpeed 2610.0
#define standardProfileSpeed 1800.0
#define slowMovementSpeed 200.0

int speed = 500;
double profileSpeed;
double binspeed;
char output[64];
bool request = 0; 
bool dir = 1; // 0 = left, 1 = right
bool presentDir;
bool mode = 0; // 0 = lom, 1 = com
bool presentMode = 0;
int n = 0;
int nmax = rampTime/controlTime;
int m = 0;
int mmax = (profileTime - 2*rampTime)/controlTime;
int o = 0;


StateMachine * myStateMachine;
Keyboard * myKeyboard;

SystemManager :: SystemManager() {
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!

	
	//LOM
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
	
	//Updater (requests/keyboard)
	tab[1][0] = new TableEntry ("StateUpdate1","StateUpdate2","TrigStartMovementLOM",0,myActionDoNothing,myConditionTrue);
	tab[1][1] = new TableEntry ("StateUpdate2","StateUpdate2","modeLomKey",0,myActionModeLom,myConditionTrue);
	tab[1][2] = new TableEntry ("StateUpdate2","StateUpdate2","modeComKey",0,myActionModeCom,myConditionTrue);
	tab[1][3] = new TableEntry ("StateUpdate2","StateUpdate1","TrigMovementDone",0,myActionDoNothing,myConditionTrue);
	tab[1][4] = new TableEntry ("StateUpdate2","StateUpdate2","RequestUpdate",0,myActionRequestUpdate,myConditionTrue); 
	tab[1][5] = new TableEntry ("StateUpdate1","StateUpdate2","TrigStartMovementCOM",0,myActionDoNothing,myConditionTrue);
	
	//Keyboard
	tab[2][0] = new TableEntry ("StateK","StateK","Timer2",50,myActionKeyboard,myConditionTrue);
	
	//Motor
	tab[3][0] = new TableEntry ("StateMotorIdle","StateCustomProfile","TrigStartCustomProfile",0,myActionRunCustomProfile,myConditionTrue);
	tab[3][1] = new TableEntry ("StateMotorIdle","StateStandardProfile","TrigStartStandardProfile",0,myActionRunStandardProfile,myConditionTrue);
	tab[3][2] = new TableEntry ("StateCustomProfile","StateMotorIdle","Timer8s",profileTime,myActionCustomProfileDone,myConditionTrue); 
	tab[3][3] = new TableEntry ("StateStandardProfile","StateMotorIdle","Timer8s",profileTime,myActionStandardProfileDone,myConditionTrue); 
	tab[3][4] = new TableEntry ("StateMotorIdle","StateSlowMovement","TrigStartSlowMovement",0,myActionStartSlowMovement,myConditionTrue); 
	tab[3][5] = new TableEntry ("StateSlowMovement","StateMotorIdle","TrigStopSlowMovement",0,myActionStopSlowMovement,myConditionTrue); 

	//Chain Mode general
	tab[4][0] = new TableEntry ("StatePauseCOM","StateIdleCOM","TrigStartCOM",0,myActionDoNothing,myConditionTrue); 
	tab[4][1] = new TableEntry ("StateIdleCOM","StateIdleCOM","modeComKey",0,myActionStayCOM,myConditionTrue);	//action com
	tab[4][2] = new TableEntry ("StateIdleCOM","StatePauseCOM","modeLomKey",0,myActionStartLOM,myConditionTrue); // action lom
	tab[4][3] = new TableEntry ("StateIdleCOM","StateMovingCOM","RequestUpdate",0,myActionRequest,myConditionTrue);
	tab[4][4] = new TableEntry ("StateMovingCOM","StateIdleCOM","TrigMovementDone",0,myActionStayCOM,myConditionCOMRequest0);
	tab[4][5] = new TableEntry ("StateMovingCOM","StatePauseCOM","TrigMovementDone",0,myActionStartLOM,myConditionLOM);
	tab[4][6] = new TableEntry ("StateMovingCOM","StateMovingCOM","TrigMovementDone",0,myActionRequest,myConditionCOMRequest1);
	
	//Chain Mode Moving
	tab[5][0] = new TableEntry ("StateIdleMovingCOM","StateSlowMovement","TrigStartMovementCOM",0,myActionStartSlowMovement1,myConditionTrue); 
	tab[5][1] = new TableEntry ("StateSlowMovement","StateStandardProfile","Timer1s",slowTime,myActionStartStandardProfile,myConditionTrue);	
	tab[5][2] = new TableEntry ("StateStandardProfile","StateWaitForAnswer","TrigMotorDone",0,myActionStopStandardProfile,myConditionTrue);
	tab[5][3] = new TableEntry ("StateWaitForAnswer","StateWaitForAnswer","getWait",0,myActionGetWait,myConditionTrue);
	tab[5][4] = new TableEntry ("StateWaitForAnswer","StateWaitForRelease","getReady",0,myActionStartSlowMovement2,myConditionTrue);
	tab[5][5] = new TableEntry ("StateWaitForRelease","StateIdleMovingCOM","getRelease",0,myActionMovementDone,myConditionTrue);
	
	//Motor Controller
	tab[6][0] = new TableEntry ("StateControllerIdle","StateControllerRampUp","TrigStartProfile",0,myActionStartRampUp,myConditionTrue); 
	tab[6][1] = new TableEntry ("StateControllerRampUp","StateControllerRampUp","Timer50ms",controlTime,myActionUpdateRampUp,myConditionU1s);	
	tab[6][2] = new TableEntry ("StateControllerRampUp","StateControllerConstant","Timer50ms",controlTime,myActionStartConstantMovement,myConditionO1s); 
	tab[6][3] = new TableEntry ("StateControllerConstant","StateControllerConstant","Timer50ms",controlTime,myActionUpdateConstantMovement,myConditionU6s);
	tab[6][4] = new TableEntry ("StateControllerConstant","StateControllerRampDown","Timer50ms",controlTime,myActionStartRampDown,myConditionO6s);
	tab[6][5] = new TableEntry ("StateControllerRampDown","StateControllerRampDown","Timer50ms",controlTime,myActionUpdateRampDown,myConditionTrue);
	tab[6][6] = new TableEntry ("StateControllerRampDown","StateControllerIdle","TrigMovementDone",0,myActionStopMotor,myConditionTrue);
	tab[6][7] = new TableEntry ("StateControllerRampDown","StateControllerIdle","TrigMotorDone",0,myActionStopMotor,myConditionTrue);
	tab[6][8] = new TableEntry ("StateControllerIdle","StateControllerSlowMovement","TrigMoveSlow",0,myActionMoveSlow,myConditionTrue);
	tab[6][9] = new TableEntry ("StateControllerSlowMovement","StateControllerSlowMovement","Timer50ms",controlTime,myActionUpdateSlowMovement,myConditionTrue);
	tab[6][10] = new TableEntry ("StateControllerSlowMovement","StateControllerIdle","TrigSlowMovementDone",0,myActionStopMotor,myConditionTrue);

	
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
	timerNames[6] = "Timer50ms";
	

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
	lines[6] = 11;

	// Initialize first state for all diagrams
	actualState[0] = "StateIdleLOM";
	actualState[1] = "StateUpdate1";
	actualState[2] = "StateK";
	actualState[3] = "StateMotorIdle";
	actualState[4] = "StatePauseCOM";
	actualState[5] = "StateIdleMovingCOM";
	actualState[6] = "StateControllerIdle";
	
	
	// Set the actual number of diagrams
	diagrams = 7;
	
	// Create instance of my Keyboard
	myKeyboard = new Keyboard;

	// Create instance of state machine
	myStateMachine = new StateMachine;

	// Start timer for each diagram which needs one in the first state!
	// In my case these are diagram 0 and 2
	myStateMachine->diaTimerTable[1]->startTimer(tab[1][3]->eventTime);
	myStateMachine->diaTimerTable[2]->startTimer(tab[2][0]->eventTime);

	// Initial actions can be done here, if needed!
	

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
	writeToDisplay (4, 2, output );
	return;
}

void SystemManager :: actionDirRight(){
	printf("direction = right \n\r"); 
	dir = 1;
	sprintf(output,"Direction = right  ");
	writeToDisplay (4, 2, output );
	return;
}

void SystemManager :: actionSpeedInc(){
	if (speed >= 2200){
			printf ("maximum Speed reached \n");
			sprintf(output,"maximum Speed reached");
			writeToDisplay (3, 30, output );
		}
	else {
		speed = speed + 100;
		sprintf(output,"                         ");
		writeToDisplay (3, 30, output );
		printf("Speed = %d\n", speed);
		sprintf(output,"Speed = %d             ", speed);
		writeToDisplay (3, 2, output );
	}
	return;
}

void SystemManager :: actionSpeedDec(){
	if (speed <= 200){
		printf ("minimum Speed reached \n");
		sprintf(output,"minimum Speed reached");
		writeToDisplay (3, 30, output );
	}
	else {
		speed = speed - 100;
		sprintf(output,"                         ");
		writeToDisplay (3, 30, output );
		printf("Speed = %i \n", speed);
		sprintf(output,"Speed = %d              ", speed);
		writeToDisplay (3, 2, output );
	}
	
	return;
}

void SystemManager :: actionStartMovingLocal(){ // Start of custom profle
	printf("Start Key pressed \n\r"); 
	// custom profil (speed, direction)
	myStateMachine->sendEvent("TrigStartMovementLOM");
	myStateMachine->sendEvent("TrigStartCustomProfile");
	return;
}

void SystemManager :: actionStartCOM(){ // do nothing 
	myStateMachine->sendEvent("TrigStartCOM");
	printf("Entering Chain Operation mode --> MODE =1 \n\r");
	sprintf(output,"Selected Mode = Chain Operation Mode");
	writeToDisplay (1, 2, output );
	
	sprintf(output,"                                       ");
	writeToDisplay (2, 2, output );
	mode=1;
	return;
}
void SystemManager :: actionStayLOM(){ // do nothing 
	//myStateMachine->sendEvent("TrigStartCOM");
	printf("Stay in Local Operation mode --> MODE =0 \n\r");
	sprintf(output,"Selected Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	
	sprintf(output,"                                       ");
	writeToDisplay (2, 2, output );
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
	sprintf(output,"Next Mode = Local Operation Mode");
	writeToDisplay (2, 2, output );
	return;
}
void SystemManager :: actionModeCom(){ // chain operation mode selected
	mode = 1;
	printf("mode = COM \n");
	sprintf(output,"Next Mode = Chain Operation Mode");
	writeToDisplay (2, 2, output );
	return;
}


void SystemManager :: actionRequestUpdate(){ // chain operation mode selected
	request = 1;
	sprintf(output,"Message received: Request   ");
	writeToDisplay (18, 2, output );
	
	//send wait
	sprintf(output,"Message sent: Wait   ");
	writeToDisplay (17, 2, output );	
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
	presentMode = 0;
	myStateMachine->sendEvent("TrigStartProfile");
	sprintf(output,"Motor: Custom Profile   ");
	writeToDisplay (20, 2, output );
	return;
}

void SystemManager :: actionCustomProfileDone(){
	myStateMachine->sendEvent("TrigMovementDone");	
	sprintf(output,"Motor: idle                      ");
	writeToDisplay (20, 2, output );
	return;
}

void SystemManager :: actionRunStandardProfile(){
	presentMode = 1;
	myStateMachine->sendEvent("TrigStartProfile");
	sprintf(output,"Motor: Standard Profile   ");
	writeToDisplay (20, 2, output );
	return;
}

void SystemManager :: actionStandardProfileDone(){
	myStateMachine->sendEvent("TrigMotorDone");	
	sprintf(output,"Motor: idle                 ");
	writeToDisplay (20, 2, output );
	return;
}

void SystemManager :: actionStartSlowMovement(){
	myStateMachine->sendEvent("TrigMoveSlow");	
	sprintf(output,"Motor: Slow Movement   ");
	writeToDisplay (20, 2, output );
	return;
}

void SystemManager :: actionStopSlowMovement(){
	myStateMachine->sendEvent("TrigSlowMovementDone");	
	sprintf(output,"Motor: idle                 ");
	writeToDisplay (20, 2, output );
	return;
}
//Diagram 4

void SystemManager :: actionStartLOM(){ // do nothing 
	myStateMachine->sendEvent("TrigStartLOM");
	printf("Entering Local Operation mode --> MODE =0 \n\r");
	sprintf(output,"Selected Mode = Local Operation Mode");
	writeToDisplay (1, 2, output );
	sprintf(output,"                                        ");
	writeToDisplay (2, 2, output );
	mode=0;
	return;
}
void SystemManager :: actionStayCOM(){ // do nothing 
	//myStateMachine->sendEvent("TrigStartCOM");
	printf("Stay in Chain Operation mode --> MODE =1 \n\r");
	sprintf(output,"Next Mode = Chain Operation Mode");
	writeToDisplay (2, 2, output );
	mode=1;
	return;
}

void SystemManager :: actionRequest(){ // do nothing 
	request = 1;
	printf("Request received++++");
	sprintf(output,"Message received: Request   ");
	writeToDisplay (18, 2, output );
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
	sprintf(output,"Message sent: Ready   ");
	writeToDisplay (17, 2, output );
	
	return;
}
void SystemManager :: actionStartStandardProfile(){  
	printf("Standard Movement Started \n");
	myStateMachine->sendEvent("TrigStopSlowMovement");
	//sendRelease
	sprintf(output,"Message sent: Release   ");
	writeToDisplay (17, 2, output );
	myStateMachine->sendEvent("TrigStartStandardProfile");
	return;
}
void SystemManager :: actionStopStandardProfile(){ 
	printf("Standard Movement Stopped \n");
	//sendRequest
	sprintf(output,"Message sent: Request   ");
	writeToDisplay (17, 2, output );
	return;
}
void SystemManager :: actionStartSlowMovement2(){ 
	printf("Second Slow Movement Started /n");
	myStateMachine->sendEvent("TrigStartSlowMovement");
	sprintf(output,"Message received: Ready   ");
	writeToDisplay (18, 2, output );
	return;
}
void SystemManager :: actionMovementDone(){ 
	myStateMachine->sendEvent("TrigStopSlowMovement");
	myStateMachine->sendEvent("TrigMovementDone");
	printf("moevement done /n");
	sprintf(output,"Message received: Release   ");
	writeToDisplay (18, 2, output );
	return;
}
void SystemManager :: actionGetWait(){ 
	sprintf(output,"Message received: Wait   ");
	writeToDisplay (18, 2, output );
	return;
}

//Diagram 6
void SystemManager :: actionStartRampUp(){ 
	switch (presentMode){
	case 0:
		switch (dir){
		case 1:
			profileSpeed = speed*(zeroSpeed/2200.0); // binary speed from zero point right
			break;
		case 0:
			profileSpeed = speed*((4095.0-zeroSpeed)/2200); // binary speed from zero point left
			break;
		}
		presentDir = dir; //for ramp calculation
		break;
	case 1:
		profileSpeed = standardProfileSpeed*(zeroSpeed/2200.0); // right
		presentDir = 1;
		break;
	}
	//turn Motor on without movement
	binspeed=zeroSpeed;
	writeAnalog (0, (int)binspeed);
	printf("presentMode = %d \n",presentMode);
	printf("Profile Speed = %f \n",profileSpeed);
	motorOn();
	n=0;
	//sprintf(output,"Motor: Custom Profile   ");
	//writeToDisplay (20, 2, output );
	return;
}
void SystemManager :: actionUpdateRampUp(){ 
	switch (presentDir){
	case 0:
		binspeed = zeroSpeed+0.0+profileSpeed*((n*controlTime)/rampTime);
		break;
	case 1:
		binspeed = zeroSpeed+0.0-profileSpeed*((n*controlTime)/rampTime);
		break;
	}
	writeAnalog (0, (int)binspeed);
	printf("Speed = %f \n",binspeed);
	n++;
	return;
}
void SystemManager :: actionStartConstantMovement(){ 
	m=0;
	return;
}
void SystemManager :: actionUpdateConstantMovement(){ 
	switch (presentDir){
	case 0:
		binspeed = zeroSpeed+profileSpeed;
		break;
	case 1:
		binspeed = zeroSpeed-profileSpeed;
		break;
	}
	writeAnalog (0, (int)binspeed);
	m++;
	return;
}
void SystemManager :: actionStartRampDown(){ 
	o=0;
	return;
}
void SystemManager :: actionUpdateRampDown(){
	switch (presentDir){
	case 0:
		binspeed = zeroSpeed+profileSpeed-(profileSpeed*o*controlTime)/rampTime;
		break;
	case 1:
		binspeed = zeroSpeed-profileSpeed+(profileSpeed*o*controlTime)/rampTime;
		break;
	}
	writeAnalog (0, (int)binspeed);
	o++;
	return;
}
void SystemManager :: actionStopMotor(){ 
	binspeed=zeroSpeed;
	writeAnalog (0, (int)binspeed);
	return;
}
void SystemManager :: actionMoveSlow(){ 
	profileSpeed = zeroSpeed-slowMovementSpeed*((zeroSpeed+0.0)/2200.0);
	binspeed = profileSpeed;
	writeAnalog (0, (int)binspeed);
	return;
}
void SystemManager :: actionUpdateSlowMovement(){ 
	binspeed = profileSpeed;
	writeAnalog (0, (int)binspeed);
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

//Diagram6

bool SystemManager :: conditionU1s(){
	if (n < nmax) {
		return TRUE;
	}
	else return FALSE;
}

bool SystemManager :: conditionO1s(){
	if (n >= nmax) return TRUE;
	else return FALSE;
}

bool SystemManager :: conditionU6s(){
	if (m < mmax) return TRUE;
	else return FALSE;
}

bool SystemManager :: conditionO6s(){
	if (m >= mmax) return TRUE;
	else return FALSE;
}

