#ifndef _BUTTONS_h
#define _BUTTONS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h" 
#else
#include "WProgram.h"
#endif
#include "Constans.h"

byte buttons[] = { BUTTON_PUMP, BUTTON_LIGHT, BUTTON_3, BUTTON_4 };
#define NUMBUTTONS sizeof(buttons)
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];



void checkButtons()
{
	static byte previousstate[NUMBUTTONS];
	static byte currentstate[NUMBUTTONS];
	static long lasttime;
	byte index;
	if (millis() < lasttime) {
		// we wrapped around, lets just try again
		lasttime = millis();
	}
	if ((lasttime + DEBOUNCE) > millis()) {
		// not enough time has passed to debounce
		return;
	}
	// ok we have waited DEBOUNCE milliseconds, lets reset the timer
	lasttime = millis();
	for (index = 0; index < NUMBUTTONS; index++) {
		justpressed[index] = 0;       //when we start, we clear out the "just" indicators
		justreleased[index] = 0;
		currentstate[index] = digitalRead(buttons[index]);   //read the button

		if (currentstate[index] != previousstate[index]) {
			if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
				// just pressed
				justpressed[index] = 1;
			}
			else if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
				justreleased[index] = 1; // just released
			}
			pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
		}
		previousstate[index] = currentstate[index]; //keep a running tally of the buttons
	}
}

byte getPressedButton() {
	byte thisSwitch = 255;
	checkButtons();  //check the switches &amp; get the current state
	for (byte i = 0; i < NUMBUTTONS; i++) {
		current_keystate[i] = justreleased[i];
		if (current_keystate[i] != previous_keystate[i]) {
			if (current_keystate[i]) {
				thisSwitch = i;
			}
		}
		previous_keystate[i] = current_keystate[i];
	}
	return thisSwitch;
}


void button1Press() {
	tone(BEEP_PIN, 5000, 200);
	if (pump_state != PUMP_STATES::WORKING) {
		pumpOnWithoutSms();
	}
	else {
		pumpOffWithoutSms();
	}
	Serial.println("switch 1 just pressed");
}

void button2Press() {
	tone(BEEP_PIN, 4500, 200);
	//showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "button2 press");
	Serial.println("switch 2 just pressed");
}

void button3Press() {
	tone(BEEP_PIN, 4000, 200);
	Serial.println("switch 3 just pressed");
	if (isValveOpenZone1) {
		Serial.println("rele HIGH");
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "Valve s1 closed ");
		digitalWrite(RELAY3_PIN, HIGH);
		isValveOpenZone1 = false;
	}
	else {
		Serial.println("rele LOW");
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "Valve s1 opened ");
		digitalWrite(RELAY3_PIN, LOW);
		isValveOpenZone1 = true;
	}
}

void button4Press() {
	tone(BEEP_PIN, 3500, 200);	
	Serial.println("switch 4 just pressed");
	if (isValveOpenZone2) {
		Serial.println("rele HIGH");
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "Valve s2 closed ");
		digitalWrite(RELAY4_PIN, HIGH);
		
		isValveOpenZone2 = false;
	}
	else {		
		Serial.println("rele LOW");
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "Valve s2 opened ");
		digitalWrite(RELAY4_PIN, LOW);
		isValveOpenZone2 = true;
	}
}



#endif

