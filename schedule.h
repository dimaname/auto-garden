// schedule.h
#ifndef _SCHEDULE_h
#define _SCHEDULE_h

// размер массива для времени с учётом завершающего нуля
#define LEN_TIME 12
// размер массива для даты с учётом завершающего нуля
#define LEN_DATE 12

#include <Wire.h>
#include <EEPROM.h>
#include <TroykaRTC.h>
#include <StandardCplusplus.h>
#include <vector>
#include <algorithm>
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

using namespace std;
class Schedule {
public:
	enum TYPE { EveryDay, EveryWeek };
private:


	class ScheduleItem {
	private:
		int prevFirePeriodWas;
		void(*callback)();

	public:
		vector<int> weekdays;
		Schedule::TYPE type;

		ScheduleItem(Schedule::TYPE _type, vector<int> _weekdays, void(*_callback)()) {
			type = _type;
			callback = _callback;
			weekdays = _weekdays;
			prevFirePeriodWas = -1;
		}

		void fire() {
			this->callback();
		}

		bool canFire() {
			bool isCanFire = false;

			switch (type)
			{
			case Schedule::EveryDay:
				isCanFire = prevFirePeriodWas != Schedule::weekday;
				this->prevFirePeriodWas = Schedule::weekday;
				break;
			case Schedule::EveryWeek:
				if (find(this->weekdays.begin(), this->weekdays.end(), Schedule::weekday) != this->weekdays.end()) {
					isCanFire = prevFirePeriodWas != Schedule::weekday;
					this->prevFirePeriodWas = Schedule::weekday;
				}
				break;
			default:
				break;
			}

			return isCanFire;
		}

	};
public:
	RTC clock;
	Schedule(RTC);
	vector<int> daysToArray(String);
	int nameOfDayToNumber(String);
	char time[LEN_TIME] = { 0 };
	char date[LEN_DATE] = { 0 };
	static int weekday;

	void tact();
	void addTask(String, void());
	void Schedule::checkTasks();
	vector<ScheduleItem> items;
};


#endif




