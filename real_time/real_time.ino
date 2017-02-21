#include <DHT.h>
#include <TimerOne.h>
#include <Thread.h>
#include "Schedule.h"
#include <functional>
Thread waterThread = Thread();
Thread printThread = Thread();
Thread threadEvery5s = Thread();


#define RELAY_PIN 25
#define WATER_PIN 18
#define BEEP_PIN 22
#define DH11_PIN A0
#define WATER_LEVEL_PIN A1
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
volatile int  lastDH11_Temperature = 0;
volatile int  lastDH11_Humidity = 0;
volatile int waterLevel_1 = 0;

void printTime() {


	Serial.print(String(schedule.hour) + ":" + String(schedule.minute) + ":" + String(schedule.second));
	Serial.println("\tTemperature: " + String(lastDH11_Temperature) + "C, Humidity: " + String(lastDH11_Humidity) + "%");


	//	Serial.println("items.size:  " + String(schedule.items.size() ) );

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
	threadEvery5s.run();
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



	Timer1.initialize(10000);
	Timer1.attachInterrupt(timer1_action);

	schedule.addTask("01:12", onLamp);
	schedule.addTask("01:12:30", offLamp);

	schedule.addTask("00:09", pumpOff);
	schedule.addTask("MO TU WE TH FR SA SU, 00:08:50", pumpOn);

	pumpOff();

}

void pumpOn() {
	digitalWrite(RELAY_PIN, HIGH);
	Serial.println("  Everyday pumpOn");
}
void pumpOff() {
	digitalWrite(RELAY_PIN, LOW);
	Serial.println("  pumpOn2 pumpOff");
}



void loop()
{

	if (printThread.shouldRun())
		printThread.run(); // запускаем поток

	if (waterThread.shouldRun())
		waterThread.run(); // запускаем поток

	if (threadEvery5s.shouldRun())
		threadEvery5s.run(); // запускаем поток
//	tone(22, 2000, 500);
	//tone1.play(NOTE_B2,1000);

}

