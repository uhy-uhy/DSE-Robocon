/** 
 * ファイル名 : MainTask.cpp
 *
 *
 **/

#include "MainTask.h"

/** 
 * コンストラクタ
 *
 *
 **/
MainTask::MainTask() {	
}

/** 
 * デストラクタ
 *
 *
 **/
MainTask::~MainTask() {
}

/** 
 * 初期化処理
 *
 *
 **/
void MainTask::mInit() {
	// センサー情報初期化
	mSensorController.mInit();
	
	// モーター情報初期化
	mMotorController.mInit();
	
	// バランサー初期化
	balance_init();
	
	// ステージ情報設定（現行 STAGE 1）
	mStageState = 1;
}

/** 
 * スタートメッセージチェック
 *
 *
 **/
bool MainTask::mCheckStart(){
	char* bmsg = mBluetoothCommunicator.mReceiveMessage();
	mSensorParam = mSensorController.mGet();

	// Bluetooth からスタートメッセージを受信した場合
	if (strcmp(bmsg, START_MESSAGE) == 0) {
		return true;
	// Touch センサーが押下された場合
	} else if (mSensorParam[0]) {
		return true;
	}
	
	// スタートメッセージを検出しなかった場合
	return false;
}

/** 
 * 倒立走行
 *
 *
 **/
void MainTask::mTraveling() {
	// センサー情報取得
	mSensorParam = mSensorController.mGet();
	
	// モーター情報取得
	mMotorParam = mGetCount();
	
	// モーター出力値決定
	mInvertedTraveling.mDecideBehavior(
				mSensorParam, mMotorParam, mSensorController.mListenObject, mStageState
				);
	
	// モーター出力値設定
	mMotorController.mSet((int)mInvertedTraveling.mGetPWM);
}
