#include <stdio.h>
#include <sysLib.h>
#include <stdLib.h>
#include "keyboard.h"

#include "systemManager.h"
#include "stateMachine.h"

extern "C" 
{
#include "hardware/hwFunc.h"
}
StateMachine * myStateMachine;
int keyCount;
char currentKey;


Keyboard :: Keyboard() {
	printf("Keyboard Konstruktor!\n\r");	
	return;
}

Keyboard :: ~Keyboard() {
	pressedKey = 0;
	return;
}


			
char Keyboard::getPressedKey( )
{
	
	pressedKey = getKey();
	if(pressedKey!=currentKey){
		currentKey=pressedKey;
		switch(pressedKey){
		case '1': printf("**Start Key Pressed \n"); //start
		myStateMachine->sendEvent("startKey");
		break;
		case '2': printf("**Direction: Left\n"); //left
		myStateMachine->sendEvent("leftKey");
		break;
		case '3': printf("**Direction: Right\n"); //rigth
		myStateMachine->sendEvent("rightKey");
		break;
		case '4': printf("**Speed Increased\n"); //increase
		myStateMachine->sendEvent("incKey");
		break;
		case '5': printf("**Speed Decreased\n"); //decrease
		myStateMachine->sendEvent("decKey");
		break;
		case 'A': printf("**Local Operation Mode selected\n"); //Local op
		myStateMachine->sendEvent("modeLomKey");
		break;
		case 'B': printf("**Chain Operation Mode selected\n"); //chain op
		myStateMachine->sendEvent("modeComKey");
		break;
		 
		
		}
	}
	//printf(".");
	return pressedKey;
}
