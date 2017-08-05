/**
 * ファイル名 : PassLookUpGate.cpp
 *
 *
 **/

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
///// TODO : 走行体後傾時、光センサーによる検知不可
/////        超音波センサによるゲート検知
/////        前進
PassLookUpGate::mAttackCheckPoint(int *forward, int *turn, int color, bool existObject) {
	///// TODO : 処理の追記が必要
	// レーンの色が変わったことを検知（灰 -> 黒、黒 -> 灰）
	bool isChanged = false;
	// 状態検知用（メンバ変数の必要あり）
	int nowState = 0;
	///// : TODO
	
	// 現在の走行状態を判定する
	// ゲート検知
	if (existObject) {
		nowState = 1;
	// ゲート検知からゲート通過まで
	} else if (nowState == 1 && !existObject) {
		nowState = 2;
	// ゲート通過から、ゲート手前に戻るまで
	} else if (nowState == 2 && existObject) {
		nowState = 3;
	// ゲート2回目を通過するまで
	} else if (nowState = 3 && existObject) {
		nowState = 4;
	// ゲート2回目通過
	} else if (nowState = 4 && !existObject) {
		nowState = 5;
	}
	
	switch (nowState) {
	// マーカー検知～ゲート通過～マーカー検知まで
	case 0:
		///// tail コントロール用の{値 | メソッド}が必要
		///// pwm を引数にとるか
		///// control_tail というメソッドを想定
		// テールを後傾走行角度に設定 ///// （暫定）
		controll_tail(BACKWARD_TILTING);
		
		///// ジャイロセンサーオフセット値操作
		GYRO_OFFSET = BACK_TILT_GYRO;
		
		// 直進させる
		forward = FORWARD;
		turn = 0;
		break;
	case 1:
		
		break;
	case 2:
		
		break;
	case 3:
		
		break;
	case 4:
		
		break;
	case 5:
		
		break;
	default:
		break;
	}
}