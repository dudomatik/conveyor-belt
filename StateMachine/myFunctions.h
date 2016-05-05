
#ifndef MYFUNCTIONS_H_
#define MYFUNCTIONS_H_

//Diagram0
void myActionDirLeft();
void myActionDirRight();
void myActionSpeedInc();
void myActionSpeedDec();
void myActionStartMovingLocal();
void myActionLeaveStateMovingLocal();
void myActionDoNothing();
void myActionStartCOM();
void myActionStayLOM();

//Diagram1
void myActionModeLom();
void myActionModeCom();
void myActionRequestUpdate();

//Diagram2
void myActionKeyboard();

//Diagram3
void myActionRunCustomProfile();
void myActionCustomProfileDone();
void myActionRunStandardProfile();
void myActionStandardProfileDone();
void myActionStartSlowMovement();
void myActionStopSlowMovement();

//Diagram4
void myActionStartLOM();
void myActionStayCOM();
void myActionRequest();

//Diagram5
void myActionStartSlowMovement1();
void myActionStartStandardProfile();
void myActionStopStandardProfile();
void myActionStartSlowMovement2();
void myActionMovementDone();
void myActionGetWait();

//Diagram 6
void myActionStartRampUp();
void myActionUpdateRampUp();
void myActionStartConstantMovement();
void myActionUpdateConstantMovement();
void myActionStartRampDown();
void myActionUpdateRampDown();
void myActionStopMotor();
void myActionMoveSlow();
void myActionUpdateSlowMovement();

//Conditions
bool myConditionTrue();

//Diagram0
bool myConditionCOM();
bool myConditionLOM();
bool myConditionCOMRequest0();
bool myConditionCOMRequest1();

//Diagram6
bool myConditionU1s();
bool myConditionO1s();
bool myConditionU6s();
bool myConditionO6s();

#endif // MYFUNCTIONS_H_
