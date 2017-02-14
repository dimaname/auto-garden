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
#include <EEPROM.h>
#include <TroykaRTC.h>
#include <StandardCplusplus.h>
#include <vector>
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

using namespace std;
class Schedule {
private:
	class ScheduleItem {
	public:
		int type;
		ScheduleItem() {
			type = Schedule::TYPE::EveryDay;
		}

	};
	vector<ScheduleItem> items;

public:
	RTC clock;

	Schedule(RTC);
	enum TYPE { EveryDay, EveryWeek };
	char time[LEN_TIME] = { 0 };
	char date[LEN_DATE] = { 0 };
	char weekday[LEN_DOW] = { 0 };

	void tact();
	void addTask(String, void());


};

#endif


