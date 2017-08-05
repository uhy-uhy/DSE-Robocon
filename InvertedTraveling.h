/** 
 * �t�@�C���� : InvertedTraveling.h
 * 
 * �T�v : �|�����s�@�\
 *
 **/

#ifndef INVERTED_TRAVELING
#define INVERTED_TRAVELING

#include "ev3api.h"
#include "balancer.h"
#include "LineMonitor.h"
// #include "CheckPointStrategy.h"

// �~����
#define PI (3.1415926536)

// ���s�̃p�����[�^ �P�� : mm
static const int RADIUS = 80;			// �^�C�����a
static const int TREAD = 160;			// �ԗ֊ԋ���

///// *** �b�� *** /////
// ���Z���T�l
static const int LIGHT_WHITE = 60;		// ���F���ˌ�
static const int LIGHT_GRAY = 20;		// �D�F���ˌ�
static const int LIGHT_BLACK = 0;		// ���F���ˌ�

// ���[�^�[�o�͒l�ݒ�
static const int FORWARD = 30;			// �O�i�l
static const int TURN = 30;				// ����l�i���F�E����A���F������j
static const float P_GAIN = 0.38;		// P����W��

// ���E�G���R�[�_�l��/�x �i�����l����Z�o�j
static const float TURNING_ANGLE_UNIT = 0.23;
///// *** �b�� *** /////

static const int GYRO_OFFSET = 0;

class InvertedTraveling {
private:
	char mPWM[3];				// ���[�^�[�o�͒l
	int mEventState;			// ����ʗp�}�[�J�[
	LineMonitor* mLineMonitor;
//	CheckPointStrategy* mCheckPointStrategy[5] = {
//		new SelectMaxNumRoute,
//		new PassLookUpGate,
//		new DoPerformance,
//		new CrossParallelBridges,
//		new CrossArchBridge
//	};
public:
	InvertedTraveling();
	~InvertedTraveling();
	
	void mDecideBehavior(int* senSorParam, int* motorParam, int stageState);	// ���[�^�[�o�͒l����
	void mGetPWM(char *pwm);		// ���[�^�[�o�͒l�ԋp
	
	static int mMeasureMileage(int leftMotorAngle, int rightMotorDegee);		// ���s�����v��
	static int mMeasureTurningAngle(int leftMotorAngle, int rightMotorDegee);	// ����p�x�v��
};

#endif
