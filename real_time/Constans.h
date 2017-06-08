#ifndef CONSTANS_H
#define CONSTANS_H

#define RELAY1_PIN 34
#define RELAY2_PIN 32
#define RELAY3_PIN 30
#define RELAY4_PIN 28

#define WATER_FLOW_PIN 18
#define WATER_LEVEL_PIN A9
#define WATER_LEVEL_2_PIN A10
#define BEEP_PIN 43
#define DH11_PIN A8
#define EXTERNAL_THERMO 17
#define LIGHT_SENSOR 36

#define LCD_LIGHT_RED 35
#define LCD_RS_ORANGE 33
#define LCD_E_YELLOW 31
#define LCD_D4_GREEN 29
#define LCD_D5_BLUE 27
#define LCD_D6_PUPRPLE 25
#define LCD_D7_GRAY 23


#define DEBOUNCE 5 
#define BUTTON_PUMP 38
#define BUTTON_LIGHT 42
#define BUTTON_3 46
#define BUTTON_4 50

enum PUMP_STATES { WAITING, WORKING };
PUMP_STATES pump_state = PUMP_STATES::WAITING;

RTC clock;
Schedule schedule(clock);
int taskWateringId = -1;
bool isDisabledGSM = false;
void pumpOn(bool isNeedSms = false);
void pumpOff(bool isNeedSms = false);
void pumpOnWithSms();
void pumpOnWithoutSms();
void pumpOffWithSms();
void pumpOffWithoutSms();
void showLcdMessage(int showTimeout, int lightTimeout, LcdContent::MODES mode, char *msg0 = "", char *msg1 = "");
void sendMessage(char* message, bool isNeedSms = false, bool isSendToEachHost = false);

#endif 