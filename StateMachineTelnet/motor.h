#ifndef MOTOR_H_
#define MOTOR_H_
#define controlTime 50
#define profileTime 8000.0
#define rampTime 1000.0
#define slowTime 1000.0
#define zeroSpeed 2610.0
#define standardProfileSpeed 1800.0
#define slowMovementSpeed 200.0

#include "systemManager.h"

class Motor
{
	
public:
	Motor();
	~Motor();
	

	int m;
	int n;
	int o;
	double profileSpeed;
	int Speed;
	
	void setParent(SystemManager * passedSystemManager);
	void actionStartRampUp();
	void actionUpdateRampUp();
	void actionStartConstantMovement();
	void actionUpdateConstantMovement();
	void actionStartRampDown();
	void actionUpdateRampDown();
	void actionStopMotor();
	void actionMoveSlow();
	void actionUpdateSlowMovement();
	
	//Conditions
	bool conditionU1s();
	bool conditionO1s();
	bool conditionO6s();
	bool conditionU6s();
	
private:
	/*
	int m;
	int n;
	int o;
	double profileSpeed;

	double binspeed; 
	bool presentDir;
	bool presMode;
	//bool presentMode = 0;
	int nmax;
	int mmax;*/
	
};




#endif // MOTOR_H_
