#ifndef VALVE_H
#define VALVE_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h" 
#else
#include "WProgram.h"
#endif

#include "Constans.h"
class Valve
{
public:
	bool isOpened;
	int relePin;
	String name;
	
	Valve() {	
	}

	Valve(int _relePin, String _name) {
		relePin = _relePin;
		name = _name;
		this->openValve();
	}

	void openValve() {
		String msg = "Valve " + name + " opened ";
		Serial.println(msg);
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, (char*)msg.c_str());
		digitalWrite(relePin, HIGH);
		isOpened = true;
	}

	void closeValve() {
		String msg = "Valve " + name + " closed ";
		Serial.println(msg);
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, (char*)msg.c_str());
		digitalWrite(relePin, LOW);
		isOpened = false;
	}

	void toggleValve() {
		if (this->isOpened) {
			this->closeValve();
		}
		else {
			this->openValve();
		}
	}

};
#endif 