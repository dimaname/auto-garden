#ifndef CONSTANS_H
#define CONSTANS_H
#include <LiquidCrystal.h>
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
bool isValveOpenZone1 = false;
bool isValveOpenZone2 = false;

RTC clock;
Schedule schedule(clock);
int taskWateringZone1Id = -1;
int taskWateringZone2Id = -1;
bool isDisabledGSM = false;

double SMS_COST = 1.5;
double currentBalance = 999999;
bool isBalanceData = false;
int counter5second = 0;
char* TRUSTED_NUMBERS[] = { "79617638670", "79068577144" };
int lastHostNumberIndex = 0;
SMSGSM GSM;

unsigned long watering_internal = 300;   // время полива, секунд
String watering_animate[4] = { "\x97", "\x96", "\x95", "\x94" };

void pumpOn(bool isNeedSms = false);
void pumpOff(bool isNeedSms = false);
void pumpOnWithSms();
void pumpOnWithoutSms();
void pumpOffWithSms();
void pumpOffWithoutSms();
void EEEPROMRecovery();
void saveTimeplanToEEPROM(int positionOrderInMemory, int taskId);
void clearTimeplanInEEPROM(int positionOrderInMemory);

void showLcdMessage(int showTimeout, int lightTimeout, LcdContent::MODES mode, char *msg0 = "", char *msg1 = "");
void sendMessage(char* message, bool isNeedSms = false, bool isSendToEachHost = false);

LiquidCrystal LCD16x2(LCD_RS_ORANGE, LCD_E_YELLOW, LCD_D4_GREEN, LCD_D5_BLUE, LCD_D6_PUPRPLE, LCD_D7_GRAY);
LcdContent lcdContent = LcdContent();
DHT dh11(DH11_PIN, DHT11);
volatile int lastDH11_Temperature = 0;
volatile int lastDH11_Humidity = 0;
volatile int lastLightSensorState = 1;
volatile int waterLevel_1 = 0;



#endif 