// schedule.h
#ifndef _SCHEDULE_h
#define _SCHEDULE_h

// размер массива для времени с учётом завершающего нуля
#define LEN_TIME 12
#define SECONDS_IN_DAY 86400L


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
		void(*callback)();

	public:
		vector<int> weekdays;
		int hour;
		int minute;
		int second;
		int prevFirePeriodWas;
			
		Schedule::TYPE type;

		ScheduleItem(Schedule::TYPE _type, vector<int> _weekdays, int _hour, int _minute, int _second, void(*_callback)()) {
			type = _type;
			callback = _callback;
			weekdays = _weekdays;
			hour = _hour;
			minute = _minute;
			second = _second;
			prevFirePeriodWas = -1;
		}

		void fire() {
			this->prevFirePeriodWas = Schedule::weekday;
			this->callback();
		}

		bool canFire() {
			bool isCanFire = false;

			switch (type)
			{
			case Schedule::EveryDay:
				if (this->hour == Schedule::hour && this->minute == Schedule::minute && this->second == Schedule::second) {
					isCanFire = prevFirePeriodWas != Schedule::weekday;			
				}			
				break;
			case Schedule::EveryWeek:
				if (this->hour == Schedule::hour && this->minute == Schedule::minute && this->second == Schedule::second) {
					if (find(this->weekdays.begin(), this->weekdays.end(), Schedule::weekday) != this->weekdays.end()) {
						isCanFire = prevFirePeriodWas != Schedule::weekday;					
					}
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
	vector<int>  parseTime(String);
	int nameOfDayToNumber(String);

	static int weekday;
	static int hour;
	static int minute;
	static int second;
	static char timeStr[LEN_TIME];
	
	void tact();
	int addTask(String, void());
	unsigned long timeLeftFor(int);
	void Schedule::checkTasks();
	vector<ScheduleItem> items;
};


#endif




