/**
 * ファイル名 : CheckPointStrategy.h
 * 
 * 概要 : 各難所攻略クラスのスーパークラス
 *        インターフェースとしての役割
 * 
 **/

#ifndef CHECK_POINT_STRATEGY
#define CHECK_POINT_STRATEGY

class CheckPointStrategy {
protected:
	int mMileage;		// イベント開始時からの走行距離(mm)
	int mTurningAngle;	// 旋回角度
	int mTravelState;	// イベント走行状態
public:
	CheckPointStrategy();
	virtual ~CheckPointStrategy();
	
	// 難所攻略
	virtual void mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam);
};

#endif
