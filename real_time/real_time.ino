#include "LcdContent.h"
#include <SimpleTimer.h>
#include <DHT.h>
#include <TimerOne.h>
#include <Thread.h>
#include "Schedule.h"
#include <functional>
#include <LiquidCrystal.h>



Thread waterThread = Thread();
Thread printThread = Thread();
Thread threadEvery5s = Thread();


#define RELAY_PIN 25
#define WATER_PIN 18
#define BEEP_PIN 22
#define DH11_PIN A0
#define WATER_LEVEL_PIN A1


enum PUMP_STATES { WAITING, WORKING };
PUMP_STATES pump_state = PUMP_STATES::WAITING;
LiquidCrystal LCD16x2(4, 5, 10, 11, 12, 13);
LcdContent lcdContent = LcdContent();
SimpleTimer lcdIntroTimer, pumpOffTimer;
int pumpOffTimerId;
unsigned long watering_internal = 30;
unsigned long pumpOnTimeStamp = 0;
String watering_animate[4] = { "\x97","\x96","\x95","\x94" };
// создаём объект для работы с часами реального времени
RTC clock;
Schedule schedule(clock);

DHT dh11(DH11_PIN, DHT11);
const byte COUNT_NOTES = 39; // Колличество нот




int frequences[COUNT_NOTES] = {
	392, 392, 392, 311, 466, 392, 311, 466, 392,
	587, 587, 587, 622, 466, 369, 311, 466, 392,
	784, 392, 392, 784, 739, 698, 659, 622, 659,
	415, 554, 523, 493, 466, 440, 466,
	311, 369, 311, 466, 392
};
int durations[COUNT_NOTES] = {
	350, 350, 350, 250, 100, 350, 250, 100, 700,
	350, 350, 350, 250, 100, 350, 250, 100, 700,
	350, 250, 100, 350, 250, 100, 100, 100, 450,
	150, 350, 250, 100, 100, 100, 450,
	150, 350, 250, 100, 750
};
volatile int lastDH11_Temperature = 0;
volatile int lastDH11_Humidity = 0;
volatile int waterLevel_1 = 0;

int taskWateringId;



void printTime() {


	Serial.print(schedule.timeStr);
	Serial.println("\tTemperature: " + String(lastDH11_Temperature) + "C, Humidity: " + String(lastDH11_Humidity) + "%");
	String addSpaceT = "", addSpaceH = "";
	if (lastDH11_Temperature < 10) {
		addSpaceT = " ";
	}
	if (lastDH11_Humidity < 10) {
		addSpaceH = " ";
	}


	lcdContent.set(String(schedule.timeStr) + " " + addSpaceT + String(lastDH11_Temperature) + "\xb0 " + addSpaceH + String(lastDH11_Humidity) + "%",
		String("    \xef\xf3\xf1\xea " + distanceFormat(schedule.timeLeftFor(taskWateringId))), LcdContent::NORMAL);


	if (lcdContent.Mode == LcdContent::WATERING) {
		unsigned long diff = (unsigned long)watering_internal - (millis() - pumpOnTimeStamp) / 1000L;
		String animateframe = watering_animate[diff % 4];

		lcdContent.set(String(schedule.timeStr) + " " + addSpaceT + String(lastDH11_Temperature) + "\xb0 " + addSpaceH + String(lastDH11_Humidity) + "%",
			String(" \xef\xee\xeb\xe8\xe2 " + animateframe + " " + distanceFormat(diff)), LcdContent::WATERING);

	}




}



void threadEvery5sAction() {


	lastDH11_Temperature = dh11.readHumidity();
	lastDH11_Humidity = dh11.readTemperature();

	// Check if any reads failed and exit early (to try again).
	if (isnan(lastDH11_Temperature) || isnan(lastDH11_Humidity)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}


}

void timer1_action() {
	schedule.tact();

	int tmp = digitalRead(WATER_LEVEL_PIN);
	if (waterLevel_1 != tmp) {
		waterLevel_1 = tmp;
		tone(22, 2000, 500);
		if (waterLevel_1 == HIGH) {
			Serial.println("WATER FULL!");
		}
		else {
			Serial.println("WATER EMPTY!");

		}
	}



}

void onLamp() {
	digitalWrite(RELAY_PIN, HIGH);
	Serial.println(" onLamp");
}
void offLamp() {
	digitalWrite(RELAY_PIN, LOW);
	Serial.println(" offLamp");
}


volatile int NbTopsFan;
void rpm() {
	NbTopsFan++;
	Serial.println("NbTopsFan: " + String(NbTopsFan));
}
double totalWater = 0;
unsigned long prevCallTime = 0;

void calcWater() {
	cli();
	double litersPerSec = NbTopsFan* (1000.0 / (millis() - prevCallTime)) / 4.5 / 60;
	prevCallTime = millis();
	NbTopsFan = 0;
	totalWater += litersPerSec;
	Serial.println("litersPerSec: " + String(litersPerSec) + "      totalWater: " + String(totalWater));
	sei();
}



void setup()
{
	pinMode(WATER_PIN, INPUT);
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);
	dh11.begin();


	//	tone1.begin(BEEP_PIN);

	//	attachInterrupt(5, rpm, RISING);
	//	waterThread.onRun( calcWater );
	//	waterThread.setInterval(1000);

	threadEvery5s.onRun(threadEvery5sAction);
	threadEvery5s.setInterval(5000);

	printThread.onRun(printTime);
	printThread.setInterval(1000);
	// открываем последовательный порт
	Serial.begin(9600);
	// инициализация часов
	clock.begin();
	// метод установки времени и даты в модуль вручную
	// clock.set(10,25,45,27,07,2005,THURSDAY);
	// метод установки времени и даты автоматически при компиляции
	clock.set(__TIMESTAMP__);

	LCD16x2.begin(16, 2);
	LCD16x2.command(0b101010);
	lcdIntroTimer.setTimeout(5000, stopIntro);
	lcdContent.set("     \xcf\xd0\xc8\xc2\xc5\xd2\x2c",
		"\xc3\xce\xd2\xce\xc2\xc0 \xca \xd0\xc0\xc1\xce\xd2\xc5\x90", LcdContent::INTRO);

	Timer1.initialize(10000);
	Timer1.attachInterrupt(timer1_action);

	schedule.addTask("01:12", onLamp);
	schedule.addTask("01:12:30", offLamp);

	schedule.addTask("00:09", pumpOff);
	taskWateringId = schedule.addTask("22:05:00", pumpOn);

	pumpOff();
	threadEvery5s.run();
}


void loop()
{

	if (printThread.shouldRun())
		printThread.run(); // запускаем поток

	if (waterThread.shouldRun())
		waterThread.run(); // запускаем поток

	if (threadEvery5s.shouldRun())
		threadEvery5s.run(); // запускаем поток

	lcdRunner();
	pumpOffTimer.run();
	//	tone(22, 2000, 500);
		//tone1.play(NOTE_B2,1000);

}



void pumpOn() {
	if (pump_state != PUMP_STATES::WORKING) {
		Serial.println("~~~~~~~~~~~~ pumpOn ~~~~~~~~~~~~");
		pump_state = PUMP_STATES::WORKING;
		lcdContent.Mode = LcdContent::WATERING;
		digitalWrite(RELAY_PIN, HIGH);
		pumpOnTimeStamp = millis();
		pumpOffTimerId = pumpOffTimer.setTimeout((unsigned long)watering_internal * 1000L, pumpOff);
	}

}
void pumpOff() {
	pumpOffTimer.deleteTimer(pumpOffTimerId);

	if (pump_state != PUMP_STATES::WAITING) {
		Serial.println("---------- pumpOff -----------");
		pump_state = PUMP_STATES::WAITING;
		lcdContent.Mode = LcdContent::NORMAL;
		digitalWrite(RELAY_PIN, LOW);
	}

}

void stopIntro() {
	LCD16x2.clear();
	lcdContent.Mode = LcdContent::NORMAL;

}

void lcdRunner() {
	lcdIntroTimer.run();
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