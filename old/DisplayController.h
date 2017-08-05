/** 
 * ファイル名 : DisplayController.h
 *
 *
 **/

#ifndef DISPLAY_CONTROLLER
#define DISPLAY_CONTROLLER

#include "ev3api.h"

// LCDフォントサイズ
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)

class DisplayController {
public:
	DisplayController();
	~DisplayController();
	
	void mShowLog(const char* const message);
};

#endif
