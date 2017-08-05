/** 
 * ファイル名 : BluetoothCommunicator.h
 *
 *
 **/

#ifndef BLUETOOTH_COMMUNICATOR
#define BLUETOOTH_COMMUNICATOR

#include "ev3api.h"
#include <syssvc/serial.h>

// メッセージ長
#define MESSAGE_LEN 8

class BluetoothCommunicator {
public:
	BluetoothCommunicator();
	~BluetoothCommunicator();
	
	void mReceiveMessage(char *message);
	void mSendMessage(char* message);
};

#endif
