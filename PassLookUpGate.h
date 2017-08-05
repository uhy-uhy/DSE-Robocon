/**
 * �t�@�C���� : PassLookUpGate.h
 * 
 * �T�v : ���b�N�A�b�v�Q�[�g�U��
 **/

#ifndef PASS_LOOK_UP_GATE
#define PASS_LOOK_UP_GATE

#include "CheckPointStrategy.h"

// ���s��Ԃɉ������w�苗��
static const int GATE_PASS_MILEAGE = 350;	// �Q�[�g�ʉߋ���

// �O�i�l
static const int GATE_LOW_FORWARD = 10;		// 
static const int GATE_MIDDLE_FORWARD = 40;	// 

// ����l
static const int GATE_LOW_TURN = 10;		// 
static const int GATE_MIDDLE_TURN = 20;		// 

// �e�[���p�x
static const int GATE_TAIL_BACK_TILT_ANGLE = 68;	// ��X�e�[���p�x
static const int GATE_TAIL_TRAVEL_ANGLE = 3;		// ���s�e�[���p�x

// �e�[�������pP�Q�C��
static const float GATE_TAIL_P_GAIN = 0.4f;

class PassLookUpGate : public CheckPointStrategy {
public:
	PassLookUpGate();
	~PassLookUpGate();
	
	void mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam);
};

#endif
