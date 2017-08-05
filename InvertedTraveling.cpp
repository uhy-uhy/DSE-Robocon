/** 
 * �t�@�C���� : InvertedTraveling.cpp
 *
 *
 **/

#include "InvertedTraveling.h"

/** 
 * �R���X�g���N�^
 * 
 * 
 **/
InvertedTraveling::InvertedTraveling() : mPWM(), mEventState() {
	mLineMonitor = new LineMonitor();
}

/** 
 * �f�X�g���N�^
 * 
 * 
 **/
InvertedTraveling::~InvertedTraveling() {
	delete mLineMonitor;
}

/** 
 * ���s��Ԃ𔻒肵�A���[�^�[�̏o�͂����肷��
 * 
 * 
 **/
void InvertedTraveling::mDecideBehavior(int *sensorParam, int *motorParam, int stageState) {
	// ���[���F���� 0 : WHITE, 1 : GRAY, 2 : BLACK
	
	int color = mLineMonitor->mDecideColor(sensorParam[2]);
	
	// �O��i�l
	signed char forward = FORWARD;
	// ����l
	signed char turn = TURN;
	
	// �|���U�q����API�ɓn���p�����[�^���擾����
	int volt;// �o�b�e���[�̓d��
	
	// STAGE ���ɂ�葖�s���@�𔻒�
	// STAGE1 �̏ꍇ
	if (stageState == 1) {
		///// ��قǌ���
		// P����ڕW�l
		// int target = (LIGHT_WHITE + LIGHT_BLACK)/2;
		// ���[���̐F�ɉ��������s��������
		// ���C���g���[�X�E�����m
		switch(color) {
		// ���[���̐F�����̏ꍇ
		case 0:
			// ���ɐ���
			turn = TURN;
			break;
		// ���[���̐F���D�F�̏ꍇ
		case 1:
			///// 
			// �}�[�J�[����̏����L�q
			break;
		// ���[���̐F�����̏ꍇ
		case 2:
			turn = -TURN;
			break;
		default:
			break;
		}
	}
//	// STAGE2 �̏ꍇ
//	} else if (stageState == 2) {
//		// ����ʃ}�[�J�[�ɂ��U�镑�����菈��
//		switch(mEventState) {
//		// ���[�g�I��
//		case 1:
//			mCheckPointStrategy[0]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// ���b�N�A�b�v�Q�[�g
//		case 2:
//			mCheckPointStrategy[1]->mAttackCheckPoint(&forward, &turn, color, existObject);
//			break;
//		// �p�t�H�[�}���X�X�e�[�W
//		case 3:
//			mCheckPointStrategy[2]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// ��{��
//		case 4:
//			mCheckPointStrategy[3]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		// �A�[�`��
//		case 5:
//			mCheckPointStrategy[4]->mAttackCheckPoint(&forward, &turn, color);
//			break;
//		default:
//			break;
//		}
//		
//	}
	
	// �o�b�e���[�l���擾����
	volt = ev3_battery_voltage_mV();
	
	///// print debug
//	char msg[32];
//	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
//	ev3_lcd_draw_string("before balancer", 0, 8*1);
//	sprintf(msg, "Gyro Sensor : %d", sensorParam[3]);
//	const char* mg = msg;
//	ev3_lcd_draw_string(mg, 0, 8*2);
//	sprintf(msg, "motorParam 0 : %d", motorParam[0]);
//	const char* mp0 = msg;
//	ev3_lcd_draw_string(mp0, 0, 8*3);
//	sprintf(msg, "motorParam 1 : %d", motorParam[1]);
//	const char* mp1 = msg;
//	ev3_lcd_draw_string(mp1, 0, 8*4);
//	sprintf(msg, "mPWM 0 : %d", mPWM[0]);
//	const char* m0 = msg;
//	ev3_lcd_draw_string(m0, 0, 8*5);
//	sprintf(msg, "mPWM 1 : %d", mPWM[1]);
//	const char* m1 = msg;
//	ev3_lcd_draw_string(m1, 0, 8*6);
//	sprintf(msg, "mPWM 2 : %d", mPWM[2]);
//	const char* m2 = msg;
//	ev3_lcd_draw_string(m2, 0, 8*7);
//	tslp_tsk(3000);
	
	// �|���U�q����API���Ăяo���A
	// �|�����s���邽�߂̍��E���[�^�[�o�͒l�𓾂�
	balance_control(
		(float)forward,
		(float)turn,
		(float)sensorParam[3],
		(float)GYRO_OFFSET,
		(float)motorParam[0],
		(float)motorParam[1],
		(float)volt,
		(signed char*)&mPWM[0],
		(signed char*)&mPWM[1]);

//	///// print debug
//	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
//	ev3_lcd_draw_string("after balancer", 0, 8*1);
//	sprintf(msg, "Gyro Sensor : %d", sensorParam[3]);
//	const char* amg = msg;
//	ev3_lcd_draw_string(amg, 0, 8*2);
//	sprintf(msg, "motorParam 0 : %d", motorParam[0]);
//	const char* amp0 = msg;
//	ev3_lcd_draw_string(amp0, 0, 8*3);
//	sprintf(msg, "motorParam 1 : %d", motorParam[1]);
//	const char* amp1 = msg;
//	ev3_lcd_draw_string(amp1, 0, 8*4);
//	sprintf(msg, "mPWM 0 : %d", mPWM[0]);
//	const char* am0 = msg;
//	ev3_lcd_draw_string(am0, 0, 8*5);
//	sprintf(msg, "mPWM 1 : %d", mPWM[1]);
//	const char* am1 = msg;
//	ev3_lcd_draw_string(am1, 0, 8*6);
//	sprintf(msg, "mPWM 2 : %d", mPWM[2]);
//	const char* am2 = msg;
//	ev3_lcd_draw_string(am2, 0, 8*7);
//	tslp_tsk(3000);
}

/** 
 * ���[�^�[�̏o�͒l��ԋp����
 * 
 * 
 **/
void InvertedTraveling::mGetPWM(char *pwm) {
	pwm = mPWM;
}

/** 
 * ���s�����v��
 * 
 * 
 **/
int mMeasureMileage(int leftMotorAngle, int rightMotorAngle) {
	// ���E���[�^�G���R�[�_�l�̕��ς��擾
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;
	
	// ���s�������v�Z
	int mileage = RADIUS * 2 * PI * (aveAngle / 360);
	
	return mileage;
}

/** 
 * ����p�x�v��
 * 
 * 
 **/
int mMeasureTurningAngle(int leftMotorAngle, int rightMotorAngle) {
	// ����p�x�v���i����肪���j
	///// �v�Z�����狁�߂�ꍇ
	///// 
	// int turningAngle = RADIUS * 2 * PI * (rightMotorDegee - leftMotorAngle) / TREAD;
	
	///// �����l���狁�߂�ꍇ
	int turningAngle = (rightMotorAngle - leftMotorAngle) * TURNING_ANGLE_UNIT;
	return turningAngle;
}
