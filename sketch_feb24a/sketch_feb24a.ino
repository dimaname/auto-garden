
#include <Thread.h>


Thread waterThread = Thread();
#define WATER_PIN 18


volatile int NbTopsFan;
void rpm() {
  cli();
  NbTopsFan++;
  // Serial.println(String(NbTopsFan));
  sei();
 
}
double totalWater = 0;
unsigned long prevCallTime = 0;

void calcWater() {

  
    cli();
     Serial.println(String(NbTopsFan));
   // Serial.println("litersPerSec: " + String(litersPerSec) + "      totalWater: " + String(totalWater));
    NbTopsFan = 0;
   // totalWater += litersPerSec;
  //  prevCallTime = millis();

    sei();

    
 
}



void setup()
{
Serial.begin(9600);
 


  ////// датчик потока
  pinMode(WATER_PIN, INPUT);
  attachInterrupt(5, rpm, RISING);
  waterThread.onRun( calcWater );
  waterThread.setInterval(500);
  ///////


}


void loop()
{



  if (waterThread.shouldRun())
    waterThread.run(); // запускаем поток

 

}

