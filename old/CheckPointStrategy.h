/**
 * ƒtƒ@ƒCƒ‹–¼ : CheckPointStrategy.h
 *
 *
 **/

#ifndef CHECK_POINT_STRATEGY
#define CHECK_POINT_STRATEGY

class CheckPointStrategy {
public:
	CheckPointStrategy();
	virtual ~CheckPointStrategy();
	
	virtual void mAttackCheckPoint(int *forward, int *turn, int color);
	virtual void mAttackCheckPoint(int *forward, int *turn, int color, bool existObject);
};

#endif
