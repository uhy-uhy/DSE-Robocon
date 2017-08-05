/** 
 * �t�@�C���� : LineMonitor.cpp
 *
 *
 **/

#include "LineMonitor.h"

/**
 * �R���X�g���N�^
 * 
 * 
 **/
LineMonitor::LineMonitor() {
	// ���O�f�[�^������
	for (int i = 0; i < LOG_SIZE; i++) {
		mColorLog[i] = 0;
	}
}

/**
 * �f�X�g���N�^
 * 
 * 
 **/
LineMonitor::~LineMonitor() {
}

/**
 * ���[���F����
 * 
 * 
 **/
int LineMonitor::mDecideColor(int lightParam) {
	// ���肵�����[���̐F
	int laneColor = 0;
	
	// ���O�̊D�F����
	int GrayPer = 0;
	
	// ���O�f�[�^�X�V
	for (int i = 0; i < LOG_SIZE - 1; i++) {
		mColorLog[i] = mColorLog[i + 1];
		// ���ƊD�F��臒l���傫�����ƊD�F��臒l�����̂Ƃ�
		// �D�F�̃��O���C���N�������g����
		if (BGTH < mColorLog[i] && mColorLog[i] < GWTH) {
			++GrayPer;
		}
	}
	
	// �ŐV�f�[�^�L�^
	mColorLog[LOG_SIZE - 1] = lightParam;
	// ���ƊD�F��臒l���傫�����ƊD�F��臒l�����̂Ƃ�
	// �D�F�̃��O���C���N�������g����
	if (BGTH < lightParam && lightParam < GWTH) {
		++GrayPer;
	}
	
	// ���O�̊D�F�l�̊�����臒l���傫���ꍇ
	if (GrayPer >= GTH) {
		// ���[���F���D�F�Ɣ���
		laneColor = 1;
	} else {
		// ���Z���T�[�l�����ƍ���臒l���傫���ꍇ
		if (lightParam > BWTH) {
			// ���[���F�𔒂Ɣ���
			laneColor = 2;
		// ���Z���T�[�l�����ƍ���臒l�ȉ��ꍇ
		} else {
			// ���[���F�����Ɣ���
			laneColor = 0;
		}
	}
	
	// ���肵�����[���F��ԋp
	return laneColor;
}