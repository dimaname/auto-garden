#include <DHT.h>
#include <TimerOne.h>
#include <Thread.h>
#include "LcdContent.h"
#include "Schedule.h"
#include "Buttons.h"
#include <functional>
#include <LiquidCrystal.h>
#include <Bounce2.h>
#include <SimpleTimer.h>
#include "Constans.h"
#include <SoftwareSerial.h>
#include <Sim800l.h>



Thread threadEvery1s = Thread();
Thread threadEvery5s = Thread();




LiquidCrystal LCD16x2(LCD_RS_ORANGE, LCD_E_YELLOW, LCD_D4_GREEN, LCD_D5_BLUE, LCD_D6_PUPRPLE, LCD_D7_GRAY);
LcdContent lcdContent = LcdContent();
SimpleTimer lcdMessageTimer, lcdLightTimer, pumpOffTimer;

int pumpOffTimerId, lcdLightTimerId, lcdMessageTimerId;
unsigned long watering_internal = 30;   // время полива
unsigned long pumpOnTimeStamp = 0;

String watering_animate[4] = { "\x97", "\x96", "\x95", "\x94" };
const float waterFlowK = 52;   // подобпать коэф. на месте. (дома на 5 литрах было ~85 или 52)
volatile int waterLevel_1 = 0;


Sim800l GSM;
char* TRUSTED_NUMBERS[] = { "79617638670" };
int lastHostNumberIndex = 0;

DHT dh11(DH11_PIN, DHT11);

volatile int lastDH11_Temperature = 0;
volatile int lastDH11_Humidity = 0;
volatile int lastLightSensorState = 1;


void setup()
{
	Serial.begin(19200);
	GSM.begin();

	digitalWrite(RELAY1_PIN, HIGH);
	pinMode(RELAY1_PIN, OUTPUT);
	pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);
	pinMode(LIGHT_SENSOR, INPUT);
	pinMode(LCD_LIGHT_RED, OUTPUT);
	dh11.begin();

	Timer1.initialize(1000000);
	Timer1.attachInterrupt(timer1_action);


	////// датчик потока
   // pinMode(WATER_FLOW_PIN, INPUT);
   // attachInterrupt(5, rpm, RISING);
	///////


	threadEvery5s.onRun(threadEvery5sAction);
	threadEvery5s.setInterval(5000);

	threadEvery1s.onRun(threadEvery1sAction);
	threadEvery1s.setInterval(1000);

	// инициализация часов
	clock.begin();
	// метод установки времени и даты в модуль вручную
	// clock.set(10,25,45,27,07,2005,THURSDAY);
	// метод установки времени и даты автоматически при компиляции
	clock.set(__TIMESTAMP__);

	LCD16x2.begin(16, 2);
	LCD16x2.command(0b101010);

	showLcdMessage(5000, 15000, LcdContent::MESSAGE, "     \xcf\xd0\xc8\xc2\xc5\xd2\x2c", "\xc3\xce\xd2\xce\xc2\xc0 \xca \xd0\xc0\xc1\xce\xd2\xc5\x90");


	// кнопки
	pinMode(BUTTON_PUMP, INPUT);
	pinMode(BUTTON_LIGHT, INPUT);
	pinMode(BUTTON_3, INPUT);
	pinMode(BUTTON_4, INPUT);
		
	//"MO TU WE TH FR SA SU, 15:32:00"
	taskWateringId = schedule.addTask("18:19:00", pumpOn);

	pumpOff();
	threadEvery5s.run();
}


void loop()
{

	if (threadEvery1s.shouldRun())
		threadEvery1s.run(); // запускаем поток		

	if (threadEvery5s.shouldRun())
		threadEvery5s.run(); // запускаем поток
	
	pumpOffTimer.run();
	
	// гасим дребезг контактов
	byte pressedButton = getPressedButton();

	switch (pressedButton)
	{
	case 0:
		button1Press();
		break;
	case 1:
		button2Press();
		break;
	case 2:
		button3Press();
		break;
	case 3:
		button4Press();
		break;
	}


	//	tone(22, 2000, 500);
	//tone1.play(NOTE_B2,1000);

}


void threadEvery1sAction() {

	int lightSensorState = digitalRead(LIGHT_SENSOR);
	if (lightSensorState != lastLightSensorState) {
		lastLightSensorState = lightSensorState;
		tone(BEEP_PIN, 5500, 200);
		Serial.println(" LIGHT_SENSOR: " + String(lightSensorState));
	}


	//calcWater();
}



void threadEvery5sAction() {
//	checkIncomingSMS();	
	lastDH11_Temperature = dh11.readHumidity();
	lastDH11_Humidity = dh11.readTemperature();
	if (isnan(lastDH11_Temperature) || isnan(lastDH11_Humidity)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}
}

void timer1_action() {
	schedule.tact();
	checkWaterLevel();
	Serial.print(schedule.timeStr);
	lcdContentBuilder();
	lcdRunner();
}

void lcdContentBuilder() {

	Serial.println("\tTemperature: " + String(lastDH11_Temperature) + "C, Humidity: " + String(lastDH11_Humidity) + "%");
	String addSpaceT = "", addSpaceH = "", animateframe, _first = "", _second = "";
	if (lastDH11_Temperature < 10) {
		addSpaceT = " ";
	}
	if (lastDH11_Humidity < 10) {
		addSpaceH = " ";
	}

	unsigned long diff;
	switch (lcdContent.Mode)
	{

	case  LcdContent::MESSAGE:
		_first = lcdContent.FirstRow;
		_second = lcdContent.SecondRow;
		break;
	case  LcdContent::MESSAGE_HALF:
		_first = String(schedule.timeStr) + " " + addSpaceT + String(lastDH11_Temperature) + "\xb0 " + addSpaceH + String(lastDH11_Humidity) + "%";
		_second = lcdContent.SecondRow;
		break;
	case  LcdContent::WATERING:
		diff = (unsigned long)watering_internal - (millis() - pumpOnTimeStamp) / 1000L;
		animateframe = watering_animate[diff % 4];
		_first = String(schedule.timeStr) + " " + addSpaceT + String(lastDH11_Temperature) + "\xb0 " + addSpaceH + String(lastDH11_Humidity) + "%";
		_second = " \xef\xee\xeb\xe8\xe2 " + animateframe + " " + distanceFormat(diff);

		break;

	case  LcdContent::NORMAL:
		_first = String(schedule.timeStr) + " " + addSpaceT + String(lastDH11_Temperature) + "\xb0 " + addSpaceH + String(lastDH11_Humidity) + "%";
		_second = "    \xef\xf3\xf1\xea " + distanceFormat(schedule.timeLeftFor(taskWateringId));
		break;
	}

	lcdContent.set(_first, _second, lcdContent.Mode);

}

void checkWaterLevel() {
	sei();
	int tmp = digitalRead(WATER_LEVEL_PIN);
	if (waterLevel_1 != tmp) {
		waterLevel_1 = tmp;
		if (waterLevel_1 == HIGH) {
			tone(BEEP_PIN, 3000, 200);
			Serial.println("WATER FULL!");
		}
		else {
			tone(BEEP_PIN, 2000, 200);
			Serial.println("WATER Empty!");
			pumpOffEmergency();
		}
	}
	cli();
}

volatile int NbTopsFan;
void rpm() {
	cli();
	NbTopsFan++;
	sei();
	Serial.println("NbTopsFan: " + String(NbTopsFan));
}
double totalWater = 0;
unsigned long prevCallTime = 0;

void calcWater() {

	if (NbTopsFan != 0) {
		cli();
		double litersPerSec = NbTopsFan / waterFlowK * (1000.0 / (millis() - prevCallTime));

		Serial.println("litersPerSec: " + String(litersPerSec) + "      totalWater: " + String(totalWater));
		NbTopsFan = 0;
		totalWater += litersPerSec;
		prevCallTime = millis();
		sei();
	}
}

void pumpOn() {
	if (pump_state != PUMP_STATES::WORKING) {
		lcdLightOn(5000);

		if (waterLevel_1 == LOW) {
			sendMessage("Warning! Can't start watering. No water.", true);
			showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "\xcd\xe5\xeb\xfc\xe7\xff! \xcd\xe5\xf2 \xe2\xee\xe4\xfb");
			return;
		}
		pump_state = PUMP_STATES::WORKING;
		lcdContent.Mode = LcdContent::WATERING;
		digitalWrite(RELAY1_PIN, LOW);
		pumpOnTimeStamp = millis();
		pumpOffTimerId = pumpOffTimer.setTimeout((unsigned long)watering_internal * 1000L, pumpOff);
		sendMessage("Watering start.");
	}

}
void pumpOff() {
	pumpOffTimer.deleteTimer(pumpOffTimerId);
	if (pump_state != PUMP_STATES::WAITING) {
		lcdLightOn(5000);
		pump_state = PUMP_STATES::WAITING;
		lcdContent.Mode = LcdContent::NORMAL;
		digitalWrite(RELAY1_PIN, HIGH);
		sendMessage("Watering finish.");
	}

}

void pumpOffEmergency() {
	pumpOffTimer.deleteTimer(pumpOffTimerId);
	if (pump_state != PUMP_STATES::WAITING) {
		lcdLightOn(5000);
		pump_state = PUMP_STATES::WAITING;
		lcdContent.Mode = LcdContent::NORMAL;
		digitalWrite(RELAY1_PIN, HIGH);
		sendMessage("Warning! Emergency stop watering. No water.", true);
		showLcdMessage(3000, 5000, LcdContent::MESSAGE_HALF, "\xd1\xf2\xee\xef! \xcd\xe5\xf2 \xe2\xee\xe4\xfb!");
	}
}

void lcdLightOn(int timer) {
	digitalWrite(LCD_LIGHT_RED, HIGH);
	lcdLightTimer.deleteTimer(lcdLightTimerId);
	lcdLightTimerId = lcdLightTimer.setTimeout(timer, lcdLightOff);
}

void lcdLightOff() {
	digitalWrite(LCD_LIGHT_RED, LOW);
}


void showLcdMessage(int showTimeout, int lightTimeout, LcdContent::MODES mode, char *msg0 = "", char *msg1 = "") {
	if (lcdContent.Mode == LcdContent::WATERING) {
		return;
	}

	lcdMessageTimer.deleteTimer(lcdMessageTimerId);
	lcdMessageTimerId = lcdMessageTimer.setTimeout(showTimeout, hideLcdMessage);

	if (mode == LcdContent::MESSAGE_HALF) {
		lcdContent.Mode = mode;
		lcdContent.setSecondLine(msg0, mode);
	}
	else if (mode == LcdContent::MESSAGE) {
		lcdContent.Mode = mode;
		lcdContent.set(msg0, msg1, mode);
	}

	lcdLightOn(lightTimeout);
	lcdContentBuilder();
	lcdRunner();
}


void hideLcdMessage() {
	lcdMessageTimer.deleteTimer(lcdMessageTimerId);
	if (lcdContent.Mode == LcdContent::MESSAGE || lcdContent.Mode == LcdContent::MESSAGE_HALF)
		lcdContent.Mode = LcdContent::NORMAL;
	lcdContentBuilder();
	lcdRunner();
}

void lcdRunner() {
	lcdMessageTimer.run();
	lcdLightTimer.run();
	if (lcdContent.hasNew) {
		lcdContent.hasNew = false;
		LCD16x2.setCursor(0, 0);
		LCD16x2.print(lcdContent.FirstRow);
		LCD16x2.setCursor(0, 1);
		LCD16x2.print(lcdContent.SecondRow);

	}

}

String distanceFormat(unsigned long distance_in_second) {

	int s = distance_in_second % 60;
	distance_in_second /= 60;
	int m = distance_in_second % 60;
	distance_in_second /= 60;
	int h = distance_in_second % 24;
	distance_in_second /= 24;
	int d = distance_in_second;

	// Serial.println("d: " + String(d) + "  h:" + String(h) + "  m:" + String(m) + +"  s:" + String(s));

	String _d = d < 10 ? "0" + String(d) : String(d);
	String _h = h < 10 ? "0" + String(h) : String(h);
	String _m = m < 10 ? "0" + String(m) : String(m);
	String _s = s < 10 ? "0" + String(s) : String(s);

	String result = String();
	String addSpaces = "       ";

	if (d > 0) {
		result = String(d) + "\xe4 ";
		result += h > 0 ? _h + "\xf7" : (m > 0 ? _m + "\xec" : _s + "\xf1");

	}
	else {
		if (h > 0) {
			result = String(h) + "\xf7 ";
			result += m > 0 ? _m + "\xec" : _s + "\xf1";
		}
		else {
			result = _m + "\xec " + _s + "\xf1";
		}

	}

	if ((int)7 - (int)result.length() > 0) {
		result = addSpaces.substring(0, 7 - result.length()) + result;
	}
	return String(result);
}


void checkIncomingSMS() {

	String textSms = GSM.readSms(1);
	if (textSms && textSms.length()) {
		String numberSms = GSM.getNumberSms(1);
		Serial.println("numberSms: " + numberSms);
		Serial.println("textSms: " + textSms);

		int numberIndex = numberIndexInTrustedList(numberSms);
		if (numberIndex != -1) {
			lastHostNumberIndex = numberIndex;
			processSmsCommand(textSms);
		}
		else {
			Serial.println("Not trusted number: " + numberSms);
		}
		GSM.delAllSms();
	}
}

void processSmsCommand(String smsText) {
	smsText.toUpperCase();

	if (smsText.indexOf("HELP") != -1)
	{
		Serial.println("HELP");
	}
	else if (smsText.indexOf("PUMP ON") != -1 || smsText.indexOf("PUMPON") != -1)
	{
		pumpOn();
	}
	else if (smsText.indexOf("PUMP OFF") != -1 || smsText.indexOf("PUMPOFF") != -1)
	{
		pumpOff();
	}
	else if (smsText.indexOf("LIGHT ON") != -1 || smsText.indexOf("LIGHTON") != -1)
	{
		Serial.println("LIGHT Off!");
	}
	else if (smsText.indexOf("START AT") != -1 || smsText.indexOf("STARTAT") != -1)
	{
		Serial.println("START AT!");
	}


};


int numberIndexInTrustedList(String number) {
	unsigned length = sizeof(TRUSTED_NUMBERS) / sizeof(TRUSTED_NUMBERS[0]);
	int result = -1;
	for (int i = 0; i < length; i++)
	{
		if (number.endsWith(TRUSTED_NUMBERS[i])) {
			result = i;
			break;
		}
	}
	return result;
};



void sendMessage(char* message, bool isSendToEachHost = false) {

	char* fullMessage = addTimePrefix(message);
	Serial.println("isDisabledGSM: " + String(isDisabledGSM));
	Serial.println("\tSending sms: " + String(TRUSTED_NUMBERS[lastHostNumberIndex]) + "\r\n\t" + fullMessage);
	if (isSendToEachHost && !isDisabledGSM) {
		unsigned length = sizeof(TRUSTED_NUMBERS) / sizeof(TRUSTED_NUMBERS[0]);
		for (int i = 0; i < length; i++)
		{
			GSM.sendSms(TRUSTED_NUMBERS[i], fullMessage);
		}

	}
	else if (!isDisabledGSM) {
		GSM.sendSms(TRUSTED_NUMBERS[lastHostNumberIndex], fullMessage);
	}

}

char* addTimePrefix(char* str) {
	char* _timeStr = schedule.timeStr;
	int bufferSize = strlen(_timeStr) + strlen(str) + 2;
	char* concatString = new char[bufferSize];
	strcpy(concatString, _timeStr);
	strcat(concatString, " ");
	strcat(concatString, str);
	return concatString;
}