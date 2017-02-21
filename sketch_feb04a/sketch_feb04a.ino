
#include <ThreadController.h>
#include <Thread.h>
#include <StaticThreadController.h>


#include <EEPROM.h>
#include <Wire.h>
#include <TroykaRTC.h>

// размер массива для времени с учётом завершающего нуля
#define LEN_TIME 12
// размер массива для даты с учётом завершающего нуля
#define LEN_DATE 12
// размер массива для дня недели с учётом завершающего нуля
#define LEN_DOW 12

 
RTC clock;фывфыв

#include <TimerOne.h>
#include <TimerThree.h>


#include <DallasTemperature.h>
#include <OneWire.h>


#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// тут будет хранится адрес нашего датчика. Каждый DS18B20 имеет свой уникальный адрес, зашитый на заводе

#include <Bounce2.h>

#define RELAY_PIN 25

#define RELAY_PIN2 23
Bounce bouncer = Bounce();



#include <Encoder.h>
#define PIN_ENCODER_CLK 7
#define PIN_ENCODER_DT 6
#define PIN_ENCODER_SW 28

Encoder myEnc(PIN_ENCODER_DT, PIN_ENCODER_CLK);

#include <LiquidCrystal.h>
LiquidCrystal lcd(4, 5, 10, 11, 12, 13);

DeviceAddress insideThermometer = { 0x28, 0xA0, 0xCF, 0x0, 0x0, 0x0, 0x80, 0x83 };



void printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}
// Эта волшебная функция пишет температуру в Serial
void printTemperature(DeviceAddress deviceAddress)
{

	float tempC = sensors.getTempC(deviceAddress);
	//float tempC = sensors.getTempCByIndex(0);
	String msg = "Temperature is: " + String(tempC);
	Serial.println(msg);

}

void Timer1_action()
{
	sensors.requestTemperatures(); // Отправка команды на получение температуры
	printTemperature(insideThermometer);
}


void ClockTimer_action()
{
	// массив для хранения текущего времени
	char time[LEN_TIME] = { 0 };
	// массив для хранения текущей даты
	char date[LEN_DATE] = { 0 };
	// массив для хранения текущего дня недели
	char dow[LEN_DOW] = { 0 };

	// запрашиваем данные с часов
	clock.read();

	// сохраняем текущее время в массив time
	clock.getTimeStr(time, LEN_TIME);
	// сохраняем текущую дату в массив date
	clock.getDateStr(date, LEN_DATE);
	// сохраняем текущий день недели в массив dow
	clock.getDOWStr(dow, LEN_DOW);

	// выводим в serial порт текущее время, дату и день недели
	Serial.print(time);
	Serial.print("--");
	Serial.print(date);
	Serial.print("--");
	Serial.println(dow);

}






void setup() {

	clock.begin();
	// метод установки времени и даты в модуль вручную
	// clock.set(10,25,45,27,07,2005,THURSDAY);
	// метод установки времени и даты автоматически при компиляции
	clock.set(__TIMESTAMP__);


	Timer3.initialize(2000000);
	Timer3.attachInterrupt(Timer1_action);

	Timer1.initialize(1000000);
	Timer1.attachInterrupt(ClockTimer_action);

	sensors.begin();

	// определяемся с режимом питания датчиков. Они кстати могут питаться по тому же проводу, по которому отправляют данные
	Serial.print("Parasite power is: ");
	if (sensors.isParasitePowerMode()) Serial.println("ON");
	else Serial.println("OFF");

	// тут можно прописать адрес вручную
	// Есть 2 варианта:
	// 1) oneWire.search(deviceAddress) – сразу если известен адрес датчика
	// 2) sensors.getAddress(deviceAddress, index) – по номеру подключения, начиная с 0


	// Мы будем использовать 2 вариант – поищем первый датчик (номер=0):
	//if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Не найден адрес датчика 0");

	sensors.setResolution(insideThermometer, 10);

	Serial.print("getResolution ");
	Serial.print(sensors.getResolution(insideThermometer), DEC);
	Serial.println();



	lcd.begin(16, 2);
	lcd.command(0b101010);
	// печатаем первую строку
	lcd.print("     \xcf\xd0\xc8\xc2\xc5\xd2\x2c");
	lcd.setCursor(0, 1);
	lcd.print(" \xdf \xcf\xce\xcc\xce\xc3\xd3 \xd2\xc5\xc1\xc5");
	// Конфигурируем нужный пин на выход
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(RELAY_PIN2, OUTPUT);
	pinMode(2, INPUT); // кнопка на пине 2
	digitalWrite(2, HIGH); // подключаем встроенный подтягивающий резистор
	bouncer.attach(2); // устанавливаем кнопку
	bouncer.interval(100); // устанавливаем параметр stable interval = 5 мс
	Serial.begin(9600); //установка Serial-порта на скорость 9600 бит/сек

}
long oldPosition = -999;


void loop() {

	long newPosition = myEnc.read();
	if (newPosition != oldPosition) {
		oldPosition = newPosition;
		Serial.println(newPosition);
	}


	// Включаем реле
	//

	//

	if (bouncer.update())
	{ //если произошло событие
		if (bouncer.read() == 1)
		{ //если кнопка нажата
			digitalWrite(RELAY_PIN2, LOW);
			delay(50);
			digitalWrite(RELAY_PIN, HIGH);
			Serial.println("pressed"); //вывод сообщения о нажатии
		}
		else {
			Serial.println("released"); //вывод сообщения об отпускании
			digitalWrite(RELAY_PIN, LOW);
			delay(50);
			digitalWrite(RELAY_PIN2, HIGH);

		}
	}




	// Далее всё повторяется
}
