
#ifndef SYSTEMMANAGER_H_
#define SYSTEMMANAGER_H_

/*
#include <queue>
#include <semLib.h>

#include "diaTimer.h"
#include "stateTable.h"
*/

class SystemManager {
public:
	SystemManager();
	~SystemManager();
	//Diagram0
	void actionDirLeft();
	void actionDirRight();
	void actionSpeedDec();
	void actionSpeedInc();
	void actionStartMovingLocal();	
	void actionLeaveStateMovingLocal();
	void actionDoNothing();
	void actionStartCOM();
	void actionStayLOM();
	
	//Diagram1
	void actionModeLom();
	void actionModeCom();
	void actionRequestUpdate();
	
	//Diagram2
	void actionKeyboard();
	
	//Diagram3
	void actionCustomProfileDone();
	void actionRunCustomProfile();
	void actionRunStandardProfile();
	void actionStandardProfileDone();
	void actionStartSlowMovement();
	void actionStopSlowMovement();

	
	//Diagram4
	void actionStartLOM();
	void actionStayCOM();
	void actionRequest();
	
	//Diagram5
	void actionStartSlowMovement1();
	void actionStartStandardProfile();
	void actionStopStandardProfile();
	void actionStartSlowMovement2();
	void actionMovementDone();
	//Conditions
	bool conditionTrue();
	
	//Diagram0
	bool conditionCOM();
	bool conditionLOM();
	bool conditionCOMRequest0();
	bool conditionCOMRequest1();
	
private:
	
};

#endif // SYSTEMMANAGER_H_
