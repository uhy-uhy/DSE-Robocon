/**
 * �t�@�C���� : SelectMaxNumRoute.h
 *
 *
 **/

#ifndef SELECT_MAX_NUM_ROUTE
#define SELECT_MAX_NUM_ROUTE

#include "CheckPointStrategy.h"

// ���[�g�I�����̐�(magic number)
const int ROUTE_NUM = 5;

class SelectMaxNumRoute : public CheckPointStrategy {
private:
	// ���[�g�}�[�J�[�J�E���^�[
	int MarkerCount[ROUTE_NUM];
	///// �ǋL
	// ���s�����L�^
	int mMileage;
public:
	SelectMaxNumRoute();
	virtual ~SelectMaxNumRoute();
	
	virtual void mAttackCheckPoint(int *forward, int *turn, int color);
};

#endif
