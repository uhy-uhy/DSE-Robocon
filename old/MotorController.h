/** 
 * ƒtƒ@ƒCƒ‹–¼ : MotorController.h
 *
 *
 **/

#ifndef MOTOR_CONTROLLER
#define MOTOR_CONTROLLER

#include "ev3api.h"
#include "Port.h"
#include "Motor.h"

using namespace ev3api;

class MotorController {
private:
	Motor mLeftMotor(PORT_C);
	Motor mRightMotor(PORT_B);
	Motor mTailMotor(PORT_A);
public:
	MotorController();
	~MotorController();
	
	void mInit();
	void mSet(int *pwm);
	void mGetCount(int *motorParam);
};

#endif