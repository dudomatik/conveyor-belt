
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
	void noAction();
	void setLocalModeTrue();
	void keyPressed();
	void action10();
	void action11();
	void action12();
	void action13();
	void action20();
	bool conditionTrue();
	bool noNewKey();
	bool keyB();
	bool condition11();
	bool condition12();

private:
	void storeKeyValue();
};

#endif // SYSTEMMANAGER_H_
