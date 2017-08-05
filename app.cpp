/**
 * ファイル名 : app.cpp
 *
 * 概要 : 2016新人ロボコン研修用プログラム
 *
 * 参考 : 2輪倒立振子ライントレースロボットのTOPPERS/HRP2用Cサンプルプログラム
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

///// __dso_handle エラー回避のため
void *__dso_handle = 0;

//*****************************************************************************
// 定数設定 -----
//*****************************************************************************
/**
 * センサー、モーターの接続を定義します
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

static int bt_cmd = 0;  /* Bluetoothコマンド 1:リモートスタート */
static FILE *bt = NULL; /* Bluetoothファイルハンドル */

/* 下記のマクロは個体/環境に合わせて変更する必要があります */
/* sample_c1マクロ */
#define GYRO_OFFSET 0  /* ジャイロセンサオフセット値(角速度0[deg/sec]時) */
#define LIGHT_WHITE 45 /* 白色の光センサ値 */
#define LIGHT_BLACK 3  /* 黒色の光センサ値 */
/* sample_c2マクロ */
#define SONAR_ALERT_DISTANCE 30 /* 超音波センサによる障害物検知距離[cm] */
/* sample_c3マクロ */
// テール角度
#define TAIL_ANGLE_STAND_UP 87 /* 完全停止時の角度[度] */
#define TAIL_ANGLE_DRIVE 3	 /* バランス走行時の角度[度] */

// #define P_GAIN			  2.5F /* 完全停止用モータ制御比例係数 */
#define P_GAIN 0.5F	/* 完全停止用モータ制御比例係数 */
#define PWM_ABS_MAX 60 /* 完全停止用モータ制御PWM絶対最大値 */
/* sample_c4マクロ */
//#define DEVICE_NAME	  "ET0"	 /* Bluetooth名 hrp2/target/ev3.h BLUETOOTH_LOCAL_NAMEで設定 */
//#define PASS_KEY		  "1234" /* パスキー	hrp2/target/ev3.h BLUETOOTH_PIN_CODEで設定 */
#define CMD_START '1' /* リモートスタートコマンド */

/* LCDフォントサイズ */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6 /*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8 /*TODO: magic number*/)

static const int tailRunAngle = 67; // テール走行時テール角度（TODO:要調整）
//static const int GATE_PASS_MILEAGE = 350; //ゲート通過距離（TODO:要調整）
static const int GATE_PASS_MILEAGE = 150; //ゲート通過距離（TODO:要調整）
// PID制御（差分記録用）
static int diff[2];	// 差分記録用
static float integral; //
//
static int state = 0;			// 走行状態（シナリオ）
static float dist = 0;			// 総走行距離
static float s_dist = 0;		// 距離計測開始時走行距離
static int turning_angle = 0;   // 総旋回角度
static int s_turning_angle = 0; // 計測開始時

static int init_forward = 70;
static int init_turn = 20;

static int line_white = 0;
static int line_gray = 0;
static int line_black = 0;
// シナリオ内部状態初期化フラグ
static bool scenario_init = false;

// シナリオ走行状態
enum Scene
{
	INIT = 0,
	COURSE_RUN,
	LOOK_UP_GATE,
	STAIRS,
	GARAGE
};

static Scene ScenarioState = INIT;

// ルックアップゲート走行状態
enum LookUpState
{
	TAIL_RUN = 0,
	STRAIGHT,
	TURN,
	STAND_UP,
	END
};

// 階段走行状態 TODO
enum StairsRunState
{
	Up = 0,
	TURN_AROUND,
	TURN_AT_RIGHT_ANGLE,
	Down,
	STAND_UP_STIR
};

// ガレージ走行状態 TODO
enum GarageState
{
	STOP
};
//*****************************************************************************
// ----- 定数設定
//*****************************************************************************

/* 関数プロトタイプ宣言 */
static void initPort();
static void RunParamReset();
static int sonar_alert(void);
static void tail_control(signed int angle);
static void Run(int forward, int turn);
static void CourseRun(int forward, int turn);
static void LookUpGate(bool init);

/* メインタスク */
void main_task(intptr_t unused)
{
	int forward = init_forward; /* 前後進命令 */
	int turn = init_turn;		/* 旋回命令 */

	/////
	line_white = LIGHT_WHITE;
	line_gray = (LIGHT_BLACK + LIGHT_WHITE) / 2;
	line_black = LIGHT_BLACK;
	ColorSensor csCS(PORT_3);
	/////

	/* LCD画面表示 */
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	ev3_lcd_draw_string("EV3way-ET sample_c4", 0, CALIB_FONT_HEIGHT * 1);
	initPort();

	/* Open Bluetooth file */
	bt = ev3_serial_open_file(EV3_SERIAL_BT);
	assert(bt != NULL);

	/* Bluetooth通信タスクの起動 */
	act_tsk(BT_TASK);

	ev3_led_set_color(LED_ORANGE); /* 初期化完了通知 */
	/////
	// 光センサー値キャリブレーション
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

	/* スタート待機 */
	while (1)
	{
		tail_control(TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */

		if (bt_cmd == 1)
		{
			bt_cmd = 0;
			break; /* リモートスタート */
		}

		if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
		{
			break; /* タッチセンサが押された */
		}

		tslp_tsk(10); /* 10msecウェイト */
	}

	RunParamReset();

	tslp_tsk(10); /* 10msecウェイト */
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
		// 走行終了(Backボタン)
		ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
		if (ev3_button_is_pressed(BACK_BUTTON))
			break;

		while (1)
		{
			tail_control(TAIL_ANGLE_STAND_UP); /* 完全停止用角度に制御 */
											   //			tail_control(tail);
			ev3_lcd_draw_string("TRAVEL SET MODE :UDLR btn:", 0, CALIB_FONT_HEIGHT * 1);

			if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
			{
				flag = true;
				// タッチセンサの揺れが収まるのを待ってからジャイロリセット
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
				// enumを循環インクリメント
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
		tslp_tsk(5); /* 10msecウェイト */

		// スタートダッシュ（前傾姿勢）
		for (int i = 0; i < 100; i++)
		{
			tail_control(TAIL_ANGLE_STAND_UP + 10);
			tslp_tsk(1);
		}

		// コース走行
		while (1)
		{
			// 走行距離、旋回角度常時計測
			// 走行距離計測
			dist = ((float)(ev3_motor_get_counts(left_motor) + ev3_motor_get_counts(right_motor)) / 2) * 0.72;
			// 旋回角度計測
			turning_angle = ((ev3_motor_get_counts(right_motor) - ev3_motor_get_counts(left_motor)) * 0.27);

			//			ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
			//			ev3_lcd_draw_string("TRAVELING MODE", 0, CALIB_FONT_HEIGHT*1);
			// 停止処理
			if (ev3_touch_sensor_is_pressed(touch_sensor) == 1)
			{
				flag = true;
			}
			else if ((ev3_touch_sensor_is_pressed(touch_sensor) == 0 && flag) ||
					 bt_cmd == 1)
			{
				forward = 0;
				turn = 0;

				// テール静止角度
				for (int i = 0; i < 10000; i++)
				{
					tail_control(TAIL_ANGLE_STAND_UP - (5 / (10001 - i)));
				}

				// 後傾移行（微直進）
				for (int i = 0; i < 50; i++)
				{
					ev3_motor_set_power(left_motor, 50);
					ev3_motor_set_power(right_motor, 50);
					tslp_tsk(1);
				}
				// モーター停止
				ev3_motor_stop(left_motor, false);
				ev3_motor_stop(right_motor, false);
				ev3_motor_stop(tail_motor, false);
				tslp_tsk(10); /* 10msecウェイト */

				// ev3_motor_reset_counts(left_motor);
				// ev3_motor_reset_counts(right_motor);
				// tslp_tsk(10); /* 10msecウェイト */
				// ev3_gyro_sensor_reset(gyro_sensor);
				// tslp_tsk(10); /* 10msecウェイト */
				// balance_init(); /* 倒立振子API初期化 */
				// tslp_tsk(10); /* 10msecウェイト */

				bt_cmd = 0;
				flag = false;
				// シナリオ内部状態の初期化
				scenario_init = true;
				break;
			}

			// 走行体状態表示
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
			tslp_tsk(4); /* 4msec周期起動 */
		}

		tslp_tsk(10); /* 10msecウェイト */
	}
	// 終了処理
	// モーター停止
	ev3_motor_stop(left_motor, false);
	ev3_motor_stop(right_motor, false);
	ev3_motor_stop(tail_motor, false);
	// Bluetooth通信タスクの終了
	ter_tsk(BT_TASK);
	fclose(bt);

	ext_tsk();
}

//*****************************************************************************
// 関数名 : initPort
// 引数 : なし
// 返り値 : なし
// 概要 : センサー、モーター初期化
//*****************************************************************************
void initPort()
{
	/* センサー入力ポートの設定 */
	ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
	ev3_sensor_config(color_sensor, COLOR_SENSOR);
	ev3_color_sensor_get_reflect(color_sensor); /* 反射率モード */
	ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
	ev3_sensor_config(gyro_sensor, GYRO_SENSOR);
	/* モーター出力ポートの設定 */
	ev3_motor_config(left_motor, LARGE_MOTOR);
	ev3_motor_config(right_motor, LARGE_MOTOR);
	ev3_motor_config(tail_motor, LARGE_MOTOR);
	ev3_motor_reset_counts(tail_motor);
}

//*****************************************************************************
// 関数名 : RunParamReset
// 引数 : なし
// 返り値 : なし
// 概要 : 走行体センサ値、モータ値、バランサー初期化
//*****************************************************************************
void RunParamReset()
{
	/* 走行モーターエンコーダーリセット */
	tslp_tsk(10); /* 10msecウェイト */
	ev3_motor_reset_counts(left_motor);
	ev3_motor_reset_counts(right_motor);
	ev3_lcd_draw_string("motor reset", 0, CALIB_FONT_HEIGHT * 2);

	/* ジャイロセンサーリセット */
	tslp_tsk(10); /* 10msecウェイト */
	ev3_gyro_sensor_reset(gyro_sensor);
	ev3_lcd_draw_string("gyro reset", 0, CALIB_FONT_HEIGHT * 3);

	tslp_tsk(10);   /* 10msecウェイト */
	balance_init(); /* 倒立振子API初期化 */
	ev3_lcd_draw_string("balance init", 0, CALIB_FONT_HEIGHT * 4);

	tslp_tsk(10);				  /* 10msecウェイト */
	ev3_led_set_color(LED_GREEN); /* スタート通知 */
	tslp_tsk(10);				  /* 10msecウェイト */
}

//*****************************************************************************
// 関数名 : sonar_alert
// 引数 : 無し
// 返り値 : 1(障害物あり)/0(障害物無し)
// 概要 : 超音波センサによる障害物検知
//*****************************************************************************
static int sonar_alert(void)
{
	static unsigned int counter = 0;
	static int alert = 0;

	signed int distance;

	if (++counter == 40 / 4) /* 約40msec周期毎に障害物検知  */
	{
		/*
		 * 超音波センサによる距離測定周期は、超音波の減衰特性に依存します。
		 * NXTの場合は、40msec周期程度が経験上の最短測定周期です。
		 * EV3の場合は、要確認
		 */
		distance = ev3_ultrasonic_sensor_get_distance(sonar_sensor);
		if ((distance <= SONAR_ALERT_DISTANCE) && (distance >= 0))
		{
			alert = 1; /* 障害物を検知 */
		}
		else
		{
			alert = 0; /* 障害物無し */
		}
		counter = 0;
	}

	return alert;
}

//*****************************************************************************
// 関数名 : tail_control
// 引数 : angle (モータ目標角度[度])
// 返り値 : 無し
// 概要 : 走行体完全停止用モータの角度制御
//*****************************************************************************
static void tail_control(signed int angle)
{
	float pwm = (float)(angle - ev3_motor_get_counts(tail_motor)) * P_GAIN; /* 比例制御 */
	/* PWM出力飽和処理 */
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
// 関数名 : bt_task
// 引数 : unused
// 返り値 : なし
// 概要 : Bluetooth通信によるリモートスタート。 Tera Termなどのターミナルソフトから、
//		 ASCIIコードで1を送信すると、リモートスタートする。
//*****************************************************************************
void bt_task(intptr_t unused)
{
	while (1)
	{
		uint8_t c = fgetc(bt); /* 受信 */
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
// 関数名 : Run
// 引数 : forward 前進値、turn 旋回値
// 返り値 : なし
// 概要 : 状態に応じた走行を行う
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
// 関数名 : CourseRun
// 引数 : forward 前進値、turn 旋回値
// 返り値 : なし
// 概要 : ライントレースによるコース走行を行う
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

	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */
	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;

	tail_control(TAIL_ANGLE_DRIVE); /* バランス走行用角度に制御 */
									//	tail_control(TAIL_ANGLE_STAND_UP - 15);
									//	tail_control(tail);	  ////// STAGE2 Look up

	//		  if (sonar_alert() == 1) /* 障害物検知 */
	if (0) // 無効
	{
		forward = turn = 0; /* 障害物を検知したら停止 */
	}
	// On/Off走行(現在未使用)
	else
	{
		forward = forward; /* 前進命令 */
						   //			if (ev3_color_sensor_get_reflect(color_sensor) >= (LIGHT_WHITE + LIGHT_BLACK)/2)
		if (ev3_color_sensor_get_reflect(color_sensor) >= (line_white + line_gray) / 2)
		{
			turn = turn; /* 左旋回命令 */
		}
		else
		{
			turn = -turn; /* 右旋回命令 */
		}
	}

	// PID 制御
	diff[0] = diff[1];
	diff[1] = ev3_color_sensor_get_reflect(color_sensor) - target;
	integral += (diff[0] + diff[1]) / 2 * 0.004;

	p = Kp * diff[1];
	i = Ki * integral;
	d = Kd * (diff[1] - diff[0]) / 0.004;

	turn = p + i + d;

	/* 倒立振子制御API に渡すパラメータを取得する */
	motor_ang_l = ev3_motor_get_counts(left_motor);
	motor_ang_r = ev3_motor_get_counts(right_motor);
	gyro = ev3_gyro_sensor_get_rate(gyro_sensor);
	volt = ev3_battery_voltage_mV();

	/* 倒立振子制御APIを呼び出し、倒立走行するための */
	/* 左右モータ出力値を得る */
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

	/* EV3ではモーター停止時のブレーキ設定が事前にできないため */
	/* 出力0時に、その都度設定する */
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
// 関数名 : LookUpGate
// 引数 : forward 前進値、turn 旋回値
// 返り値 : なし
// 概要 : ルックアップゲートを攻略する
//*****************************************************************************
void LookUpGate(bool init)
{
	// バランサー用変数
	signed char pwm_L, pwm_R; /* 左右モータPWM出力 */
	int32_t motor_ang_l, motor_ang_r;
	int gyro, volt;
	bool blance = false;

	int forward = 0;
	int turn = 0;

	// シーン内状態
	static LookUpState State = TAIL_RUN;
	// 走行距離測定用変数
	// 関数内のstaticは1回目の呼び出しだけ初期化されて2回目以降は初期化されないぞ！すごい！
	static int mMileage = (int)dist;
	static int mTurn = turning_angle;

	int nowDist;  // 走行開始から現在までの距離
	int moveDist; // 難所用測定開始時からの走行距離

	int nowTurningAngle;

	// 攻略開始時パラメタを記録
	int leftMotorAngle = ev3_motor_get_counts(right_motor);
	int rightMotorAngle = -ev3_motor_get_counts(left_motor);
	static int tailAngle = ev3_motor_get_counts(tail_motor);
	// 引数initがtrueの場合、static変数を初期化してルックアップゲート攻略前状態に戻る
	if (init)
	{
		State = TAIL_RUN;
		mMileage = (int)dist;
		mTurn = turning_angle;
		tailAngle = ev3_motor_get_counts(tail_motor);
	}

	int RADIUS = 80;
	float PI = 3.14159;

	// 現時点の走行距離
	// 左右モータエンコーダ値の平均を取得
	float aveAngle = (leftMotorAngle + rightMotorAngle) / 2;

	// 走行距離を計算
	nowDist = (int)dist;

	// 現時点の旋回値
	nowTurningAngle = (rightMotorAngle - leftMotorAngle) * 0.23;

	// 測定開始時からの走行距離
	moveDist = nowDist - mMileage;

	switch (State)
	{
	case TAIL_RUN:
	{
		// テール走行用角度に移行　
		forward = 0; // TODO
		turn = 0;	// TODO

		// テールモーター出力値を目標値との差に応じて決定
		//float tail_pwm = (float)(TAIL_ANGLE_DRIVE + 1 - tailAngle) * P_GAIN; // 比例制御
		//tail_control(tailRunAngle);
		tailAngle = tailRunAngle;

		// テールを後傾走行角度に設定した場合
		if (ev3_motor_get_counts(tail_motor) <= tailRunAngle)
		{
			State = STRAIGHT;
			mTurn = nowTurningAngle;
		}

		break;
	}
	case STRAIGHT:
		// 直進
		forward = 20;

		// ゲートを超えた場合
		if (moveDist > GATE_PASS_MILEAGE)
		{
			// 測定開始距離更新
			mMileage = nowDist;
			// 一度目は後退
			//mTravelState = (mTravelState == 2) ? 3 : 5;
			State = STAND_UP;
		}

		break;
	case TURN:
		break;
	case STAND_UP:
	{
		// 起き上がり
		forward = 0;
		turn = 0;

		// // テールモーター出力値を目標値との差に応じて決定
		// float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // 比例制御
		// // テールモーター出力値を設定
		// motorParam[2] = tail_pwm;
		tailAngle = TAIL_ANGLE_STAND_UP + 5;

		// テールを走行角度に設定した場合
		if (ev3_motor_get_counts(tail_motor) >= TAIL_ANGLE_STAND_UP + 5 - 1)
		{
			// 走行状態を更新
			State = END;
		}
		break;
	}
	default:
		forward = 3;
		turn = 0;
		tailAngle = TAIL_ANGLE_DRIVE;
		blance = true;
		/* 倒立振子制御API に渡すパラメータを取得する */

		break;
	}
	tail_control(tailAngle);
	// バランサー使用
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
	// バランサー不使用
	else
	{
		pwm_L = (signed char)forward;
		pwm_R = (signed char)forward;
	}

	/* EV3ではモーター停止時のブレーキ設定が事前にできないため */
	/* 出力0時に、その都度設定する */
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

	// 走行状態に応じて振る舞いを決定
	// switch (mTravelState) {
	// // 難所攻略開始時
	// case 0:
	// 	// case0は使用しない
	// 	// // 開始時走行距離を記録
	// 	// mMileage = nowDist;

	// 	// // 開始時旋回角度を記録
	// 	// mTurningAngle = nowTurningAngle;

	// 	// // 走行状態を次の段階へ
	// 	// mTravelState = 1;

	// 	break;
	// // テールを後傾走行角度に設定
	// case 1:
	// 	{
	// 	// 低速走行
	// 	forward = GATE_LOW_FORWARD;
	// 	turn = GATE_LOW_TURN;

	// 	// テールモーター出力値を目標値との差に応じて決定
	// 	float tail_pwm = (float)(tailRunAngle + 1 - tailAngle) * GATE_TAIL_P_GAIN; // 比例制御
	// 	// テールモーター出力値を設定
	// 	motorParam[2] = tail_pwm;

	// 	// テールを後傾走行角度に設定した場合
	// 	if (tailAngle < tailRunAngle) {
	// 		mTravelState = 2;
	// 		mTurningAngle = nowTurningAngle;
	// 	}

	// 	break;
	// 	}
	// // 指定距離だけ前進
	// case 2:
	// case 4:
	// 	// 直進
	// 	motorParam[0] = GATE_MIDDLE_FORWARD;
	// 	motorParam[1] = GATE_MIDDLE_FORWARD;

	// 	// ゲートを超えた場合
	// 	if (moveDist > GATE_PASS_MILEAGE) {
	// 		// 測定開始距離更新
	// 		mMileage = nowDist;
	// 		// 一度目は後退
	// 		mTravelState = (mTravelState == 2) ? 3 : 5;
	// 	}

	// 	break;
	// // アーチ橋通過後
	// case 3:
	// 	// 後退
	// 	motorParam[0] = -GATE_MIDDLE_FORWARD;
	// 	motorParam[1] = -GATE_MIDDLE_FORWARD;

	// 	// 指定距離戻った場合
	// 	if (moveDist < -GATE_PASS_MILEAGE) {
	// 		// 測定開始距離更新
	// 		mMileage = nowDist;

	// 		// 走行状態を更新
	// 		mTravelState = 4;
	// 	}

	// 	break;
	// // テールを走行開始角度に設定
	// case 5:
	// 	{
	// 	// 低速走行
	// 	*forward = GATE_LOW_FORWARD;
	// 	*turn = GATE_LOW_TURN;

	// 	// テールモーター出力値を目標値との差に応じて決定
	// 	float tail_pwm = (float)(GATE_TAIL_TRAVEL_ANGLE - tailAngle) * GATE_TAIL_P_GAIN; // 比例制御
	// 	// テールモーター出力値を設定
	// 	motorParam[2] = tail_pwm;

	// 	// テールを走行角度に設定した場合
	// 	if (tailAngle < GATE_TAIL_TRAVEL_ANGLE + 3) {
	// 		// 走行状態を更新
	// 		mTravelState = 6;
	// 	}

	// 	break;
	// 	}
	// // 次の難所に進む
	// case 6:
	// 	// 通常走行
	// 	*forward = GATE_MIDDLE_FORWARD;
	// 	*turn = GATE_MIDDLE_TURN;

	// 	break;
	// default:
	// 	*forward = GATE_MIDDLE_FORWARD;
	// 	*turn = GATE_MIDDLE_TURN;
	// 	break;
	// }
}
