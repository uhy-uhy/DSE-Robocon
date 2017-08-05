/** 
 * ファイル名 : SensorController.cpp
 *
 *
 **/

#include "SensorController.h"

/** 
 * コンストラクタ
 *
 *
 **/
SensorController::SensorController() {
}

/** 
 * デストラクタ
 *
 *
 **/
SensorController::~SensorController() {
}

/** 
 * センサー初期化
 *
 *
 **/
void SensorController::mInit() {
	// ジャイロセンサー初期化
	mGyroSensor.reset();
}

/** 
 * センサー情報取得
 *
 *
 **/
void SensorController::mGet(int *sensorParam) {
	// タッチセンサーの押下情報取得
	sensorParam[0] = mTouchSensor.isPressed() ? 1 : 0;
	
	// 超音波センサーの距離情報取得
	sensorParam[1] = mSonarSensor.getDistance();
	
	// カラーセンサーの反射光強度を取得
	sensorParam[2] = mColorSensor.getBrightness();
	
	// ジャイロセンサーの角速度取得
	sensorParam[3] = mGyroSensor.genAnglerVelocity();
}

/** 
 * センサー情報設定
 *
 *
 **/
void SensorController::mSet(const int offset) {
	// ジャイロセンサーの角速度に対するオフセット値を設定する
	mGyroSensor.setOffset(offset);
}

/** 
 * 超音波センサーによる障害物検知
 *
 *
 **/
bool SensorController::mListenObject() {
	return mSonarSensor.listen();
}
