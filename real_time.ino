#include <SimpleTimer.h>
#include <TimerOne.h>
#include <Thread.h>
#include "Schedule.h"

Thread timeThread = Thread(); 
Thread printThread = Thread();



#define RELAY_PIN 25

// создаём объект для работы с часами реального времени
RTC clock;
Schedule schedule(clock);


SimpleTimer timer;
void printTime() {

	Serial.println(String(schedule.hour) + ":"+ String(schedule.minute)+":"+ String(schedule.second));
	
//	Serial.println("items.size:  " + String(schedule.items.size() ) );
	
}

void timer1_action() {		
	schedule.tact();
}

void onLamp() {
	digitalWrite(RELAY_PIN, HIGH);
	timer.setTimeout(1000, offLamp);
	Serial.println(" onLamp");
}
void offLamp() {
	digitalWrite(RELAY_PIN, LOW);	
	Serial.println(" offLamp");
}
void setup()
{
	pinMode(RELAY_PIN, OUTPUT);

//	timeThread.onRun( getTime );
//  timeThread.setInterval(90);

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

	timer.setInterval(2000, onLamp);

	Timer1.initialize(10000);
	Timer1.attachInterrupt(timer1_action);

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
	timer.run();
				
	if (printThread.shouldRun())
		printThread.run(); // запускаем поток



}
