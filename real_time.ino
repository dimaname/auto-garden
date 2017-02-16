
#include <TimerOne.h>
#include <Thread.h>
#include "Schedule.h"
#include <functional>
Thread waterThread = Thread();
Thread printThread = Thread();



#define RELAY_PIN 25
#define WATER_PIN 18
// создаём объект для работы с часами реального времени
RTC clock;
Schedule schedule(clock);


void printTime() {

	Serial.println(String(schedule.hour) + ":"+ String(schedule.minute)+":"+ String(schedule.second));
	
//	Serial.println("items.size:  " + String(schedule.items.size() ) );
	
}

void timer1_action() {		
	schedule.tact();
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
void rpm(){  
	NbTopsFan++;  
	Serial.println("NbTopsFan: "+String(NbTopsFan) );
}
double totalWater = 0;
unsigned long prevCallTime =0;

void calcWater() {
	cli();
	double litersPerSec = NbTopsFan* (1000.0 / (millis() - prevCallTime)) /4.5/60;
	prevCallTime = millis();
	NbTopsFan = 0;
	totalWater += litersPerSec;
	Serial.println( "litersPerSec: "+String(litersPerSec)+"      totalWater: "+ String(totalWater));
	sei();
}

void setup()
{
	pinMode(WATER_PIN, INPUT);
	pinMode(RELAY_PIN, OUTPUT);
	attachInterrupt(5, rpm, RISING);
	waterThread.onRun( calcWater );
	waterThread.setInterval(1000);

	printThread.onRun( printTime );
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



}
