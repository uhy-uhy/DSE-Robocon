/**
 * t@C¼ : PassLookUpGate.h
 * 
 * Tv : bNAbvQ[gUª
 **/

#ifndef PASS_LOOK_UP_GATE
#define PASS_LOOK_UP_GATE

#include "CheckPointStrategy.h"

// sóÔÉ¶½wè£
static const int GATE_PASS_MILEAGE = 350;	// Q[gÊß£

// Oil
static const int GATE_LOW_FORWARD = 10;		// 
static const int GATE_MIDDLE_FORWARD = 40;	// 

// ùñl
static const int GATE_LOW_TURN = 10;		// 
static const int GATE_MIDDLE_TURN = 20;		// 

// e[px
static const int GATE_TAIL_BACK_TILT_ANGLE = 68;	// ãXe[px
static const int GATE_TAIL_TRAVEL_ANGLE = 3;		// se[px

// e[²®pPQC
static const float GATE_TAIL_P_GAIN = 0.4f;

class PassLookUpGate : public CheckPointStrategy {
public:
	PassLookUpGate();
	~PassLookUpGate();
	
	void mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam);
};

#endif
