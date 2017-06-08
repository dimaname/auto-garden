
#include  "schedule.h"

vector<String> WEEKDAYS_SHORT_NAMES = { "MO","TU","WE","TH","FR","SA","SU" };


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
	timeplan.trim();
	vector<int> daysOfWeekVector;
	vector<int> timePartsVector;
	Schedule::TYPE type = getTimeplaneType(timeplan);
	if (type == Schedule::TYPE::EveryDay) {
		timePartsVector = getTimeFromTimeplan(timeplan);
	}
	else {
		timePartsVector = getTimeFromTimeplan(timeplan);
		daysOfWeekVector = getDaysFromTimeplan(timeplan);
	}

	items.push_back(ScheduleItem(type, daysOfWeekVector, timePartsVector[0], timePartsVector[1], timePartsVector[2], callback));
	return items.size() - 1;
};

void  Schedule::removeTask(int taskId) {
	items.erase(items.begin() + taskId);
};

void Schedule::changeTaskTime(int taskId, String timeplan) {
	ScheduleItem &task = items.at(taskId);
	timeplan.trim();
	vector<int> daysOfWeekVector;
	vector<int> timePartsVector;
	Schedule::TYPE type = getTimeplaneType(timeplan);
	if (type == Schedule::TYPE::EveryDay) {
		timePartsVector = getTimeFromTimeplan(timeplan);
	}
	else {
		timePartsVector = getTimeFromTimeplan(timeplan);
		daysOfWeekVector = getDaysFromTimeplan(timeplan);
	}

	task.type = type;
	task.weekdays = daysOfWeekVector;
	task.hour = timePartsVector[0];
	task.minute = timePartsVector[1];
	task.second = timePartsVector[2];
	task.prevFirePeriodWas = -1;
}

String Schedule::getTaskTimeplan(int taskId) {
	ScheduleItem &task = items.at(taskId);
	String timeplan = "";
	for (std::vector<int>::iterator it = task.weekdays.begin(); it != task.weekdays.end(); ++it) {
		timeplan += weekdayNumberToName(*it);
	}
	String hour = task.hour < 10 ? "0" + String(task.hour) : String(task.hour);
	String minute = task.minute < 10 ? "0" + String(task.minute) : String(task.minute);
	String second = task.second < 10 ? "0" + String(task.second) : String(task.second);
	timeplan += " " + hour + ":" + minute + ":" + second;
	return timeplan;
};


unsigned long Schedule::timeLeftFor(int taskId) {
	if (taskId > items.size() - 1 || taskId < 0) {
		return 0;
	}

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

Schedule::TYPE Schedule::getTimeplaneType(String timeplan)
{
	int comPos = timeplan.indexOf(",");
	return comPos == -1 ? Schedule::TYPE::EveryDay : Schedule::TYPE::EveryWeek;
}

vector<int> Schedule::getDaysFromTimeplan(String timeplan) {
	int comPos = timeplan.indexOf(",");
	String daysPart = timeplan.substring(0, comPos);
	daysPart.trim();
	daysPart += " ";
	int startPos = 0;
	int spacePos = daysPart.indexOf(" ", startPos);


	String day;
	vector<int> ret;
	do {
		int dayNumber = weekdayNameToNumber(daysPart.substring(startPos, spacePos));	
		if (dayNumber != -1) {
			ret.push_back(dayNumber);
		}
		startPos = spacePos + 1;
		spacePos = daysPart.indexOf(" ", startPos);

	} while (spacePos != -1);
	sort(ret.begin(), ret.end());
	return ret;
}

vector<int> Schedule::getTimeFromTimeplan(String timeplan) {
	int comPos = timeplan.indexOf(",");
	String timePart = timeplan.substring(comPos + 1);
	timePart.trim();
	vector<int> ret;
	if (timePart.length() != 0) {
		int startPos = 0;
		int delimiterPos = timePart.indexOf(":", startPos);
		while (delimiterPos != -1) {
			ret.push_back(timePart.substring(startPos, delimiterPos).toInt());
			startPos = delimiterPos + 1;
			delimiterPos = timePart.indexOf(":", startPos);
		}
		ret.push_back(timePart.substring(startPos).toInt());
	}
	ret.resize(3);
	return ret;
}




int Schedule::weekdayNameToNumber(String day) {
	day.trim();
	day.toUpperCase();
	std::vector<String>::iterator it;

	it = find(WEEKDAYS_SHORT_NAMES.begin(), WEEKDAYS_SHORT_NAMES.end(), day);
	
	if (it != WEEKDAYS_SHORT_NAMES.end()) {
		return it - WEEKDAYS_SHORT_NAMES.begin() + 1;
	}
	else {
		return -1;
	}	
}

String Schedule::weekdayNumberToName(int day) {
	return WEEKDAYS_SHORT_NAMES.at(day-1);
}