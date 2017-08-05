/** 
 * �t�@�C���� : MainTask.cpp
 *
 *
 **/

#include "MainTask.h"

/** 
 * �R���X�g���N�^
 *
 *
 **/
MainTask::MainTask() {	
}

/** 
 * �f�X�g���N�^
 *
 *
 **/
MainTask::~MainTask() {
}

/** 
 * ����������
 *
 *
 **/
void MainTask::mInit() {
	// �Z���T�[��񏉊���
	mSensorController.mInit();
	
	// ���[�^�[��񏉊���
	mMotorController.mInit();
	
	// �o�����T�[������
	balance_init();
	
	// �X�e�[�W���ݒ�i���s STAGE 1�j
	mStageState = 1;
}

/** 
 * �X�^�[�g���b�Z�[�W�`�F�b�N
 *
 *
 **/
bool MainTask::mCheckStart(){
	char* bmsg = mBluetoothCommunicator.mReceiveMessage();
	mSensorParam = mSensorController.mGet();

	// Bluetooth ����X�^�[�g���b�Z�[�W����M�����ꍇ
	if (strcmp(bmsg, START_MESSAGE) == 0) {
		return true;
	// Touch �Z���T�[���������ꂽ�ꍇ
	} else if (mSensorParam[0]) {
		return true;
	}
	
	// �X�^�[�g���b�Z�[�W�����o���Ȃ������ꍇ
	return false;
}

/** 
 * �|�����s
 *
 *
 **/
void MainTask::mTraveling() {
	// �Z���T�[���擾
	mSensorParam = mSensorController.mGet();
	
	// ���[�^�[���擾
	mMotorParam = mGetCount();
	
	// ���[�^�[�o�͒l����
	mInvertedTraveling.mDecideBehavior(
				mSensorParam, mMotorParam, mSensorController.mListenObject, mStageState
				);
	
	// ���[�^�[�o�͒l�ݒ�
	mMotorController.mSet((int)mInvertedTraveling.mGetPWM);
}
