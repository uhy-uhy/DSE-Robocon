/** 
 * �t�@�C���� : LineMonitor.h
 *
 *
 **/

#ifndef LINE_MONITOR
#define LINE_MONITOR

///// ���ؗp�Ǐ��萔
const int BLACK = 0;
const int GRAY = 20;
const int WHITE = 60;
const int BWTH = (WHITE + BLACK) / 2;
// const int BGTH = (BLACK + GRAY) / 2;
///// TODO : �������K�v
const int BGTH = 40;
// const int GWTH = (GRAY + WHITE) / 2;
///// TODO : �������K�v
const int GWTH = 50;
/////

///// TODO : �v����
const int LOG_SIZE = 100; // �������郍�O�̃T�C�Y
const int GTH = 30; // �D�F�l�Ɣ��肷�銄��

class LineMonitor {
private:
	int mColorLog[LOG_SIZE];
	
public:
	LineMonitor();
	~LineMonitor();
	
	int mDecideColor(int lightParam);
};

#endif
