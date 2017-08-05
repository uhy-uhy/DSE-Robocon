/** 
 * ファイル名 : BluetoothCommunicator.cpp
 *
 *
 **/

#include "BluetoothCommunicator.h"

/** 
 * コンストラクタ
 *
 *
 **/
BluetoothCommunicator::BluetoothCommunicator() {
}

/** 
 * デストラクタ
 *
 *
 **/
BluetoothCommunicator::~BluetoothCommunicator() {
}

/** 
 * メッセージ受信
 *
 *
 **/
void BluetoothCommunicator::mReceiveMessage(char *message) {
	// Bluetooth 仮想シリアルポートからmessege が指すバッファへ
	// 最大 MESSAGE_LEN バイトを読み込む
	serial_rea_dat(SIO_PORT_BT, message, MESSAGE_LEN);
}

/** 
 * メッセージ送信
 *
 *
 **/
void BluetoothCommunicator::mSendMessage(char* message) {
	// メッセージを受信した場合、messege が指すバッファから
	// Bluetooth 仮想シリアルポートへ最大 MESSAGE_LEN バイトを書き込む
	serial_wri_dat(SIO_PORT_BT, message, MESSAGE_LEN);
}
