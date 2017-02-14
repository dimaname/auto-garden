
#include  "schedule.h"




	Schedule::Schedule(RTC _clock) {
		clock = _clock;	
	};


	void Schedule::tact() {
		sei();	
		clock.read();
		clock.getTimeStr(time, LEN_TIME);
		clock.getDateStr(date, LEN_DATE);	
		clock.getDOWStr(weekday, LEN_DOW);
		cli();

	};

	void Schedule::addTask( String timestamp, void callback()) {	
		
		
		Schedule::TYPE type = Schedule::TYPE::EveryDay;
		callback();
		switch (type) {
		case Schedule::TYPE::EveryDay:
			Serial.print(" EveryDay ... ");
			items.push_back(ScheduleItem());
			break;
		case Schedule::TYPE::EveryWeek:
			Serial.print(" EveryWeek ... ");
			items.push_back(ScheduleItem());
			break;
		default:
			Serial.print(" takoe ... ");
		};
	};
