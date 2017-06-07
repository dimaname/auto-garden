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
	enum MODES { NORMAL, WATERING, MESSAGE, MESSAGE_HALF, STOP };
	String FirstRow;
	String SecondRow;
	MODES Mode;
	bool hasNew;
	LcdContent(char* s1, char* s2) {
		FirstRow = String(s1);
		SecondRow = String(s2);
		hasNew = true;
		Mode = NORMAL;
	}
	LcdContent() {
		FirstRow = String("");
		SecondRow = String("");
		hasNew = true;
		Mode = STOP;
	}
	void set(char* s1, char* s2, MODES m) {
		if (Mode == m) {
			FirstRow = addSpaces(String(s1));
			SecondRow = addSpaces(String(s2));
			hasNew = true;
		}

	}
	void set(String s1, String s2, MODES m) {
		if (Mode == m) {
			FirstRow = addSpaces(s1);
			SecondRow = addSpaces(s2);
			hasNew = true;
		}
	}

	void setSecondLine(String s, MODES m) {
		if (Mode == m) {			
			SecondRow = addSpaces(s);
			hasNew = true;
		}
	}
	void setFirstLine(String s, MODES m) {
		if (Mode == m) {
			FirstRow = addSpaces(s);		
			hasNew = true;
		}
	}


	String addSpaces(String str) {
		int length = str.length();
		for (int i = length; i < 16; i++) {
			str += " ";
		}
		return str;
	};

};


#endif

