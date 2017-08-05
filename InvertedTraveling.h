/** 
 * ファイル名 : InvertedTraveling.h
 * 
 * 概要 : 倒立走行機能
 *
 **/

#ifndef INVERTED_TRAVELING
#define INVERTED_TRAVELING

#include "ev3api.h"
#include "balancer.h"
#include "LineMonitor.h"
// #include "CheckPointStrategy.h"

// 円周率
#define PI (3.1415926536)

// 走行体パラメータ 単位 : mm
static const int RADIUS = 80;			// タイヤ半径
static const int TREAD = 160;			// 車輪間距離

///// *** 暫定 *** /////
// 光センサ値
static const int LIGHT_WHITE = 60;		// 白色反射光
static const int LIGHT_GRAY = 20;		// 灰色反射光
static const int LIGHT_BLACK = 0;		// 黒色反射光

// モーター出力値設定
static const int FORWARD = 30;			// 前進値
static const int TURN = 30;				// 旋回値（正：右旋回、負：左旋回）
static const float P_GAIN = 0.38;		// P制御係数

// 左右エンコーダ値差/度 （実測値から算出）
static const float TURNING_ANGLE_UNIT = 0.23;
///// *** 暫定 *** /////

static const int GYRO_OFFSET = 0;

class InvertedTraveling {
private:
	char mPWM[3];				// モーター出力値
	int mEventState;			// 難所識別用マーカー
	LineMonitor* mLineMonitor;
//	CheckPointStrategy* mCheckPointStrategy[5] = {
//		new SelectMaxNumRoute,
//		new PassLookUpGate,
//		new DoPerformance,
//		new CrossParallelBridges,
//		new CrossArchBridge
//	};
public:
	InvertedTraveling();
	~InvertedTraveling();
	
	void mDecideBehavior(int* senSorParam, int* motorParam, int stageState);	// モーター出力値決定
	void mGetPWM(char *pwm);		// モーター出力値返却
	
	static int mMeasureMileage(int leftMotorAngle, int rightMotorDegee);		// 走行距離計測
	static int mMeasureTurningAngle(int leftMotorAngle, int rightMotorDegee);	// 旋回角度計測
};

#endif
