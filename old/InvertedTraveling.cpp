/** 
 * ファイル名 : InvertedTraveling.cpp
 *
 *
 **/

#include "InvertedTraveling.h"

/** 
 * コンストラクタ
 * 
 * 
 **/
InvertedTraveling::InvertedTraveling() {
}

/** 
 * デストラクタ
 * 
 * 
 **/
InvertedTraveling::~InvertedTraveling() {
}

/** 
 * 走行状態を判定し、モーターの出力を決定する
 * 
 * 
 **/
void InvertedTraveling::mDecideBehavior(int *sensorParam, int *motorParam, int stageState, bool existObject) {
	// レーン色判定 0 : WHITE, 1 : GRAY, 2 : BLACK
	int color = mLineMonitor.mDecideColor(sensorParam[2]);
	
	// 前後進値
	signed char forward = FORWARD;
	// 旋回値
	signed char turn = TURN;
	
	// 倒立振子制御APIに渡すパラメータを取得する
	int volt;// バッテリーの電圧
	
	// STAGE 情報により走行方法を判定
	// STAGE1 の場合
	if (stageState == 1) {
		///// 後ほど検討
		// P制御目標値
		// int target = (LIGHT_WHITE + LIGHT_BLACK)/2;
		
		// レーンの色に応じた走行処理決定
		// ライントレース右側検知
		switch(color) {
		// レーンの色が白の場合
		case 0:
			// 左に旋回
			turn = TURN;
			break;
		// レーンの色が灰色の場合
		case 1:
			///// 
			// マーカー判定の処理記述
			break;
		// レーンの色が黒の場合
		case 2:
			turn = -TURN;
			break;
		default:
			break;
		}
	}
//	// STAGE2 の場合
//	} else if (stageState == 2) {
//		// 難所識別マーカーによる振る舞い決定処理
//		switch(mEventState) {
//		// ルート選択
//		case 1:
//			mCheckPointStrategy[0]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// ルックアップゲート
//		case 2:
//			mCheckPointStrategy[1]->mAttackCheckPoint(&forward, &turn, color, existObject);
//			break;
//		// パフォーマンスステージ
//		case 3:
//			mCheckPointStrategy[2]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// 二本橋
//		case 4:
//			mCheckPointStrategy[3]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// アーチ橋
//		case 5:
//			mCheckPointStrategy[4]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		default:
//			break;
//		}
//		
//	}
	
	// バッテリー値を取得する
	volt = ev3_battery_voltage_mV();
	
	// 倒立振子制御APIを呼び出し、
	// 倒立走行するための左右モーター出力値を得る
	balance_control(
		(float)forward,
		(float)turn,
		(float)sensorParam[3],
		(float)GYRO_OFFSET,
		(float)motorParam[0],
		(float)motorParam[1],
		(float)volt,
		(signed char*)&mPWM[0],
		(signed char*)&mPWM[1]);
}

/** 
 * モーターの出力値を返却する
 * 
 * 
 **/
void InvertedTraveling::mGetPWM(char *pwm) {
	pwm = mPWM;
}
