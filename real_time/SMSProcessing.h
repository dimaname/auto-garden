#ifndef _SMSPPROCESSING_h
#define _SMSPPROCESSING_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h" 
#else
#include "WProgram.h"
#endif
#include "Constans.h"

void processSmsCommand(String smsText) {
	smsText.toUpperCase();

	if (smsText.indexOf("HELP") != -1)
	{
		Serial.println("SMS command: HELP");
		sendMessage("\r\nHELP\r\nPUMP ON S=[1,2]\r\nPUMP OFF\r\nSTART AT [MO TU WE TH FR SA SU], [HH:MM:SS], S=[1,2]\r\nSTOP PLAN\r\nINFO", true);
	}
	else if (smsText.indexOf("PUMP ON") != -1 || smsText.indexOf("PUMPON") != -1)
	{
		Serial.println("SMS command: PUMP ON");
		int zonePartIndex = smsText.indexOf("S=");
		if (zonePartIndex == -1) {
			String message = "Fail. Not found zone parametr S. It must be S=1 or S=2. Use HELP sms for syntax tips.";
			sendMessage((char*)message.c_str(), true);
			return;
		}
		int zoneNumber = smsText.substring(zonePartIndex + 2, zonePartIndex + 3).toInt();

		if (zoneNumber == 1) 
			wateringZone1();
	
		if (zoneNumber == 2)
			wateringZone2();
		

	}
	else if (smsText.indexOf("PUMP OFF") != -1 || smsText.indexOf("PUMPOFF") != -1)
	{
		Serial.println("SMS command: PUMP OFF");
		pumpOffWithSms();
	}
	else if (smsText.indexOf("LIGHT ON") != -1 || smsText.indexOf("LIGHTON") != -1)
	{
		Serial.println("SMS command: LIGHT ON");
		Serial.println("LIGHT Off!");
	}
	else if (smsText.indexOf("START AT") != -1 || smsText.indexOf("STARTAT") != -1)
	{
		Serial.println("SMS command: START AT");
		String message;
		int timeplanBeginIndex = smsText.indexOf("START AT");
		timeplanBeginIndex = timeplanBeginIndex == -1 ? smsText.indexOf("STARTAT") + 8 : timeplanBeginIndex + 9;
		String timeplan = smsText.substring(timeplanBeginIndex);
		timeplan.trim();
		int zonePartIndex = timeplan.indexOf("S=");
		if (zonePartIndex == -1) {
			message = "Fail. Not found zone parametr S. It must be S=1 or S=2. Use HELP sms for syntax tips.";
			sendMessage((char*)message.c_str(), true);
			return;
		}
		int zoneNumber = timeplan.substring(zonePartIndex + 2, zonePartIndex + 3).toInt();
		timeplan = timeplan.substring(0, zonePartIndex);
		timeplan.trim();
		if (timeplan.endsWith(",")) {
			timeplan.remove(timeplan.length() - 1);
		}

		if (zoneNumber == 1) {
			if (taskWateringZone1Id == -1) {
				taskWateringZone1Id = schedule.addTask(timeplan, wateringZone1);
				//первый параметр это номер зоны полива
				saveTimeplanToEEPROM(0, taskWateringZone1Id);
				lcdContent.Mode = LcdContent::NORMAL;
			}
			else {
				schedule.changeTaskTime(taskWateringZone1Id, timeplan);
				saveTimeplanToEEPROM(0, taskWateringZone1Id);
			}
			message = "Ok. Watering timeplan for zone S=1 is [" + schedule.getTaskTimeplan(taskWateringZone1Id) + "]";
		}

		if (zoneNumber == 2) {
			if (taskWateringZone2Id == -1) {
				taskWateringZone2Id = schedule.addTask(timeplan, wateringZone2);
				//первый параметр это номер зоны полива
				saveTimeplanToEEPROM(1, taskWateringZone2Id);
				lcdContent.Mode = LcdContent::NORMAL;
			}
			else {
				schedule.changeTaskTime(taskWateringZone2Id, timeplan);
				saveTimeplanToEEPROM(1, taskWateringZone2Id);
			}
			message = "Ok. Watering timeplan for zone S=2 is [" + schedule.getTaskTimeplan(taskWateringZone2Id) + "]";
		}
		sendMessage((char*)message.c_str(), true);
	}
	else if (smsText.indexOf("STOP PLAN") != -1 || smsText.indexOf("STOPPLAN") != -1)
	{
		Serial.println("SMS command: STOP PLAN");
		schedule.removeTask(taskWateringZone1Id);
		schedule.removeTask(taskWateringZone2Id);
		clearTimeplanInEEPROM(0);
		clearTimeplanInEEPROM(1);
		taskWateringZone1Id = -1;
		taskWateringZone2Id = -1;
		sendMessage("Ok. Watering timeplan cleared.", true);
	}
	else if (smsText.indexOf("INFO") != -1)
	{
		Serial.println("SMS command: INFO");
		String message = "Temperature: " + String(lastDH11_Temperature) + "C\r\n";
		message += "Humidity: " + String(lastDH11_Humidity) + "%\r\n";
		message += "Water: " + String(waterLevel_1 == HIGH ? "ok" : "no water") + "\r\n";
		message += "Pump: " + String(pump_state == PUMP_STATES::WAITING ? "stop" : "work") + "\r\n";
		message += "Watering timeplan zone 1: " + String(taskWateringZone1Id != -1 ? "[" + String(schedule.getTaskTimeplan(taskWateringZone1Id)) + "]" : "no plan") + "\r\n";
		message += "Watering timeplan zone 2: " + String(taskWateringZone2Id != -1 ? "[" + String(schedule.getTaskTimeplan(taskWateringZone2Id)) + "]" : "no plan") + "\r\n";
		message += "Watering time: " + String(watering_internal) + " sec\r\n";
		message += "Liters last watering: " + String(waterLitersForLastWatering) + "\r\n";		
		message += "Sim balance: " + String(isBalanceData == true ? String(currentBalance) + " rub" : "no data") + "\r\n";
	
		sendMessage((char*)message.c_str(), true);
	}

};


int numberIndexInTrustedList(String number) {
	unsigned length = sizeof(TRUSTED_NUMBERS) / sizeof(TRUSTED_NUMBERS[0]);
	int result = -1;
	for (int i = 0; i < length; i++)
	{
		if (number.endsWith(TRUSTED_NUMBERS[i])) {
			result = i;
			break;
		}
	}
	return result;
};

double parseBalance(String ussdRespose) {
	int startIndex = ussdRespose.indexOf("balans");
	int endIndex = ussdRespose.indexOf("r.");
	if (startIndex == -1 || endIndex == -1)
		return;
	currentBalance = ussdRespose.substring(startIndex + 6, endIndex).toDouble();
	isBalanceData = true;
}


void checkIncomingSMS() {
	int type = GSM.checkGSM();
	if (type == 1) {
		parseBalance(String(GSM.LastUSSDResponse));
		Serial.println("\nLast USSD response: " + String(GSM.LastUSSDResponse));
		Serial.println("\nbalance is: " + String(currentBalance));
	}
	else if (type == 2) {
		String phoneNumber = "", smsText = "";
		String fullSMS = String(GSM.LastSMS);

		int startIndexPhone = fullSMS.indexOf("\"+") + 2;
		int lastIndexPhone = fullSMS.indexOf("\",", startIndexPhone);

		phoneNumber = fullSMS.substring(startIndexPhone, lastIndexPhone);

		int startIndexText = fullSMS.indexOf("\n", lastIndexPhone) + 1;
		smsText = fullSMS.substring(startIndexText);

		Serial.println("phoneNumber: " + phoneNumber);
		Serial.println("smsText: " + smsText);

		int numberIndex = numberIndexInTrustedList(phoneNumber);
		if (numberIndex != -1) {
			lastHostNumberIndex = numberIndex;
			tone(BEEP_PIN, 2500, 500);
			processSmsCommand(smsText);
		}
		else {
			Serial.println("Not trusted number: " + phoneNumber);
		}

	}
}




char* addTimePrefix(char* str) {
	char* _timeStr = schedule.timeStr;
	int bufferSize = strlen(_timeStr) + strlen(str) + 1 + 2;
	char* concatString = new char[bufferSize];

	strcpy(concatString, _timeStr);
	strcat(concatString, "\r\n");
	strcat(concatString, str);
	return concatString;
}

bool sendSms(char* smsText, char* phoneNumber) {
	char* fullMessage = addTimePrefix(smsText);
	int result = GSM.SendSMS(phoneNumber, fullMessage);
	if (result == 1) {
		Serial.println("\tsms was sent");
		currentBalance -= SMS_COST;
	}
	return result == 1;
}




#endif

