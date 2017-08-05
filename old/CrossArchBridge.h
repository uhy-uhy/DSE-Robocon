/**
 * ƒtƒ@ƒCƒ‹–¼ : CrossArchBridge.h
 *
 *
 **/

#ifndef CROSS_ARCH_BRIDGE
#define CROSS_ARCH_BRIDGE

class CrossArchBridge : public CheckPointStrategy {
public:
	CrossArchBridge();
	~CrossArchBridge();
	
	void mAttackCheckPoint(int *forward, int *turn, int color);
};

#endif
