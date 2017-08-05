/** 
 * �t�@�C���� : MotorController.cpp
 *
 *
 **/

#include "MotorController.h"

/** 
 * �R���X�g���N�^
 * 
 * 
 **/
MotorController::MotorController() {
}

/** 
 * �f�X�g���N�^
 * 
 * 
 **/
MotorController::~MotorController() {
}

/** 
 * ���[�^�[������
 * 
 * 
 **/
void MotorController::mInit() {
	mLeftMotor.reset();
	mRightMotor.reset();
	mTailMotor.reset();
}

/** 
 * ���[�^�[�o�͒l�ݒ�
 * 
 * 
 **/
void MotorController::mSet(int *pwm) {
	mLeftMotor.setPWM(pwm[0]);
	mRightMotor.setPWM(pwm[1]);
	mTailMotor.setPWM(pwm[2]);
}

/** 
 * ���[�^�[�G���R�[�_�l�擾
 * 
 * 
 **/
void MotorController::mGetCount(int *pwm) {
	pwm[0] = mLeftMotor.getCount();
	pwm[1] = mRightMotor.getCount();
	pwm[2] = mTailMotor.getCount();
}
