/**
 * �t�@�C���� : PassLookUpGate.cpp
 *
 *
 **/

/**
 * �R���X�g���N�^
 * 
 * 
 **/
PassLookUpGate::PassLookUpGate() {
}

/**
 * �f�X�g���N�^
 * 
 * 
 **/
PassLookUpGate::~PassLookUpGate() {
}

/**
 * ���b�N�A�b�v�Q�[�g�U��
 * 
 * 
 **/
///// TODO : ���s�̌�X���A���Z���T�[�ɂ�錟�m�s��
/////        �����g�Z���T�ɂ��Q�[�g���m
/////        �O�i
PassLookUpGate::mAttackCheckPoint(int *forward, int *turn, int color, bool existObject) {
	///// TODO : �����̒ǋL���K�v
	// ���[���̐F���ς�������Ƃ����m�i�D -> ���A�� -> �D�j
	bool isChanged = false;
	// ��Ԍ��m�p�i�����o�ϐ��̕K�v����j
	int nowState = 0;
	///// : TODO
	
	// ���݂̑��s��Ԃ𔻒肷��
	// �Q�[�g���m
	if (existObject) {
		nowState = 1;
	// �Q�[�g���m����Q�[�g�ʉ߂܂�
	} else if (nowState == 1 && !existObject) {
		nowState = 2;
	// �Q�[�g�ʉ߂���A�Q�[�g��O�ɖ߂�܂�
	} else if (nowState == 2 && existObject) {
		nowState = 3;
	// �Q�[�g2��ڂ�ʉ߂���܂�
	} else if (nowState = 3 && existObject) {
		nowState = 4;
	// �Q�[�g2��ڒʉ�
	} else if (nowState = 4 && !existObject) {
		nowState = 5;
	}
	
	switch (nowState) {
	// �}�[�J�[���m�`�Q�[�g�ʉ߁`�}�[�J�[���m�܂�
	case 0:
		///// tail �R���g���[���p��{�l | ���\�b�h}���K�v
		///// pwm �������ɂƂ邩
		///// control_tail �Ƃ������\�b�h��z��
		// �e�[������X���s�p�x�ɐݒ� ///// �i�b��j
		controll_tail(BACKWARD_TILTING);
		
		///// �W���C���Z���T�[�I�t�Z�b�g�l����
		GYRO_OFFSET = BACK_TILT_GYRO;
		
		// ���i������
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