/** 
 * �t�@�C���� : InvertedTraveling.h
 *
 *
 **/

#ifndef INVERTED_TRAVELING
#define INVERTED_TRAVELING

#include "ev3api.h"
#include "balancer.h"
#include "LineMonitor.h"
// #include "CheckPointStrategy.h"

// *** �b�� *** //
// ���Z���T�l
static const int LIGHT_WHITE = 60;
static const int LIGHT_GRAY = 20;
static const int LIGHT_BLACK = 0;

// ���[�^�[�o�͒l�ݒ�
static const int FORWARD = 30;
static const int TURN = 30;
static const float P_GAIN = 0.38;
// *** �b�� *** //

class InvertedTraveling {
private:
	char* mPWM;					// ���[�^�[�o�͒l
	int mEventState;			// ����ʗp�}�[�J�[
	LineMonitor mLineMonitor;
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
	
	void mDeciedeBehavior(int* senSorParam, int* motorParam, int stageState, bool existObject);
	void mGetPWM(char *pwm);
};

#endif
