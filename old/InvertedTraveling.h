/** 
 * ファイル名 : InvertedTraveling.h
 *
 *
 **/

#ifndef INVERTED_TRAVELING
#define INVERTED_TRAVELING

#include "ev3api.h"
#include "balancer.h"
#include "LineMonitor.h"
// #include "CheckPointStrategy.h"

// *** 暫定 *** //
// 光センサ値
static const int LIGHT_WHITE = 60;
static const int LIGHT_GRAY = 20;
static const int LIGHT_BLACK = 0;

// モーター出力値設定
static const int FORWARD = 30;
static const int TURN = 30;
static const float P_GAIN = 0.38;
// *** 暫定 *** //

class InvertedTraveling {
private:
	char* mPWM;					// モーター出力値
	int mEventState;			// 難所識別用マーカー
	LineMonitor mLineMonitor;
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
	
	void mDeciedeBehavior(int* senSorParam, int* motorParam, int stageState, bool existObject);
	void mGetPWM(char *pwm);
};

#endif
