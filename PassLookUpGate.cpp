/**
 * �t�@�C���� : PassLookUpGate.cpp
 *
 *
 **/

#include "PassLookUpGate.h"

/**
 * �R���X�g���N�^
 * 
 * 
 **/
PassLookUpGate::PassLookUpGate() {
}

/**
 * �f�X�g���N�^
 * 
 * 
 **/
PassLookUpGate::~PassLookUpGate() {
}

/**
 * ���b�N�A�b�v�Q�[�g�U��
 * 
 * 
 **/
void PassLookUpGate::mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam) {
	// ���s��������p�ϐ�
	int nowMileage;		// ���s�J�n���猻�݂܂ł̋���
	int moveMileage;	// ��p����J�n������̑��s����
	
	int nowTurningAngle;
	
	// �U���J�n���p�����^���L�^
	int leftMotorAngle = motorParam[0];
	int rightMotorAngle = motorParam[1];
	int tailAngle = motorParam[2];
	
	int RADIUS = 80;
	float PI = 3.14159;
	
	// �����_�̑��s����
	// ���E���[�^�G���R�[�_�l�̕��ς��擾
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;
	
	// ���s�������v�Z
	int mileage = RADIUS * 2 * PI * (aveAngle / 360);

	nowMileage = mileage;
	
	// �����_�̐���l
	nowTurningAngle = (rightMotorAngle - leftMotorAngle) * 0.23;	
	
	// ����J�n������̑��s����
	moveMileage = nowMileage - mMileage;
	
	// ���s��Ԃɉ����ĐU�镑��������
	switch (mTravelState) {
	// ��U���J�n��
	case 0:
		// �J�n�����s�������L�^
		mMileage = nowMileage;
		
		// �J�n������p�x���L�^
		mTurningAngle = nowTurningAngle;
		
		// ���s��Ԃ����̒i�K��
		mTravelState = 1;
		
		break;
	// �e�[������X���s�p�x�ɐݒ�
	case 1:
		{
		// �ᑬ���s
		*forward = GATE_LOW_FORWARD;
		*turn = GATE_LOW_TURN;
		
		// �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
		float tail_pwm = (float)(GATE_TAIL_BACK_TILT_ANGLE + 1 - tailAngle) * GATE_TAIL_P_GAIN; // ��ᐧ��
		// �e�[�����[�^�[�o�͒l��ݒ�
		motorParam[2] = tail_pwm;
		
		// �e�[������X���s�p�x�ɐݒ肵���ꍇ
		if (tailAngle < GATE_TAIL_BACK_TILT_ANGLE) {
			mTravelState = 2;
			mTurningAngle = nowTurningAngle;
		}
		
		break;
		}
	// �w�苗�������O�i
	case 2:
	case 4:
		// ���i
		motorParam[0] = GATE_MIDDLE_FORWARD;
		motorParam[1] = GATE_MIDDLE_FORWARD;
		
		// �Q�[�g�𒴂����ꍇ
		if (moveMileage > GATE_PASS_MILEAGE) {
			// ����J�n�����X�V
			mMileage = nowMileage;
			// ��x�ڂ͌��
			mTravelState = (mTravelState == 2) ? 3 : 5;
		}
		
		break;
	// �A�[�`���ʉߌ�
	case 3:
		// ���
		motorParam[0] = -GATE_MIDDLE_FORWARD;
		motorParam[1] = -GATE_MIDDLE_FORWARD;
		
		// �w�苗���߂����ꍇ
		if (moveMileage < -GATE_PASS_MILEAGE) {
			// ����J�n�����X�V
			mMileage = nowMileage;
			
			// ���s��Ԃ��X�V
			mTravelState = 4;
		}
		
		break;
	// �e�[���𑖍s�J�n�p�x�ɐݒ�
	case 5:
		{
		// �ᑬ���s
		*forward = GATE_LOW_FORWARD;
		*turn = GATE_LOW_TURN;
		
		// �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
		float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // ��ᐧ��
		// �e�[�����[�^�[�o�͒l��ݒ�
		motorParam[2] = tail_pwm;
		
		// �e�[���𑖍s�p�x�ɐݒ肵���ꍇ
		if (tailAngle < GATE_TAIL_TRAVEL_ANGLE + 3) {
			// ���s��Ԃ��X�V
			mTravelState = 6;
		}
		
		break;
		}
	// ���̓�ɐi��
	case 6:
		// �ʏ푖�s
		*forward = GATE_MIDDLE_FORWARD;
		*turn = GATE_MIDDLE_TURN;
		
		break;
	default:
		*forward = GATE_MIDDLE_FORWARD;
		*turn = GATE_MIDDLE_TURN;
		break;
	}
}
