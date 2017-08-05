/** 
 * �t�@�C���� : BluetoothCommunicator.cpp
 *
 *
 **/

#include "BluetoothCommunicator.h"

/** 
 * �R���X�g���N�^
 *
 *
 **/
BluetoothCommunicator::BluetoothCommunicator() {
}

/** 
 * �f�X�g���N�^
 *
 *
 **/
BluetoothCommunicator::~BluetoothCommunicator() {
}

/** 
 * ���b�Z�[�W��M
 *
 *
 **/
void BluetoothCommunicator::mReceiveMessage(char *message) {
	// Bluetooth ���z�V���A���|�[�g����messege ���w���o�b�t�@��
	// �ő� MESSAGE_LEN �o�C�g��ǂݍ���
	serial_rea_dat(SIO_PORT_BT, message, MESSAGE_LEN);
}

/** 
 * ���b�Z�[�W���M
 *
 *
 **/
void BluetoothCommunicator::mSendMessage(char* message) {
	// ���b�Z�[�W����M�����ꍇ�Amessege ���w���o�b�t�@����
	// Bluetooth ���z�V���A���|�[�g�֍ő� MESSAGE_LEN �o�C�g����������
	serial_wri_dat(SIO_PORT_BT, message, MESSAGE_LEN);
}
