
#include  "schedule.h"




	Schedule::Schedule(RTC _clock) {
		clock = _clock;	
	};

	int Schedule::weekday = 0;

	void Schedule::tact() {
		sei();	
		clock.read();
		clock.getTimeStr(time, LEN_TIME);			
		clock.getDateStr(date, LEN_DATE);	
		Schedule::weekday = clock.getDOW();
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

	void Schedule::addTask( String timeplan, void callback()) {	
		
		
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
			timePart = timeplan.substring(comPos+1);
			weekDayPart.trim();
			daysOfWeekVector = daysToArray(weekDayPart);		
			timePart.trim();
		}
		
		//Serial.println("weekDayPart " + weekDayPart);
		//Serial.println("timePart " + timePart);
	 	items.push_back(ScheduleItem(type, daysOfWeekVector, callback));
	};

	vector<int> Schedule::daysToArray(String str) {
		int startPos = 0;
		int spacePos = str.indexOf(" ", startPos);
		String day;
		vector<int> ret;
		while (spacePos != -1) {		
		
			ret.push_back(nameOfDayToNumber(str.substring(startPos, spacePos)));
			startPos = spacePos+1;
			spacePos = str.indexOf(" ", startPos);
		}		
		ret.push_back(nameOfDayToNumber(str.substring(startPos)));
		return ret;
	}

	int  Schedule::nameOfDayToNumber(String day) {
		day.toUpperCase();
		if (day == "MO") {
			return 1;				
		}else if(day == "TU") {
			return 2;
		}else if (day == "WE") {
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