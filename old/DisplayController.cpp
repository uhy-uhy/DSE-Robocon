/** 
 * �t�@�C���� : DisplayController.cpp
 *
 *
 **/

#include "DisplayController.h"

/** 
 * �R���X�g���N�^
 *
 *
 **/
DisplayController::DisplayController() {
}

/** 
 * �f�X�g���N�^
 *
 *
 **/
DisplayController::~DisplayController() {
}

/** 
 * LCD�փ��O�\��
 *
 *
 **/
void DisplayController::mShowLog(const char* const message) {
	// LCD������
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	
	// ���b�Z�[�W�\��
	int size = sizeof(message)/sizeof(message[0]);
	for(int i = 0; i < size; i++) {
		ev3_lcd_draw_string(message[i], 0, CALIB_FONT_HEIGHT*(i+1));
	}
}