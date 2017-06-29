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
		sendMessage("\r\nHELP\r\nPUMP ON\r\nPUMP OFF\r\nSTART AT MO TU WE TH FR SA SU, HH:MM:SS\r\nSTOP PLAN", true);
	}
	else if (smsText.indexOf("PUMP ON") != -1 || smsText.indexOf("PUMPON") != -1)
	{
		Serial.println("SMS command: PUMP ON");
		pumpOnWithSms();
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
		int timeplanBeginIndex = smsText.indexOf("START AT");
		timeplanBeginIndex = timeplanBeginIndex == -1 ? smsText.indexOf("STARTAT") + 8 : timeplanBeginIndex + 9;
		String timeplan = smsText.substring(timeplanBeginIndex);
		timeplan.trim();
		if (taskWateringId == -1) {
			taskWateringId = schedule.addTask(timeplan, pumpOnWithSms);
			lcdContent.Mode = LcdContent::NORMAL;
		}
		else {
			schedule.changeTaskTime(taskWateringId, timeplan);
		}
		String message = "Ok. Watering timeplan is [" + schedule.getTaskTimeplan(taskWateringId) + "]";
		sendMessage((char*)message.c_str(), true);
	}
	else if (smsText.indexOf("STOP PLAN") != -1 || smsText.indexOf("STOPPLAN") != -1)
	{
		Serial.println("SMS command: STOP PLAN");
		schedule.removeTask(taskWateringId);
		taskWateringId = -1;
		lcdContent.Mode = lcdContent.Mode == LcdContent::NORMAL ? LcdContent::STOP : lcdContent.Mode;
		sendMessage("Ok. Watering timeplan cleared.", true);
	}
	else if (smsText.indexOf("INFO") != -1)
	{
		Serial.println("SMS command: INFO");

		sendMessage("INFO.", true);
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



int rrr = 0;
void checkIncomingSMS() {
	Serial.println("checkIncomingSMS: " + String(rrr));
	rrr++;

	int numData = GSM.IsSMSPresent(SMS_UNREAD);

	if (numData) {

		if (GSM.GetSMS(numData, phoneBuffer, smsBuffer, 160))
		{
			String smsText = String(smsBuffer);
			String phoneNumber = String(phoneBuffer);

			Serial.println("numberSms: " + String(phoneBuffer));
			Serial.println("textSms: " + String(smsBuffer));

			int numberIndex = numberIndexInTrustedList(phoneNumber);
			if (numberIndex != -1) {
				lastHostNumberIndex = numberIndex;
				tone(BEEP_PIN, 2500, 500);
				processSmsCommand(smsText);
			}
			else {
				Serial.println("Not trusted number: " + phoneNumber);
			}
			GSM.DeleteSMS(numData);
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
	}
	return result == 1;
}




#endif

