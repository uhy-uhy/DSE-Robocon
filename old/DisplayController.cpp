/** 
 * ファイル名 : DisplayController.cpp
 *
 *
 **/

#include "DisplayController.h"

/** 
 * コンストラクタ
 *
 *
 **/
DisplayController::DisplayController() {
}

/** 
 * デストラクタ
 *
 *
 **/
DisplayController::~DisplayController() {
}

/** 
 * LCDへログ表示
 *
 *
 **/
void DisplayController::mShowLog(const char* const message) {
	// LCD初期化
	ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
	
	// メッセージ表示
	int size = sizeof(message)/sizeof(message[0]);
	for(int i = 0; i < size; i++) {
		ev3_lcd_draw_string(message[i], 0, CALIB_FONT_HEIGHT*(i+1));
	}
}