// LcdContent.h

#ifndef _LCDCONTENT_h
#define _LCDCONTENT_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class LcdContent
{
protected:


public:
	enum MODES { INTRO, NORMAL, WATERING };
	String FirstRow;
	String SecondRow;
	MODES Mode;
	bool hasNew;
	LcdContent(char* s1, char* s2) {
		FirstRow = String(s1);
		SecondRow = String(s2);
		hasNew = true;
		Mode = INTRO;
	}
	LcdContent() {
		FirstRow = String("");
		SecondRow = String("");
		hasNew = true;
		Mode = INTRO;
	}
	void set(char* s1, char* s2, MODES m) {
		if (Mode == m) {
			FirstRow = String(s1);
			SecondRow = String(s2);
			hasNew = true;
		}

	}
	void set(String s1, String s2, MODES m) {	
		if (Mode == m) {
			FirstRow = s1;
			SecondRow = s2;
			hasNew = true;
		}
	}
};


#endif

