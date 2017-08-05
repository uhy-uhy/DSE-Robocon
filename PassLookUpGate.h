/**
 * ファイル名 : PassLookUpGate.h
 * 
 * 概要 : ルックアップゲート攻略
 **/

#ifndef PASS_LOOK_UP_GATE
#define PASS_LOOK_UP_GATE

#include "CheckPointStrategy.h"

// 走行状態に応じた指定距離
static const int GATE_PASS_MILEAGE = 350;	// ゲート通過距離

// 前進値
static const int GATE_LOW_FORWARD = 10;		// 
static const int GATE_MIDDLE_FORWARD = 40;	// 

// 旋回値
static const int GATE_LOW_TURN = 10;		// 
static const int GATE_MIDDLE_TURN = 20;		// 

// テール角度
static const int GATE_TAIL_BACK_TILT_ANGLE = 68;	// 後傾テール角度
static const int GATE_TAIL_TRAVEL_ANGLE = 3;		// 走行テール角度

// テール調整用Pゲイン
static const float GATE_TAIL_P_GAIN = 0.4f;

class PassLookUpGate : public CheckPointStrategy {
public:
	PassLookUpGate();
	~PassLookUpGate();
	
	void mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam);
};

#endif
