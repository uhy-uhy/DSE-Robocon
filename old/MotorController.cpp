/** 
 * ファイル名 : MotorController.cpp
 *
 *
 **/

#include "MotorController.h"

/** 
 * コンストラクタ
 * 
 * 
 **/
MotorController::MotorController() {
}

/** 
 * デストラクタ
 * 
 * 
 **/
MotorController::~MotorController() {
}

/** 
 * モーター初期化
 * 
 * 
 **/
void MotorController::mInit() {
	mLeftMotor.reset();
	mRightMotor.reset();
	mTailMotor.reset();
}

/** 
 * モーター出力値設定
 * 
 * 
 **/
void MotorController::mSet(int *pwm) {
	mLeftMotor.setPWM(pwm[0]);
	mRightMotor.setPWM(pwm[1]);
	mTailMotor.setPWM(pwm[2]);
}

/** 
 * モーターエンコーダ値取得
 * 
 * 
 **/
void MotorController::mGetCount(int *pwm) {
	pwm[0] = mLeftMotor.getCount();
	pwm[1] = mRightMotor.getCount();
	pwm[2] = mTailMotor.getCount();
}
