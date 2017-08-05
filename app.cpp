/**
 * �t�@�C���� : app.cpp
 *
 * �T�v : 2016�V�l���{�R�����C�p�v���O����
 *
 * �Q�l : 2�֓|���U�q���C���g���[�X���{�b�g��TOPPERS/HRP2�pC�T���v���v���O����
 *
 **/

#include "ev3api.h"
#include "app.h"
#include "balancer.h"

#include <cstdio>
#include <string.h>
#include "ColorSensor.h"
#include "Port.h"
#include "LineMonitor.h"
#include "Clock.h"

LineMonitor csLineMonitor;
using namespace ev3api;

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

///// __dso_handle �G���[����̂���
void *__dso_handle = 0;

//*****************************************************************************
// �萔�ݒ� -----
//*****************************************************************************
/**
 * �Z���T�[�A���[�^�[�̐ڑ����`���܂�
 */
static const sensor_port_t
	touch_sensor = EV3_PORT_1,
	sonar_sensor = EV3_PORT_2,
	color_sensor = EV3_PORT_3,
	gyro_sensor = EV3_PORT_4;

static const motor_port_t
	left_motor = EV3_PORT_C,
	right_motor = EV3_PORT_B,
	tail_motor = EV3_PORT_A;

static int bt_cmd = 0;  /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE *bt = NULL; /* Bluetooth�t�@�C���n���h�� */

/* ���L�̃}�N���͌�/���ɍ��킹�ĕύX����K�v������܂� */
/* sample_c1�}�N�� */
#define GYRO_OFFSET 0  /* �W���C���Z���T�I�t�Z�b�g�l(�p���x0[deg/sec]��) */
#define LIGHT_WHITE 45 /* ���F�̌��Z���T�l */
#define LIGHT_BLACK 3  /* ���F�̌��Z���T�l */
/* sample_c2�}�N�� */
#define SONAR_ALERT_DISTANCE 30 /* �����g�Z���T�ɂ���Q�����m����[cm] */
/* sample_c3�}�N�� */
// �e�[���p�x
#define TAIL_ANGLE_STAND_UP 87 /* ���S��~���̊p�x[�x] */
#define TAIL_ANGLE_DRIVE 3	 /* �o�����X���s���̊p�x[�x] */

// #define P_GAIN			  2.5F /* ���S��~�p���[�^������W�� */
#define P_GAIN 0.5F	/* ���S��~�p���[�^������W�� */
#define PWM_ABS_MAX 60 /* ���S��~�p���[�^����PWM��΍ő�l */
/* sample_c4�}�N�� */
//#define DEVICE_NAME	  "ET0"	 /* Bluetooth�� hrp2/target/ev3.h BLUETOOTH_LOCAL_NAME�Őݒ� */
//#define PASS_KEY		  "1234" /* �p�X�L�[	hrp2/target/ev3.h BLUETOOTH_PIN_CODE�Őݒ� */
#define CMD_START '1' /* �����[�g�X�^�[�g�R�}���h */

/* LCD�t�H���g�T�C�Y */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6 /*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8 /*TODO: magic number*/)

static const int tailRunAngle = 67; // �e�[�����s���e�[���p�x�iTODO:�v�����j
//static const int GATE_PASS_MILEAGE = 350; //�Q�[�g�ʉߋ����iTODO:�v�����j
static const int GATE_PASS_MILEAGE = 150; //�Q�[�g�ʉߋ����iTODO:�v�����j
// PID����i�����L�^�p�j
static int diff[2];	// �����L�^�p
static float integral; //
//
static int state = 0;			// ���s��ԁi�V�i���I�j
static float dist = 0;			// �����s����
static float s_dist = 0;		// �����v���J�n�����s����
static int turning_angle = 0;   // ������p�x
static int s_turning_angle = 0; // �v���J�n��

static int init_forward = 70;
static int init_turn = 20;

static int line_white = 0;
static int line_gray = 0;
static int line_black = 0;
// �V�i���I������ԏ������t���O
static bool scenario_init = false;

// �V�i���I���s���
enum Scene
{
	INIT = 0,
	COURSE_RUN,
	LOOK_UP_GATE,
	STAIRS,
	GARAGE
};

static Scene ScenarioState = INIT;

// ���b�N�A�b�v�Q�[�g���s���
enum LookUpState
{
	TAIL_RUN = 0,
	STRAIGHT,
	TURN,
	STAND_UP,
	END
};

// �K�i���s��� TODO
enum StairsRunState
{
	Up = 0,
	TURN_AROUND,
	TURN_AT_RIGHT_ANGLE,
	Down,
	STAND_UP_STIR
};

// �K���[�W���s��� TODO
enum GarageState
{
	STOP
};
//*****************************************************************************
// ----- �萔�ݒ�
//*****************************************************************************

/* �֐��v���g�^�C�v�錾 */
static void initPort();
static void RunParamReset();
static int sonar_alert(void);
static void tail_control(signed int angle);
static void Run(int forward, int turn);
static void CourseRun(int forward, int turn);
static void LookUpGate(bool init);

/* ���C���^�X�N */
void main_task(intptr_t unused)
{
	int forward = init_forward; /* �O��i���� */
	int turn = init_turn;		/* ���񖽗� */

	/////
	line_white = LIGHT_WHITE;
	line_gray = (LIGHT_BLACK + LIGHT_WHITE) / 2;
	line_black = LIGHT_BLACK;
	ColorSensor csCS(PORT_3);
	/////

	/* LCD��ʕ\�� */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	ev3_lcd_draw_string("EV3way-ET sample_c4", 0, CALIB_FONT_HEIGHT * 1);
	initPort();

	/* Open Bluetooth file */
	bt = ev3_serial_open_file(EV3_SERIAL_BT);
	assert(bt != NULL);

	/* Bluetooth�ʐM�^�X�N�̋N�� */
	act_tsk(BT_TASK);

	ev3_led_set_color(LED_ORANGE); /* �����������ʒm */
	/////
	// ���Z���T�[�l�L�����u���[�V����
	ev3_lcd_draw_string("Light callibration", 0, CALIB_FONT_HEIGHT * 1);
	while (1)
	{
		char ms[32];

		if (ev3_button_is_pressed(BACK_BUTTON))
			break;

		if (ev3_button_is_pressed(RIGHT_BUTTON))
		{
			line_white = (int)csCS.getBrightness();
		}

		if (ev3_button_is_pressed(DOWN_BUTTON))
		{
			line_gray = (int)csCS.getBrightness();
		}

		if (ev3_button_is_pressed(LEFT_BUTTON))
		{
			line_black = (int)csCS.getBrightness();
		}

		int c = csLineMonitor.mDecideColor(csCS.getBrightness());
		char *col;
		col = (c == 0) ? const_cast<char *>("BLACK")
					   : (c == 1) ? const_cast<char *>("GRAY ")
								  : const_cast<char *>("WHITE");

		sprintf(ms, ":Rbtn: W : %03d", line_white);
		ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 2);
		sprintf(ms, ":Dbtn: G : %03d", line_gray);
		ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 3);
		sprintf(ms, ":line_blacktn: B : %03d", line_black);
		ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 4);
		sprintf(ms, "NOW : %03d", csCS.getBrightness());
		ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 5);
		sprintf(ms, "NCL : %s", col);
		ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 6);
	}
	/////

	/* �X�^�[�g�ҋ@ */
	while (1)
	{
		tail_control(TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */

		if (bt_cmd == 1)
		{
			bt_cmd = 0;
			break; /* �����[�g�X�^�[�g */
		}

		if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
		{
			break; /* �^�b�`�Z���T�������ꂽ */
		}

		tslp_tsk(10); /* 10msec�E�F�C�g */
	}

	RunParamReset();

	tslp_tsk(10); /* 10msec�E�F�C�g */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	ev3_lcd_draw_string("touched", 0, CALIB_FONT_HEIGHT * 1);

	/**
	* Main loop for the self-balance control aline_grayorithm
	*/
	//////
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	bool flag = false;
	bool ubuttonFlag = false;
	bool dbuttonFlag = false;
	bool line_blackuttonFlag = false;
	bool rbuttonFlag = false;
	bool cbuttonFlag = false;
	int changeForwardNum = 0;
	int changeTurnNum = 0;
	while (1)
	{
		//ScenarioState = COURSE_RUN;
		ScenarioState = LOOK_UP_GATE;
		// ���s�I��(Back�{�^��)
		ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
		if (ev3_button_is_pressed(BACK_BUTTON))
			break;

		while (1)
		{
			tail_control(TAIL_ANGLE_STAND_UP); /* ���S��~�p�p�x�ɐ��� */
											   //			tail_control(tail);
			ev3_lcd_draw_string("TRAVEL SET MODE :UDLR btn:", 0, CALIB_FONT_HEIGHT * 1);

			if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
			{
				flag = true;
				// �^�b�`�Z���T�̗h�ꂪ���܂�̂�҂��Ă���W���C�����Z�b�g
				tslp_tsk(100);
				RunParamReset();
			}
			else if ((ev3_touch_sensor_is_pressed(touch_sensor) == 0 && flag) || bt_cmd == 1)
			{
				bt_cmd = 0;
				flag = false;
				for (int i = 0; i < 20000; i++)
				{
					tail_control(TAIL_ANGLE_STAND_UP + 5);
				}
				forward = init_forward + changeForwardNum;
				turn = init_turn + changeTurnNum;
				break;
			}

			if (ev3_button_is_pressed(UP_BUTTON))
			{
				ubuttonFlag = true;
			}
			else if (!ev3_button_is_pressed(UP_BUTTON) && ubuttonFlag)
			{
				changeForwardNum += 5;
				ubuttonFlag = false;
			}

			if (ev3_button_is_pressed(DOWN_BUTTON))
			{
				dbuttonFlag = true;
			}
			else if (!ev3_button_is_pressed(DOWN_BUTTON) && dbuttonFlag)
			{
				changeForwardNum -= 5;
				dbuttonFlag = false;
			}

			if (ev3_button_is_pressed(RIGHT_BUTTON))
			{
				rbuttonFlag = true;
			}
			else if (!ev3_button_is_pressed(RIGHT_BUTTON) && rbuttonFlag)
			{
				//				turn += 5;
				changeTurnNum += 5;
				rbuttonFlag = false;
			}

			if (ev3_button_is_pressed(LEFT_BUTTON))
			{
				line_blackuttonFlag = true;
			}
			else if (!ev3_button_is_pressed(LEFT_BUTTON) && line_blackuttonFlag)
			{
				//				turn -= 5;
				changeTurnNum -= 5;
				line_blackuttonFlag = false;
			}
			if (ev3_button_is_pressed(ENTER_BUTTON))
			{
				cbuttonFlag = true;
			}
			else if (!ev3_button_is_pressed(ENTER_BUTTON) && cbuttonFlag)
			{
				cbuttonFlag = false;
				// enum���z�C���N�������g
				if (ScenarioState == GARAGE)
				{
					ScenarioState = static_cast<Scene>(0);
				}
				else
				{
					ScenarioState = static_cast<Scene>(ScenarioState + 1);
				}
			}

			//			dist = ((ev3_motor_get_counts(left_motor) + ev3_motor_get_counts(right_motor)) / 2) * 0.74;
			dist = ((float)(ev3_motor_get_counts(left_motor) + ev3_motor_get_counts(right_motor)) / 2);
			dist = dist * 160 * 3.14 / 720;
			turning_angle = ((float)(ev3_motor_get_counts(right_motor) - ev3_motor_get_counts(left_motor)) * 0.27);
			char ms[32];
			sprintf(ms, "forward : %03d", changeForwardNum);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 2);
			sprintf(ms, ":Not Use: turn : %03d", changeTurnNum);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 3);
			sprintf(ms, "tail : %03d", (int)ev3_motor_get_counts(tail_motor));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 4);
			sprintf(ms, "L Motor : %d", (int)ev3_motor_get_counts(left_motor));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 5);
			sprintf(ms, "R Motor : %d", (int)ev3_motor_get_counts(right_motor));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 6);
			sprintf(ms, "dist : %05f mm", dist);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 7);
			sprintf(ms, "turning angle : %05d ^", turning_angle);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 8);
			sprintf(ms, "turning angle : %05f ^", ((float)(ev3_motor_get_counts(right_motor) - ev3_motor_get_counts(left_motor)) * 0.23));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 9);
			sprintf(ms, "SCENARIO STATE : %05d ^", ScenarioState);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 10);
			tslp_tsk(10);
		}

		ubuttonFlag = false;
		dbuttonFlag = false;
		line_blackuttonFlag = false;
		rbuttonFlag = false;
		dist = 0;

		ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
		ev3_lcd_draw_string("TRAVELING MODE", 0, CALIB_FONT_HEIGHT * 1);
		tslp_tsk(5); /* 10msec�E�F�C�g */

		// �X�^�[�g�_�b�V���i�O�X�p���j
		for (int i = 0; i < 100; i++)
		{
			tail_control(TAIL_ANGLE_STAND_UP + 10);
			tslp_tsk(1);
		}

		// �R�[�X���s
		while (1)
		{
			// ���s�����A����p�x�펞�v��
			// ���s�����v��
			dist = ((float)(ev3_motor_get_counts(left_motor) + ev3_motor_get_counts(right_motor)) / 2) * 0.72;
			// ����p�x�v��
			turning_angle = ((ev3_motor_get_counts(right_motor) - ev3_motor_get_counts(left_motor)) * 0.27);

			//			ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
			//			ev3_lcd_draw_string("TRAVELING MODE", 0, CALIB_FONT_HEIGHT*1);
			// ��~����
			if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
			{
				flag = true;
			}
			else if ((ev3_touch_sensor_is_pressed(touch_sensor) == 0 && flag) ||
					 bt_cmd == 1)
			{
				forward = 0;
				turn = 0;

				// �e�[���Î~�p�x
				for (int i = 0; i < 10000; i++)
				{
					tail_control(TAIL_ANGLE_STAND_UP - (5 / (10001 - i)));
				}

				// ��X�ڍs�i�����i�j
				for (int i = 0; i < 50; i++)
				{
					ev3_motor_set_power(left_motor, 50);
					ev3_motor_set_power(right_motor, 50);
					tslp_tsk(1);
				}
				// ���[�^�[��~
				ev3_motor_stop(left_motor, false);
				ev3_motor_stop(right_motor, false);
				ev3_motor_stop(tail_motor, false);
				tslp_tsk(10); /* 10msec�E�F�C�g */

				// ev3_motor_reset_counts(left_motor);
				// ev3_motor_reset_counts(right_motor);
				// tslp_tsk(10); /* 10msec�E�F�C�g */
				// ev3_gyro_sensor_reset(gyro_sensor);
				// tslp_tsk(10); /* 10msec�E�F�C�g */
				// balance_init(); /* �|���U�qAPI������ */
				// tslp_tsk(10); /* 10msec�E�F�C�g */

				bt_cmd = 0;
				flag = false;
				// �V�i���I������Ԃ̏�����
				scenario_init = true;
				break;
			}

			// ���s�̏�ԕ\��
			char ms[32];
			sprintf(ms, "R Motor : %d", (int)ev3_motor_get_counts(left_motor));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 2);
			sprintf(ms, "L Motor : %d", (int)ev3_motor_get_counts(right_motor));
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 3);
			sprintf(ms, "dist : %05f mm", dist);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 4);
			sprintf(ms, "angle : %05d mm", turning_angle);
			ev3_lcd_draw_string(ms, 0, CALIB_FONT_HEIGHT * 5);

			Run(forward, turn);
			tslp_tsk(4); /* 4msec�����N�� */
		}

		tslp_tsk(10); /* 10msec�E�F�C�g */
	}
	// �I������
	// ���[�^�[��~
	ev3_motor_stop(left_motor, false);
	ev3_motor_stop(right_motor, false);
	ev3_motor_stop(tail_motor, false);
	// Bluetooth�ʐM�^�X�N�̏I��
	ter_tsk(BT_TASK);
	fclose(bt);

	ext_tsk();
}

//*****************************************************************************
// �֐��� : initPort
// ���� : �Ȃ�
// �Ԃ�l : �Ȃ�
// �T�v : �Z���T�[�A���[�^�[������
//*****************************************************************************
void initPort()
{
	/* �Z���T�[���̓|�[�g�̐ݒ� */
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor); /* ���˗����[�h */
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	/* ���[�^�[�o�̓|�[�g�̐ݒ� */
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	ev3_motor_reset_counts(tail_motor);
}

//*****************************************************************************
// �֐��� : RunParamReset
// ���� : �Ȃ�
// �Ԃ�l : �Ȃ�
// �T�v : ���s�̃Z���T�l�A���[�^�l�A�o�����T�[������
//*****************************************************************************
void RunParamReset()
{
	/* ���s���[�^�[�G���R�[�_�[���Z�b�g */
	tslp_tsk(10); /* 10msec�E�F�C�g */
	ev3_motor_reset_counts(left_motor);
	ev3_motor_reset_counts(right_motor);
	ev3_lcd_draw_string("motor reset", 0, CALIB_FONT_HEIGHT * 2);

	/* �W���C���Z���T�[���Z�b�g */
	tslp_tsk(10); /* 10msec�E�F�C�g */
	ev3_gyro_sensor_reset(gyro_sensor);
	ev3_lcd_draw_string("gyro reset", 0, CALIB_FONT_HEIGHT * 3);

	tslp_tsk(10);   /* 10msec�E�F�C�g */
	balance_init(); /* �|���U�qAPI������ */
	ev3_lcd_draw_string("balance init", 0, CALIB_FONT_HEIGHT * 4);

	tslp_tsk(10);				  /* 10msec�E�F�C�g */
	ev3_led_set_color(LED_GREEN); /* �X�^�[�g�ʒm */
	tslp_tsk(10);				  /* 10msec�E�F�C�g */
}

//*****************************************************************************
// �֐��� : sonar_alert
// ���� : ����
// �Ԃ�l : 1(��Q������)/0(��Q������)
// �T�v : �����g�Z���T�ɂ���Q�����m
//*****************************************************************************
static int sonar_alert(void)
{
	static unsigned int counter = 0;
	static int alert = 0;

	signed int distance;

	if (++counter == 40 / 4) /* ��40msec�������ɏ�Q�����m  */
	{
		/*
		 * �����g�Z���T�ɂ�鋗����������́A�����g�̌��������Ɉˑ����܂��B
		 * NXT�̏ꍇ�́A40msec�������x���o����̍ŒZ��������ł��B
		 * EV3�̏ꍇ�́A�v�m�F
		 */
		distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
		if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
		{
			alert = 1; /* ��Q�������m */
		}
		else
		{
			alert = 0; /* ��Q������ */
		}
		counter = 0;
	}

	return alert;
}

//*****************************************************************************
// �֐��� : tail_control
// ���� : angle (���[�^�ڕW�p�x[�x])
// �Ԃ�l : ����
// �T�v : ���s�̊��S��~�p���[�^�̊p�x����
//*****************************************************************************
static void tail_control(signed int angle)
{
	float pwm = (float)(angle - ev3_motor_get_counts(tail_motor)) * P_GAIN; /* ��ᐧ�� */
	/* PWM�o�͖O�a���� */
	if (pwm > PWM_ABS_MAX)
	{
		pwm = PWM_ABS_MAX;
	}
	else if (pwm < -PWM_ABS_MAX)
	{
		pwm = -PWM_ABS_MAX;
	}

	if (pwm == 0)
	{
		ev3_motor_stop(tail_motor, true);
	}
	else
	{
		ev3_motor_set_power(tail_motor, (signed char)pwm);
	}
}

//*****************************************************************************
// �֐��� : bt_task
// ���� : unused
// �Ԃ�l : �Ȃ�
// �T�v : Bluetooth�ʐM�ɂ�郊���[�g�X�^�[�g�B Tera Term�Ȃǂ̃^�[�~�i���\�t�g����A
//		 ASCII�R�[�h��1�𑗐M����ƁA�����[�g�X�^�[�g����B
//*****************************************************************************
void bt_task(intptr_t unused)
{
	while (1)
	{
		uint8_t c = fgetc(bt); /* ��M */
		switch (c)
		{
		case '1':
			bt_cmd = 1;
			break;
		default:
			char msg[32];
			sprintf(msg, "%03d\n", ev3_color_sensor_get_reflect(color_sensor));
			fprintf(bt, msg);
			break;
		}
	}
}

//*****************************************************************************
// �֐��� : Run
// ���� : forward �O�i�l�Aturn ����l
// �Ԃ�l : �Ȃ�
// �T�v : ��Ԃɉ��������s���s��
//*****************************************************************************
void Run(int forward, int turn)
{
	switch (ScenarioState)
	{
	case INIT:
		break;
	case COURSE_RUN:
		CourseRun(forward, turn);
		break;
	case LOOK_UP_GATE:
		LookUpGate(scenario_init);
		break;
	case STAIRS:
		break;
	case GARAGE:
		break;
	default:
		break;
	}
	if (scenario_init)
	{
		scenario_init = false;
	}
}

//*****************************************************************************
// �֐��� : CourseRun
// ���� : forward �O�i�l�Aturn ����l
// �Ԃ�l : �Ȃ�
// �T�v : ���C���g���[�X�ɂ��R�[�X���s���s��
//*****************************************************************************
void CourseRun(int forward, int turn)
{
	//	int line_white = 40;
	//	int line_gray = 20;
	//	int line_black = 0;
	//	float P_GAIN = 0.38;
	float p, i, d;
	float Kp = 1.0, Ki = 1.2, Kd = 0.027;
	int target = 30;
	Clock clk;

	signed char pwm_L, pwm_R; /* ���E���[�^PWM�o�� */
	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	tail_control(TAIL_ANGLE_DRIVE); /* �o�����X���s�p�p�x�ɐ��� */
									//	tail_control(TAIL_ANGLE_STAND_UP - 15);
									//	tail_control(tail);	  ////// STAGE2 Look up

	//		  if (sonar_alert() == 1) /* ��Q�����m */
	if (0) // ����
	{
		forward = turn = 0; /* ��Q�������m�������~ */
	}
	// On/Off���s(���ݖ��g�p)
	else
	{
		forward = forward; /* �O�i���� */
						   //			if (ev3_color_sensor_get_reflect(color_sensor) >= (LIGHT_WHITE + LIGHT_BLACK)/2)
		if (ev3_color_sensor_get_reflect(color_sensor) >= (line_white + line_gray) / 2)
		{
			turn = turn; /* �����񖽗� */
		}
		else
		{
			turn = -turn; /* �E���񖽗� */
		}
	}

	// PID ����
	diff[0] = diff[1];
	diff[1] = ev3_color_sensor_get_reflect(color_sensor) - target;
	integral += (diff[0] + diff[1]) / 2 * 0.004;

	p = Kp * diff[1];
	i = Ki * integral;
	d = Kd * (diff[1] - diff[0]) / 0.004;

	turn = p + i + d;

	/* �|���U�q����API �ɓn���p�����[�^���擾���� */
	motor_ang_l = ev3_motor_get_counts(left_motor);
	motor_ang_r = ev3_motor_get_counts(right_motor);
	gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
	volt = ev3_battery_voltage_mV();

	/* �|���U�q����API���Ăяo���A�|�����s���邽�߂� */
	/* ���E���[�^�o�͒l�𓾂� */
	balance_control(
		(float)forward,
		(float)turn,
		(float)gyro,
		(float)GYRO_OFFSET,
		(float)motor_ang_l,
		(float)motor_ang_r,
		(float)volt,
		(signed char *)&pwm_L,
		(signed char *)&pwm_R);

	/* EV3�ł̓��[�^�[��~���̃u���[�L�ݒ肪���O�ɂł��Ȃ����� */
	/* �o��0���ɁA���̓s�x�ݒ肷�� */
	if (pwm_L == 0)
	{
		ev3_motor_stop(left_motor, true);
	}
	else
	{
		if (pwm_L > 90)
		{
			pwm_L = 90;
		}
		ev3_motor_set_power(left_motor, (int)pwm_L);
	}

	if (pwm_R == 0)
	{
		ev3_motor_stop(right_motor, true);
	}
	else
	{
		if (pwm_R > 90)
		{
			pwm_R = 90;
		}
		ev3_motor_set_power(right_motor, (int)pwm_R);
	}
}

//*****************************************************************************
// �֐��� : LookUpGate
// ���� : forward �O�i�l�Aturn ����l
// �Ԃ�l : �Ȃ�
// �T�v : ���b�N�A�b�v�Q�[�g���U������
//*****************************************************************************
void LookUpGate(bool init)
{
	// �o�����T�[�p�ϐ�
	signed char pwm_L, pwm_R; /* ���E���[�^PWM�o�� */
	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;
	bool blance = false;

	int forward = 0;
	int turn = 0;

	// �V�[�������
	static LookUpState State = TAIL_RUN;
	// ���s��������p�ϐ�
	// �֐�����static��1��ڂ̌Ăяo�����������������2��ڈȍ~�͏���������Ȃ����I�������I
	static int mMileage = (int)dist;
	static int mTurn = turning_angle;

	int nowDist;  // ���s�J�n���猻�݂܂ł̋���
	int moveDist; // ��p����J�n������̑��s����

	int nowTurningAngle;

	// �U���J�n���p�����^���L�^
	int leftMotorAngle = ev3_motor_get_counts(right_motor);
	int rightMotorAngle = -ev3_motor_get_counts(left_motor);
	static int tailAngle = ev3_motor_get_counts(tail_motor);
	// ����init��true�̏ꍇ�Astatic�ϐ������������ă��b�N�A�b�v�Q�[�g�U���O��Ԃɖ߂�
	if (init)
	{
		State = TAIL_RUN;
		mMileage = (int)dist;
		mTurn = turning_angle;
		tailAngle = ev3_motor_get_counts(tail_motor);
	}

	int RADIUS = 80;
	float PI = 3.14159;

	// �����_�̑��s����
	// ���E���[�^�G���R�[�_�l�̕��ς��擾
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;

	// ���s�������v�Z
	nowDist = (int)dist;

	// �����_�̐���l
	nowTurningAngle = (rightMotorAngle - leftMotorAngle) * 0.23;

	// ����J�n������̑��s����
	moveDist = nowDist - mMileage;

	switch (State)
	{
	case TAIL_RUN:
	{
		// �e�[�����s�p�p�x�Ɉڍs�@
		forward = 0; // TODO
		turn = 0;	// TODO

		// �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
		//float tail_pwm = (float)(TAIL_ANGLE_DRIVE + 1 - tailAngle) * P_GAIN; // ��ᐧ��
		//tail_control(tailRunAngle);
		tailAngle = tailRunAngle;

		// �e�[������X���s�p�x�ɐݒ肵���ꍇ
		if (ev3_motor_get_counts(tail_motor) <= tailRunAngle)
		{
			State = STRAIGHT;
			mTurn = nowTurningAngle;
		}

		break;
	}
	case STRAIGHT:
		// ���i
		forward = 20;

		// �Q�[�g�𒴂����ꍇ
		if (moveDist > GATE_PASS_MILEAGE)
		{
			// ����J�n�����X�V
			mMileage = nowDist;
			// ��x�ڂ͌��
			//mTravelState = (mTravelState == 2) ? 3 : 5;
			State = STAND_UP;
		}

		break;
	case TURN:
		break;
	case STAND_UP:
	{
		// �N���オ��
		forward = 0;
		turn = 0;

		// // �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
		// float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // ��ᐧ��
		// // �e�[�����[�^�[�o�͒l��ݒ�
		// motorParam[2] = tail_pwm;
		tailAngle = TAIL_ANGLE_STAND_UP + 5;

		// �e�[���𑖍s�p�x�ɐݒ肵���ꍇ
		if (ev3_motor_get_counts(tail_motor) >= TAIL_ANGLE_STAND_UP + 5 - 1)
		{
			// ���s��Ԃ��X�V
			State = END;
		}
		break;
	}
	default:
		forward = 3;
		turn = 0;
		tailAngle = TAIL_ANGLE_DRIVE;
		blance = true;
		/* �|���U�q����API �ɓn���p�����[�^���擾���� */

		break;
	}
	tail_control(tailAngle);
	// �o�����T�[�g�p
	if (blance == true)
	{
		motor_ang_l = ev3_motor_get_counts(left_motor);
		motor_ang_r = ev3_motor_get_counts(right_motor);
		gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
		volt = ev3_battery_voltage_mV();
		balance_control(
			(float)forward,
			(float)turn,
			(float)gyro,
			(float)GYRO_OFFSET,
			(float)motor_ang_l,
			(float)motor_ang_r,
			(float)volt,
			(signed char *)&pwm_L,
			(signed char *)&pwm_R);
	}
	// �o�����T�[�s�g�p
	else
	{
		pwm_L = (signed char)forward;
		pwm_R = (signed char)forward;
	}

	/* EV3�ł̓��[�^�[��~���̃u���[�L�ݒ肪���O�ɂł��Ȃ����� */
	/* �o��0���ɁA���̓s�x�ݒ肷�� */
	if (pwm_L == 0)
	{
		ev3_motor_stop(left_motor, true);
	}
	else
	{
		if (pwm_L > 90)
		{
			pwm_L = 90;
		}
		ev3_motor_set_power(left_motor, (int)pwm_L);
	}

	if (pwm_R == 0)
	{
		ev3_motor_stop(right_motor, true);
	}
	else
	{
		if (pwm_R > 90)
		{
			pwm_R = 90;
		}
		ev3_motor_set_power(right_motor, (int)pwm_R);
	}

	// ���s��Ԃɉ����ĐU�镑��������
	// switch (mTravelState) {
	// // ��U���J�n��
	// case 0:
	// 	// case0�͎g�p���Ȃ�
	// 	// // �J�n�����s�������L�^
	// 	// mMileage = nowDist;

	// 	// // �J�n������p�x���L�^
	// 	// mTurningAngle = nowTurningAngle;

	// 	// // ���s��Ԃ����̒i�K��
	// 	// mTravelState = 1;

	// 	break;
	// // �e�[������X���s�p�x�ɐݒ�
	// case 1:
	// 	{
	// 	// �ᑬ���s
	// 	forward = GATE_LOW_FORWARD;
	// 	turn = GATE_LOW_TURN;

	// 	// �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
	// 	float tail_pwm = (float)(tailRunAngle + 1 - tailAngle) * GATE_TAIL_P_GAIN; // ��ᐧ��
	// 	// �e�[�����[�^�[�o�͒l��ݒ�
	// 	motorParam[2] = tail_pwm;

	// 	// �e�[������X���s�p�x�ɐݒ肵���ꍇ
	// 	if (tailAngle < tailRunAngle) {
	// 		mTravelState = 2;
	// 		mTurningAngle = nowTurningAngle;
	// 	}

	// 	break;
	// 	}
	// // �w�苗�������O�i
	// case 2:
	// case 4:
	// 	// ���i
	// 	motorParam[0] = GATE_MIDDLE_FORWARD;
	// 	motorParam[1] = GATE_MIDDLE_FORWARD;

	// 	// �Q�[�g�𒴂����ꍇ
	// 	if (moveDist > GATE_PASS_MILEAGE) {
	// 		// ����J�n�����X�V
	// 		mMileage = nowDist;
	// 		// ��x�ڂ͌��
	// 		mTravelState = (mTravelState == 2) ? 3 : 5;
	// 	}

	// 	break;
	// // �A�[�`���ʉߌ�
	// case 3:
	// 	// ���
	// 	motorParam[0] = -GATE_MIDDLE_FORWARD;
	// 	motorParam[1] = -GATE_MIDDLE_FORWARD;

	// 	// �w�苗���߂����ꍇ
	// 	if (moveDist < -GATE_PASS_MILEAGE) {
	// 		// ����J�n�����X�V
	// 		mMileage = nowDist;

	// 		// ���s��Ԃ��X�V
	// 		mTravelState = 4;
	// 	}

	// 	break;
	// // �e�[���𑖍s�J�n�p�x�ɐݒ�
	// case 5:
	// 	{
	// 	// �ᑬ���s
	// 	*forward = GATE_LOW_FORWARD;
	// 	*turn = GATE_LOW_TURN;

	// 	// �e�[�����[�^�[�o�͒l��ڕW�l�Ƃ̍��ɉ����Č���
	// 	float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // ��ᐧ��
	// 	// �e�[�����[�^�[�o�͒l��ݒ�
	// 	motorParam[2] = tail_pwm;

	// 	// �e�[���𑖍s�p�x�ɐݒ肵���ꍇ
	// 	if (tailAngle < GATE_TAIL_TRAVEL_ANGLE + 3) {
	// 		// ���s��Ԃ��X�V
	// 		mTravelState = 6;
	// 	}

	// 	break;
	// 	}
	// // ���̓�ɐi��
	// case 6:
	// 	// �ʏ푖�s
	// 	*forward = GATE_MIDDLE_FORWARD;
	// 	*turn = GATE_MIDDLE_TURN;

	// 	break;
	// default:
	// 	*forward = GATE_MIDDLE_FORWARD;
	// 	*turn = GATE_MIDDLE_TURN;
	// 	break;
	// }
}
