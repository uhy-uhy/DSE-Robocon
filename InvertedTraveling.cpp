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
InvertedTraveling::InvertedTraveling() : mPWM(), mEventState() {
	mLineMonitor = new LineMonitor();
}

/** 
 * デストラクタ
 * 
 * 
 **/
InvertedTraveling::~InvertedTraveling() {
	delete mLineMonitor;
}

/** 
 * 走行状態を判定し、モーターの出力を決定する
 * 
 * 
 **/
void InvertedTraveling::mDecideBehavior(int *sensorParam, int *motorParam, int stageState) {
	// レーン色判定 0 : WHITE, 1 : GRAY, 2 : BLACK
	
	int color = mLineMonitor->mDecideColor(sensorParam[2]);
	
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
	
	///// print debug
//	char msg[32];
//	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
//	ev3_lcd_draw_string("before balancer", 0, 8*1);
//	sprintf(msg, "Gyro Sensor : %d", sensorParam[3]);
//	const char* mg = msg;
//	ev3_lcd_draw_string(mg, 0, 8*2);
//	sprintf(msg, "motorParam 0 : %d", motorParam[0]);
//	const char* mp0 = msg;
//	ev3_lcd_draw_string(mp0, 0, 8*3);
//	sprintf(msg, "motorParam 1 : %d", motorParam[1]);
//	const char* mp1 = msg;
//	ev3_lcd_draw_string(mp1, 0, 8*4);
//	sprintf(msg, "mPWM 0 : %d", mPWM[0]);
//	const char* m0 = msg;
//	ev3_lcd_draw_string(m0, 0, 8*5);
//	sprintf(msg, "mPWM 1 : %d", mPWM[1]);
//	const char* m1 = msg;
//	ev3_lcd_draw_string(m1, 0, 8*6);
//	sprintf(msg, "mPWM 2 : %d", mPWM[2]);
//	const char* m2 = msg;
//	ev3_lcd_draw_string(m2, 0, 8*7);
//	tslp_tsk(3000);
	
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

//	///// print debug
//	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
//	ev3_lcd_draw_string("after balancer", 0, 8*1);
//	sprintf(msg, "Gyro Sensor : %d", sensorParam[3]);
//	const char* amg = msg;
//	ev3_lcd_draw_string(amg, 0, 8*2);
//	sprintf(msg, "motorParam 0 : %d", motorParam[0]);
//	const char* amp0 = msg;
//	ev3_lcd_draw_string(amp0, 0, 8*3);
//	sprintf(msg, "motorParam 1 : %d", motorParam[1]);
//	const char* amp1 = msg;
//	ev3_lcd_draw_string(amp1, 0, 8*4);
//	sprintf(msg, "mPWM 0 : %d", mPWM[0]);
//	const char* am0 = msg;
//	ev3_lcd_draw_string(am0, 0, 8*5);
//	sprintf(msg, "mPWM 1 : %d", mPWM[1]);
//	const char* am1 = msg;
//	ev3_lcd_draw_string(am1, 0, 8*6);
//	sprintf(msg, "mPWM 2 : %d", mPWM[2]);
//	const char* am2 = msg;
//	ev3_lcd_draw_string(am2, 0, 8*7);
//	tslp_tsk(3000);
}

/** 
 * モーターの出力値を返却する
 * 
 * 
 **/
void InvertedTraveling::mGetPWM(char *pwm) {
	pwm = mPWM;
}

/** 
 * 走行距離計測
 * 
 * 
 **/
int mMeasureMileage(int leftMotorAngle, int rightMotorAngle) {
	// 左右モータエンコーダ値の平均を取得
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;
	
	// 走行距離を計算
	int mileage = RADIUS * 2 * PI * (aveAngle / 360);
	
	return mileage;
}

/** 
 * 旋回角度計測
 * 
 * 
 **/
int mMeasureTurningAngle(int leftMotorAngle, int rightMotorAngle) {
	// 旋回角度計測（左回りが正）
	///// 計算式から求める場合
	///// 
	// int turningAngle = RADIUS * 2 * PI * (rightMotorDegee - leftMotorAngle) / TREAD;
	
	///// 実測値から求める場合
	int turningAngle = (rightMotorAngle - leftMotorAngle) * TURNING_ANGLE_UNIT;
	return turningAngle;
}
