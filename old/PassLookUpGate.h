/**
 * ƒtƒ@ƒCƒ‹–¼ : PassLookUpGate.h
 *
 *
 **/

#ifndef PASS_LOOK_UP_GATE
#define PASS_LOOK_UP_GATE

class PassLookUpGate : public CheckPointStrategy {
public:
	PassLookUpGate();
	~PassLookUpGate();
	
	virtual void mAttackCheckPoint(int *forward, int *turn, int color, bool existObject);
};

#endif