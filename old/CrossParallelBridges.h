/**
 * ƒtƒ@ƒCƒ‹–¼ : CrossParallelBridges.h
 *
 *
 **/

#ifndef CROSS_PARALLEL_BRIDGES
#define CROSS_PARALLEL_BRIDGES

class CrossParallelBridges : public CheckPointStrategy {
public:
	CrossParallelBridges();
	~CrossParallelBridges();
	
	void mAttackCheckPoint(int *forward, int *turn, int color);
};

#endif
