/** 
 * ファイル名 : LineMonitor.h
 *
 *
 **/

#ifndef LINE_MONITOR
#define LINE_MONITOR

///// 検証用局所定数
const int BLACK = 0;
const int GRAY = 20;
const int WHITE = 60;
const int BWTH = (WHITE + BLACK) / 2;
// const int BGTH = (BLACK + GRAY) / 2;
///// TODO : 調整が必要
const int BGTH = 40;
// const int GWTH = (GRAY + WHITE) / 2;
///// TODO : 調整が必要
const int GWTH = 50;
/////

///// TODO : 要調査
const int LOG_SIZE = 100; // 所持するログのサイズ
const int GTH = 30; // 灰色値と判定する割合

class LineMonitor {
private:
	int mColorLog[LOG_SIZE];
	
public:
	LineMonitor();
	~LineMonitor();
	
	int mDecideColor(int lightParam);
};

#endif
