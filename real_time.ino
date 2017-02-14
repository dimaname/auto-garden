#include <TimerOne.h>
#include <Thread.h>
#include "Schedule.h"

Thread timeThread = Thread(); 
Thread printThread = Thread();





// создаём объект для работы с часами реального времени
RTC clock;
Schedule schedule(clock);



void printTime() {
	// выводим в serial порт текущее время, дату и день недели
	Serial.print(schedule.time);
	Serial.print(" -- ");
	Serial.print(schedule.date);
	Serial.print(" -- ");
	Serial.println(schedule.weekday);

}

void timer1_action() {		
	schedule.tact();
}



void setup()
{


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


	Timer1.initialize(10000);
	Timer1.attachInterrupt(timer1_action);

	//schedule.addTask("10:00", pumpOn);
	schedule.addTask("ПН ВТ ЧТ, 10:12:30", pumpOn);
	//schedule.addTask("ПН ВТ ЧТ, 10:12:30");
	//schedule.addTask("10:14:30");



}

void pumpOn() {
	Serial.println(" pumpOn ");
}

void loop()
{
	
				
	if (printThread.shouldRun())
		printThread.run(); // запускаем поток



}
