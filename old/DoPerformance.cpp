/**
 * ファイル名 : DoPerformance.cpp
 *
 *
 **/

#include "DoPerformance.h"

/**
 * コンストラクタ
 * 
 *
 **/
DoPerformance::DoPerformance() {
}

/**
 * デストラクタ
 * 
 *
 **/
DoPerformance::~DoPerformance() {
}

/**
 * パフォーマンスステージ
 * 
 *
 **/
void DoPerformance::mAttackCheckPoint(int *forward, int *turn, int color) {
	/************
	チェックポイント状態を把握する変数が必要
	暫定 : state
	************/
	
	/************
	低速の設定が必要
	（暫定 : LOW_SPEED）
	************/
	// マーカーを検知したのち、低速走行に切り替える
	if (state == 0) {
		forward = LOW_SPEED;
		
		/************
		距離測定用の変数もしくはメソッドが必要
		暫定 : mMileage（走行距離）
		************/
		if (mMileage > FIRST_MILEAGE) {
			state = 1;
		}
	// パフォーマンスを行う
	} else if (state == 2) {
		/************
		テール角度指定用のメソッドが必要
		暫定 : control_tail, TAIL_ANGLE_PERFORMANCE = パフォーマンス用テール角度
		************/
		// テールをバランス維持角度に設定する
		control_tail(TAIL_ANGLE_PERFORMANCE);
		
		// 制止する
		forward = 0;
		turn = 0;
		
		
		
	// パフォーマンス終了後、ライントレースを行う
	}
}