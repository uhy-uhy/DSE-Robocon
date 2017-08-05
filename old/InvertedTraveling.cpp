/** 
 * �t�@�C���� : InvertedTraveling.cpp
 *
 *
 **/

#include "InvertedTraveling.h"

/** 
 * �R���X�g���N�^
 * 
 * 
 **/
InvertedTraveling::InvertedTraveling() {
}

/** 
 * �f�X�g���N�^
 * 
 * 
 **/
InvertedTraveling::~InvertedTraveling() {
}

/** 
 * ���s��Ԃ𔻒肵�A���[�^�[�̏o�͂����肷��
 * 
 * 
 **/
void InvertedTraveling::mDecideBehavior(int *sensorParam, int *motorParam, int stageState, bool existObject) {
	// ���[���F���� 0 : WHITE, 1 : GRAY, 2 : BLACK
	int color = mLineMonitor.mDecideColor(sensorParam[2]);
	
	// �O��i�l
	signed char forward = FORWARD;
	// ����l
	signed char turn = TURN;
	
	// �|���U�q����API�ɓn���p�����[�^���擾����
	int volt;// �o�b�e���[�̓d��
	
	// STAGE ���ɂ�葖�s���@�𔻒�
	// STAGE1 �̏ꍇ
	if (stageState == 1) {
		///// ��قǌ���
		// P����ڕW�l
		// int target = (LIGHT_WHITE + LIGHT_BLACK)/2;
		
		// ���[���̐F�ɉ��������s��������
		// ���C���g���[�X�E�����m
		switch(color) {
		// ���[���̐F�����̏ꍇ
		case 0:
			// ���ɐ���
			turn = TURN;
			break;
		// ���[���̐F���D�F�̏ꍇ
		case 1:
			///// 
			// �}�[�J�[����̏����L�q
			break;
		// ���[���̐F�����̏ꍇ
		case 2:
			turn = -TURN;
			break;
		default:
			break;
		}
	}
//	// STAGE2 �̏ꍇ
//	} else if (stageState == 2) {
//		// ����ʃ}�[�J�[�ɂ��U�镑�����菈��
//		switch(mEventState) {
//		// ���[�g�I��
//		case 1:
//			mCheckPointStrategy[0]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// ���b�N�A�b�v�Q�[�g
//		case 2:
//			mCheckPointStrategy[1]->mAttackCheckPoint(&forward, &turn, color, existObject);
//			break;
//		// �p�t�H�[�}���X�X�e�[�W
//		case 3:
//			mCheckPointStrategy[2]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// ��{��
//		case 4:
//			mCheckPointStrategy[3]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// �A�[�`��
//		case 5:
//			mCheckPointStrategy[4]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		default:
//			break;
//		}
//		
//	}
	
	// �o�b�e���[�l���擾����
	volt = ev3_battery_voltage_mV();
	
	// �|���U�q����API���Ăяo���A
	// �|�����s���邽�߂̍��E���[�^�[�o�͒l�𓾂�
	balance_control(
		(float)forward,
		(float)turn,
		(float)sensorParam[3],
		(float)GYRO_OFFSET,
		(float)motorParam[0],
		(float)motorParam[1],
		(float)volt,
		(signed char*)&mPWM[0],
		(signed char*)&mPWM[1]);
}

/** 
 * ���[�^�[�̏o�͒l��ԋp����
 * 
 * 
 **/
void InvertedTraveling::mGetPWM(char *pwm) {
	pwm = mPWM;
}
