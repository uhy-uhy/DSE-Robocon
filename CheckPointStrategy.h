/**
 * �t�@�C���� : CheckPointStrategy.h
 * 
 * �T�v : �e��U���N���X�̃X�[�p�[�N���X
 *        �C���^�[�t�F�[�X�Ƃ��Ă̖���
 * 
 **/

#ifndef CHECK_POINT_STRATEGY
#define CHECK_POINT_STRATEGY

class CheckPointStrategy {
protected:
	int mMileage;		// �C�x���g�J�n������̑��s����(mm)
	int mTurningAngle;	// ����p�x
	int mTravelState;	// �C�x���g���s���
public:
	CheckPointStrategy();
	virtual ~CheckPointStrategy();
	
	// ��U��
	virtual void mAttackCheckPoint(int *forward, int *turn, int *sensorParam, int *motorParam);
};

#endif
