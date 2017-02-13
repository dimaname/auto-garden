// schedule.h
#ifndef _SCHEDULE_h
#define _SCHEDULE_h

// размер массива для времени с учётом завершающего нуля
#define LEN_TIME 12
// размер массива для даты с учётом завершающего нуля
#define LEN_DATE 12
// размер массива для дня недели с учётом завершающего нуля
#define LEN_DOW 12
#include <Wire.h>
// библиотека для работы с часами реального времени
#include "TroykaRTC.h"

// EEPROM — энергонезависимая память
// библиотека для записи и считывания информации с EEPROM
#include <EEPROM.h>


#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif





class Schedule {
private:
	class ScheduleItem {
	public:
		int type;
		ScheduleItem() {
			type = Schedule::TYPE::EveryDay;
		}

	};

public:
	Schedule(RTC);
	enum TYPE { EveryDay, EveryWeek };
	char time[LEN_TIME] = { 0 };	
	char date[LEN_DATE] = { 0 };	
	char weekday[LEN_DOW] = { 0 };

	void tact();



}; 

#endif


