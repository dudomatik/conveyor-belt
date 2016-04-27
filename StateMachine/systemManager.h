
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
	void actionDirLeft();
	void actionDirRight();
	void actionSpeedDec();
	void actionSpeedInc();
	void actionStart();	
	void actionLeaveStateMovingLocal();
	void actionDoNothing();
	void actionUpdate2();
	void actionModeLom();
	void actionModeCom();
	
	
	void action00();
	void action01();
	void action02();
	void action10();
	void action11();
	void action12();
	void action13();
	void action20();
	bool conditionTrue();
	bool condition00();
	bool condition01();
	bool condition11();
	bool condition12();

private:
	
};

#endif // SYSTEMMANAGER_H_
