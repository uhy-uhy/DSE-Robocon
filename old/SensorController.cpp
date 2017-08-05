/** 
 * �t�@�C���� : SensorController.cpp
 *
 *
 **/

#include "SensorController.h"

/** 
 * �R���X�g���N�^
 *
 *
 **/
SensorController::SensorController() {
}

/** 
 * �f�X�g���N�^
 *
 *
 **/
SensorController::~SensorController() {
}

/** 
 * �Z���T�[������
 *
 *
 **/
void SensorController::mInit() {
	// �W���C���Z���T�[������
	mGyroSensor.reset();
}

/** 
 * �Z���T�[���擾
 *
 *
 **/
void SensorController::mGet(int *sensorParam) {
	// �^�b�`�Z���T�[�̉������擾
	sensorParam[0] = mTouchSensor.isPressed() ? 1 : 0;
	
	// �����g�Z���T�[�̋������擾
	sensorParam[1] = mSonarSensor.getDistance();
	
	// �J���[�Z���T�[�̔��ˌ����x���擾
	sensorParam[2] = mColorSensor.getBrightness();
	
	// �W���C���Z���T�[�̊p���x�擾
	sensorParam[3] = mGyroSensor.genAnglerVelocity();
}

/** 
 * �Z���T�[���ݒ�
 *
 *
 **/
void SensorController::mSet(const int offset) {
	// �W���C���Z���T�[�̊p���x�ɑ΂���I�t�Z�b�g�l��ݒ肷��
	mGyroSensor.setOffset(offset);
}

/** 
 * �����g�Z���T�[�ɂ���Q�����m
 *
 *
 **/
bool SensorController::mListenObject() {
	return mSonarSensor.listen();
}
