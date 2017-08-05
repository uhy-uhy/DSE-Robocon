/**
 * ファイル名 : PassLookUpGate.cpp
 *
 *
 **/

#include "PassLookUpGate.h"

/**
 * コンストラクタ
 * 
 * 
 **/
PassLookUpGate::PassLookUpGate() {
}

/**
 * デストラクタ
 * 
 * 
 **/
PassLookUpGate::~PassLookUpGate() {
}

/**
 * ルックアップゲート攻略
 * 
 * 
 **/
void PassLookUpGate::mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam) {
	// 走行距離測定用変数
	int nowMileage;		// 走行開始から現在までの距離
	int moveMileage;	// 難所用測定開始時からの走行距離
	
	int nowTurningAngle;
	
	// 攻略開始時パラメタを記録
	int leftMotorAngle = motorParam[0];
	int rightMotorAngle = motorParam[1];
	int tailAngle = motorParam[2];
	
	int RADIUS = 80;
	float PI = 3.14159;
	
	// 現時点の走行距離
	// 左右モータエンコーダ値の平均を取得
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;
	
	// 走行距離を計算
	int mileage = RADIUS * 2 * PI * (aveAngle / 360);

	nowMileage = mileage;
	
	// 現時点の旋回値
	nowTurningAngle = (rightMotorAngle - leftMotorAngle) * 0.23;	
	
	// 測定開始時からの走行距離
	moveMileage = nowMileage - mMileage;
	
	// 走行状態に応じて振る舞いを決定
	switch (mTravelState) {
	// 難所攻略開始時
	case 0:
		// 開始時走行距離を記録
		mMileage = nowMileage;
		
		// 開始時旋回角度を記録
		mTurningAngle = nowTurningAngle;
		
		// 走行状態を次の段階へ
		mTravelState = 1;
		
		break;
	// テールを後傾走行角度に設定
	case 1:
		{
		// 低速走行
		*forward = GATE_LOW_FORWARD;
		*turn = GATE_LOW_TURN;
		
		// テールモーター出力値を目標値との差に応じて決定
		float tail_pwm = (float)(GATE_TAIL_BACK_TILT_ANGLE + 1 - tailAngle) * GATE_TAIL_P_GAIN; // 比例制御
		// テールモーター出力値を設定
		motorParam[2] = tail_pwm;
		
		// テールを後傾走行角度に設定した場合
		if (tailAngle < GATE_TAIL_BACK_TILT_ANGLE) {
			mTravelState = 2;
			mTurningAngle = nowTurningAngle;
		}
		
		break;
		}
	// 指定距離だけ前進
	case 2:
	case 4:
		// 直進
		motorParam[0] = GATE_MIDDLE_FORWARD;
		motorParam[1] = GATE_MIDDLE_FORWARD;
		
		// ゲートを超えた場合
		if (moveMileage > GATE_PASS_MILEAGE) {
			// 測定開始距離更新
			mMileage = nowMileage;
			// 一度目は後退
			mTravelState = (mTravelState == 2) ? 3 : 5;
		}
		
		break;
	// アーチ橋通過後
	case 3:
		// 後退
		motorParam[0] = -GATE_MIDDLE_FORWARD;
		motorParam[1] = -GATE_MIDDLE_FORWARD;
		
		// 指定距離戻った場合
		if (moveMileage < -GATE_PASS_MILEAGE) {
			// 測定開始距離更新
			mMileage = nowMileage;
			
			// 走行状態を更新
			mTravelState = 4;
		}
		
		break;
	// テールを走行開始角度に設定
	case 5:
		{
		// 低速走行
		*forward = GATE_LOW_FORWARD;
		*turn = GATE_LOW_TURN;
		
		// テールモーター出力値を目標値との差に応じて決定
		float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // 比例制御
		// テールモーター出力値を設定
		motorParam[2] = tail_pwm;
		
		// テールを走行角度に設定した場合
		if (tailAngle < GATE_TAIL_TRAVEL_ANGLE + 3) {
			// 走行状態を更新
			mTravelState = 6;
		}
		
		break;
		}
	// 次の難所に進む
	case 6:
		// 通常走行
		*forward = GATE_MIDDLE_FORWARD;
		*turn = GATE_MIDDLE_TURN;
		
		break;
	default:
		*forward = GATE_MIDDLE_FORWARD;
		*turn = GATE_MIDDLE_TURN;
		break;
	}
}
