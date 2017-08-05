/**
 * ƒtƒ@ƒCƒ‹–¼ : DoPerformance.h
 *
 *
 **/

#ifndef DO_PERFORMANCE
#define DO_PERFORMANCE

class DoPerformance : public CheckPointStrategy {
public:
	DoPerformance();
	~DoPerformance();
	
	void mAttackCheckPoint(int *forward, int *turn, int color);
};

#endif