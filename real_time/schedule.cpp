
#include  "schedule.h"




Schedule::Schedule(RTC _clock) {
	clock = _clock;
};

int Schedule::weekday = 0;
int Schedule::hour = 0;
int Schedule::minute = 0;
int Schedule::second = 0;
char Schedule::timeStr[LEN_TIME] = { 0 };
void Schedule::tact() {
	sei();
	clock.read();
	Schedule::weekday = clock.getDOW();
	Schedule::hour = clock.getHour();
	Schedule::minute = clock.getMinute();
	Schedule::second = clock.getSecond();
	clock.getTimeStr(timeStr, LEN_TIME);
	checkTasks();
	cli();
};

void Schedule::checkTasks() {

	for (std::vector<ScheduleItem>::iterator it = items.begin(); it != items.end(); ++it)
	{
		ScheduleItem & tmp = *it;
		if (tmp.canFire()) {
			tmp.fire();
		}
	}
};

int Schedule::addTask(String timeplan, void callback()) {


	Schedule::TYPE type;

	timeplan.trim();
	int comPos = timeplan.indexOf(",");
	String weekDayPart, timePart;
	vector<int> daysOfWeekVector;

	if (comPos == -1) {
		type = Schedule::TYPE::EveryDay;
		timePart = timeplan;
	}
	else {
		type = Schedule::TYPE::EveryWeek;
		weekDayPart = timeplan.substring(0, comPos);
		timePart = timeplan.substring(comPos + 1);
		weekDayPart.trim();
		daysOfWeekVector = daysToArray(weekDayPart);
		timePart.trim();
	}

	vector<int> time = parseTime(timePart);

	items.push_back(ScheduleItem(type, daysOfWeekVector, time[0], time[1], time[2], callback));
	return items.size() - 1;
};



unsigned long Schedule::timeLeftFor(int taskId) {

	ScheduleItem task = items.at(taskId);
	int minDistanceDays = 7;

	unsigned long  secondBeforeTask = task.hour * 3600L + task.minute * 60L + task.second;
	unsigned long  secondBeforeNow = Schedule::hour * 3600L + Schedule::minute * 60L + Schedule::second;
	bool isFireToday = secondBeforeNow >= secondBeforeTask;
	switch (task.type)
	{
	case Schedule::EveryWeek:
		for (std::vector<int>::iterator it = task.weekdays.begin(); it != task.weekdays.end(); ++it) {
			int dist = *it - weekday;
			dist = dist < 0 ? dist + 7 : dist;
			if (dist == 0 && isFireToday) {
				dist = minDistanceDays;
			}
			if (dist < minDistanceDays) {
				minDistanceDays = dist;
			}
		}
		break;
	case Schedule::EveryDay: 
		minDistanceDays = isFireToday ? 1 : 0;
		break;
	}
	unsigned long distance_in_second = 0;

	if (minDistanceDays == 0) {
		distance_in_second = secondBeforeTask - secondBeforeNow;
	}
	else {
		distance_in_second = SECONDS_IN_DAY*(minDistanceDays - 1) + SECONDS_IN_DAY - secondBeforeNow + secondBeforeTask;

	}	
	return distance_in_second;

}

vector<int> Schedule::daysToArray(String str) {
	int startPos = 0;
	int spacePos = str.indexOf(" ", startPos);
	String day;
	vector<int> ret;
	while (spacePos != -1) {
		ret.push_back(nameOfDayToNumber(str.substring(startPos, spacePos)));
		startPos = spacePos + 1;
		spacePos = str.indexOf(" ", startPos);
	}
	ret.push_back(nameOfDayToNumber(str.substring(startPos)));
	sort(ret.begin(), ret.end());
	return ret;
}

vector<int> Schedule::parseTime(String str) {
	vector<int> ret;
	if (str.length() != 0) {
		int startPos = 0;
		int delimiterPos = str.indexOf(":", startPos);
		while (delimiterPos != -1) {
			ret.push_back(str.substring(startPos, delimiterPos).toInt());
			startPos = delimiterPos + 1;
			delimiterPos = str.indexOf(":", startPos);
		}
		ret.push_back(str.substring(startPos).toInt());
	}
	ret.resize(3);
	return ret;
}




int  Schedule::nameOfDayToNumber(String day) {
	day.toUpperCase();
	if (day == "MO") {
		return 1;
	}
	else if (day == "TU") {
		return 2;
	}
	else if (day == "WE") {
		return 3;
	}
	else if (day == "TH") {
		return 4;
	}
	else if (day == "FR") {
		return 5;
	}
	else if (day == "SA") {
		return 6;
	}
	else if (day == "SU") {
		return 7;
	}
	return -1;
}