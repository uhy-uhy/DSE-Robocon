/**
 * �t�@�C���� : DoPerformance.cpp
 *
 *
 **/

#include "DoPerformance.h"

/**
 * �R���X�g���N�^
 * 
 *
 **/
DoPerformance::DoPerformance() {
}

/**
 * �f�X�g���N�^
 * 
 *
 **/
DoPerformance::~DoPerformance() {
}

/**
 * �p�t�H�[�}���X�X�e�[�W
 * 
 *
 **/
void DoPerformance::mAttackCheckPoint(int *forward, int *turn, int color) {
	/************
	�`�F�b�N�|�C���g��Ԃ�c������ϐ����K�v
	�b�� : state
	************/
	
	/************
	�ᑬ�̐ݒ肪�K�v
	�i�b�� : LOW_SPEED�j
	************/
	// �}�[�J�[�����m�����̂��A�ᑬ���s�ɐ؂�ւ���
	if (state == 0) {
		forward = LOW_SPEED;
		
		/************
		��������p�̕ϐ��������̓��\�b�h���K�v
		�b�� : mMileage�i���s�����j
		************/
		if (mMileage > FIRST_MILEAGE) {
			state = 1;
		}
	// �p�t�H�[�}���X���s��
	} else if (state == 2) {
		/************
		�e�[���p�x�w��p�̃��\�b�h���K�v
		�b�� : control_tail, TAIL_ANGLE_PERFORMANCE = �p�t�H�[�}���X�p�e�[���p�x
		************/
		// �e�[�����o�����X�ێ��p�x�ɐݒ肷��
		control_tail(TAIL_ANGLE_PERFORMANCE);
		
		// ���~����
		forward = 0;
		turn = 0;
		
		
		
	// �p�t�H�[�}���X�I����A���C���g���[�X���s��
	}
}