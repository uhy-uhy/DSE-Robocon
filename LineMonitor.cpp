/** 
 * ファイル名 : LineMonitor.cpp
 *
 *
 **/

#include "LineMonitor.h"

/**
 * コンストラクタ
 * 
 * 
 **/
LineMonitor::LineMonitor() {
	// ログデータ初期化
	for (int i = 0; i < LOG_SIZE; i++) {
		mColorLog[i] = 0;
	}
}

/**
 * デストラクタ
 * 
 * 
 **/
LineMonitor::~LineMonitor() {
}

/**
 * レーン色判定
 * 
 * 
 **/
int LineMonitor::mDecideColor(int lightParam) {
	// 判定したレーンの色
	int laneColor = 0;
	
	// ログの灰色割合
	int GrayPer = 0;
	
	// ログデータ更新
	for (int i = 0; i < LOG_SIZE - 1; i++) {
		mColorLog[i] = mColorLog[i + 1];
		// 黒と灰色の閾値より大きく白と灰色の閾値未満のとき
		// 灰色のログをインクリメントする
		if (BGTH < mColorLog[i] && mColorLog[i] < GWTH) {
			++GrayPer;
		}
	}
	
	// 最新データ記録
	mColorLog[LOG_SIZE - 1] = lightParam;
	// 黒と灰色の閾値より大きく白と灰色の閾値未満のとき
	// 灰色のログをインクリメントする
	if (BGTH < lightParam && lightParam < GWTH) {
		++GrayPer;
	}
	
	// ログの灰色値の割合が閾値より大きい場合
	if (GrayPer >= GTH) {
		// レーン色を灰色と判定
		laneColor = 1;
	} else {
		// 光センサー値が白と黒の閾値より大きい場合
		if (lightParam > BWTH) {
			// レーン色を白と判定
			laneColor = 2;
		// 光センサー値が白と黒の閾値以下場合
		} else {
			// レーン色を黒と判定
			laneColor = 0;
		}
	}
	
	// 判定したレーン色を返却
	return laneColor;
}