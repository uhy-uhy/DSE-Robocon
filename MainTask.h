/** 
 * ファイル名 : MainTask.h
 *
 *
 **/

#ifndef MAINTASK
#define MAINTASK

#include <iostream>
#include <string.h>

#include "BluetoothCommunicator.h"
#include "DisplayController.h"
#include "SensorController.h"
#include "MotorController.h"
#include "InvertedTraveling.h"
#include "balancer.h"

// Bluetooth スタートメッセージ
#define START_MESSAGE

class MainTask {
private:
	int mSensorParam[];
	int mMotorParam[];
	int mStageState;
	
	BluetoothCommunicator mBluetoothCommunicator;
	DisplayController mDisplayController;
	SensorController mSensorController;
	MotorController mMotorController;
	InvertedTraveling mInvertedTraveling;
	
public:
	MainTask();
	~MainTask();

	void mInit();
	bool mCheckStart();
	void mTraveling();
};

#endif
