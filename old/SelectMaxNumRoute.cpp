/**
 * ファイル名 : SelectMaxNumRoute.cpp
 *
 *
 **/

#include "SelectMaxNumRoute.h"

/**
 * コンストラクタ
 * 
 * 
 **/
SelectMaxNumRoute::SelectMaxNumRoute() {
	mMileage = 0;
}

/**
 * デストラクタ
 * 
 * 
 **/
SelectMaxNumRoute::~SelectMaxNumRoute() {
}

/**
 * ルート選択
 * 
 * 
 **/
SelectMaxNumRoute::mAttackCheckPoint(int *forward, int *turn, int color) {
	// マーカーを抜けるまで低速走行
	if(mMileage < 75) {
		forward = FORWARD * 0.5;
		
		// レーンの色に応じた走行処理決定
		switch(color) {
		// レーンの色が白の場合
		case 0:
			// 右に旋回
			turn = TURN;
			break;
		// レーンの色が灰色の場合
		case 1:
		// レーンの色が黒の場合
		case 2:
			// 左に旋回
			turn = TURN;
			break;
		default:
			break;
		}
	} else if(75 <= mMileage && mMileage < 300) {
		
	}
		
}
