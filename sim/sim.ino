#define SIM800_TX_PIN 10
#define SIM800_RX_PIN 11

#include <SoftwareSerial.h>
#include <Sim800l.h>
#include <SoftwareSerial.h> //is necesary for the library!! 

Sim800l GSM;

bool error; //to catch the response of sendSms

int numdata;
boolean started = false;
char smsbuffer[160];
char n[20];
char mes1[] = "On\r";    //сообщения, которые ждем
char mes2[] = "Off\r";


String textSms, numberSms;
uint8_t index1;

const String TRUSTED_NUMBERS[] = {"79617638670", "79068577144"};

void setup() {
	Serial.begin(9600);
	Serial.println("setup");
	GSM.begin(); // initializate the library. 
	char* textSms = "";  //text for the message. 
	char* numberSms = "79617638670"; //change to a valid number.
//	error = GSM.sendSms(numberSms, textSms);







	Serial.println(hexToAscii("043D0430044704300442044C0020043F043E043B04380432002000700075006D00700020006F006E"));
	//GSM.reset();
	error = GSM.delAllSms(); //clean memory of sms;


}


String hexToAscii(String hex)
{
	uint16_t len = hex.length();
	String ascii = "";

	for (uint16_t i = 0; i < len; i += 2)
		ascii += (char)strtol(hex.substring(i, i + 2).c_str(), NULL, 16);

	return ascii;
}

void loop() {

	Serial.println("loop");
	textSms = GSM.readSms(1); //read the first sms


	if (textSms && textSms.length() ) {
		numberSms = GSM.getNumberSms(1); 	
		Serial.println("numberSms: " + numberSms);
		Serial.println("textSms: " + textSms);

		if (validateNumber(numberSms)) {
			processSmsCommand(textSms);			
		}
		else {
			Serial.println("Not trusted number: " + numberSms);
		}
		GSM.delAllSms();	
	}
	
	delay(500);
};



void processSmsCommand(String smsText) {
	smsText.toUpperCase();
	if (smsText.indexOf("PUMP ON") != -1 || smsText.indexOf("PUMPON") != -1) //first we need to know if the messege is correct. NOT an ERROR
	{
		Serial.println("PUMP On!");

	}
	else if (smsText.indexOf("PUMP OFF") != -1 || smsText.indexOf("PUMPOFF") != -1)
	{
		Serial.println("PUMP Off!");
	}
	else if (smsText.indexOf("LIGHT ON") != -1 || smsText.indexOf("LIGHTON") != -1)
	{
		Serial.println("LIGHT Off!");
	}
	else if (smsText.indexOf("LIGHT OFF") != -1 || smsText.indexOf("LIGHTOFF") != -1)
	{
		Serial.println("LIGHT Off!");
	}
};


bool validateNumber(String number) {
	unsigned length = sizeof(TRUSTED_NUMBERS) / sizeof(TRUSTED_NUMBERS[0]);
	bool result = false;
	for (int i = 0; i < length; i++) 
	{
		if (number.endsWith(TRUSTED_NUMBERS[i])) {
			result = true;		
			break;
		}
	}
	return result;
};