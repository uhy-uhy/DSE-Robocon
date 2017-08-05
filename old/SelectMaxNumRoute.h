/**
 * ファイル名 : SelectMaxNumRoute.h
 *
 *
 **/

#ifndef SELECT_MAX_NUM_ROUTE
#define SELECT_MAX_NUM_ROUTE

#include "CheckPointStrategy.h"

// ルート選択肢の数(magic number)
const int ROUTE_NUM = 5;

class SelectMaxNumRoute : public CheckPointStrategy {
private:
	// ルートマーカーカウンター
	int MarkerCount[ROUTE_NUM];
	///// 追記
	// 走行距離記録
	int mMileage;
public:
	SelectMaxNumRoute();
	virtual ~SelectMaxNumRoute();
	
	virtual void mAttackCheckPoint(int *forward, int *turn, int color);
};

#endif
