/**
 * �t�@�C���� : SelectMaxNumRoute.cpp
 *
 *
 **/

#include "SelectMaxNumRoute.h"

/**
 * �R���X�g���N�^
 * 
 * 
 **/
SelectMaxNumRoute::SelectMaxNumRoute() {
	mMileage = 0;
}

/**
 * �f�X�g���N�^
 * 
 * 
 **/
SelectMaxNumRoute::~SelectMaxNumRoute() {
}

/**
 * ���[�g�I��
 * 
 * 
 **/
SelectMaxNumRoute::mAttackCheckPoint(int *forward, int *turn, int color) {
	// �}�[�J�[�𔲂���܂Œᑬ���s
	if(mMileage < 75) {
		forward = FORWARD * 0.5;
		
		// ���[���̐F�ɉ��������s��������
		switch(color) {
		// ���[���̐F�����̏ꍇ
		case 0:
			// �E�ɐ���
			turn = TURN;
			break;
		// ���[���̐F���D�F�̏ꍇ
		case 1:
		// ���[���̐F�����̏ꍇ
		case 2:
			// ���ɐ���
			turn = TURN;
			break;
		default:
			break;
		}
	} else if(75 <= mMileage && mMileage < 300) {
		
	}
		
}
