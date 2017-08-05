/** 
 * �t�@�C���� : SensorController.h
 *
 *
 **/

#ifndef SENSOR_CONTROLLER
#define SENSOR_CONTROLLER

#include "ev3api.h"
#include "Port.h"
#include "TouchSensor.h"
#include "SonarSensor.h"
#include "ColorSensor.h"
#include "GyroSensor.h"

using namespace ev3api;

class SensorController {
private:
	TouchSensor mTouchSensor(PORT_1);
	SonarSensor mSonarSensor(PORT_2);
	ColorSensor mColorSensor(PORT_3);
	GyroSensor mGyroSensor(PORT_4);
public:
	SensorController();
	~SensorController();
	
	void mInit();					// ����������̂̓W���C���Z���T�[�̂�
	void mGet(int *sensorParam);
	void mSet(const int offset);	// �Z�b�g����̂̓W���C���Z���T�[�̃I�t�Z�b�g�l�����Ȃ�
	bool mListenObject();
};

#endif