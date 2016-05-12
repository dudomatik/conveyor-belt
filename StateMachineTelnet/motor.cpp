#include <stdio.h>
#include <sysLib.h>
#include <stdLib.h>
#include "motor.h"

#include "systemManager.h"
#include "stateMachine.h"



extern "C" 
{
#include "hardware/hwFunc.h"
}
//StateMachine * myStateMachine;
SystemManager * mySystemManager;

#define controlTime 50
#define profileTime 8000.0
#define rampTime 1000.0
#define slowTime 1000.0
#define zeroSpeed 2610.0
#define standardProfileSpeed 1800.0
#define slowMovementSpeed 200.0



int m = 0;
int n = 0;
int o = 0;
double profileSpeed;
//int speed;
//int dir;


double binspeed; 
bool presentDir;
bool presMode;
//bool presentMode = 0;
int nmax = rampTime/controlTime;
int mmax = (profileTime - 2*rampTime)/controlTime;


Motor :: Motor() {
	printf("Motor Konstruktor!\n\r");	
	return;
}

Motor :: ~Motor() {
	
	return;
}
/*
void Motor :: setParent(SystemManager * passedSystemManager){
	mySystemManager = passedSystemManager;
}
*/
void Motor :: actionStartRampUp(){ 
	presMode = mySystemManager->getPresentMode();
	switch (presMode){
	case 0:
		switch (mySystemManager->getDir()){
		case 1:
			profileSpeed = mySystemManager->getSpeed()*(zeroSpeed/2200.0); // binary speed from zero point right
			break;
		case 0:
			profileSpeed = mySystemManager->getSpeed()*((4095.0-zeroSpeed)/2200); // binary speed from zero point left
			break;
		}
		presentDir = mySystemManager->getDir(); //for ramp calculation
		break;
	case 1:
		profileSpeed = standardProfileSpeed*(zeroSpeed/2200.0); // right
		presentDir = 1;
		break;
	}
	//turn Motor on without movement
	binspeed=zeroSpeed;
	writeAnalog (0, (int)binspeed);
	printf("presentMode = %d \n",mySystemManager->getPresentMode());
	printf("Profile Speed = %f \n",profileSpeed);
	motorOn();
	n=0;
	//sprintf(output,"Motor: Custom Profile   ");
	//writeToDisplay (20, 2, output );
	return;
}

void Motor :: actionUpdateRampUp(){ 
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

void Motor :: actionStartConstantMovement(){ 
	m=0;
	return;
}

void Motor :: actionUpdateConstantMovement(){ 
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

void Motor :: actionStartRampDown(){ 
	o=0;
	return;
}

void Motor :: actionUpdateRampDown(){
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

void Motor :: actionStopMotor(){ 
	binspeed=zeroSpeed;
	writeAnalog (0, (int)binspeed);
	motorOff();
	return;
}
void Motor :: actionMoveSlow(){ 
	profileSpeed = 2610.0-200.0*((2610.0+0.0)/2200.0);
	binspeed = profileSpeed;
	writeAnalog (0, (int)binspeed);
	motorOn();
	return;
}
void Motor :: actionUpdateSlowMovement(){ 
	binspeed = profileSpeed;
	writeAnalog (0, (int)binspeed);
	return;
}

// Conditions

bool Motor :: conditionU1s(){
	if (n < nmax) {
		return TRUE;
	}
	else return FALSE;
}

bool Motor :: conditionO1s(){
	if (n >= nmax) return TRUE;
	else return FALSE;
}

bool Motor :: conditionU6s(){
	if (m < mmax) return TRUE;
	else return FALSE;
}

bool Motor :: conditionO6s(){
	if (m >= mmax) return TRUE;
	else return FALSE;
}
			

