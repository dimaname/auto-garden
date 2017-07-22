#include <LiquidCrystal.h>


#include <Thread.h>


Thread waterThread = Thread();
#define WATER_PIN 18
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
LiquidCrystal LCD16x2(LCD_RS_ORANGE, LCD_E_YELLOW, LCD_D4_GREEN, LCD_D5_BLUE, LCD_D6_PUPRPLE, LCD_D7_GRAY);


byte buttons[] = { BUTTON_PUMP, BUTTON_LIGHT, BUTTON_3, BUTTON_4 };
#define NUMBUTTONS sizeof(buttons)
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];



void checkButtons()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return;
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       //when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    currentstate[index] = digitalRead(buttons[index]);   //read the button

    if (currentstate[index] != previousstate[index]) {
      if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        justreleased[index] = 1; // just released
      }
      pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index]; //keep a running tally of the buttons
  }
}

byte getPressedButton() {
  byte thisSwitch = 255;
  checkButtons();  //check the switches &amp; get the current state
  for (byte i = 0; i < NUMBUTTONS; i++) {
    current_keystate[i] = justreleased[i];
    if (current_keystate[i] != previous_keystate[i]) {
      if (current_keystate[i]) {
        thisSwitch = i;
      }
    }
    previous_keystate[i] = current_keystate[i];
  }
  return thisSwitch;
}

volatile int NbTopsFan;
void rpm() {
  cli();
  NbTopsFan++;
  // Serial.println(String(NbTopsFan));
  sei();

}
double totalWater = 0;
unsigned long prevCallTime = 0;
const float waterFlowK = 52;
int totalImp = 0;
void calcWater() {

  cli();
  double litersPerSec = NbTopsFan / waterFlowK * (1000.0 / (millis() - prevCallTime));
  totalWater += litersPerSec;
  totalImp += NbTopsFan;
  Serial.println("litersPerSec: " + String(litersPerSec) + "      totalWater: " + String(totalWater));
  LCD16x2.setCursor(0, 0);
  LCD16x2.print(String(litersPerSec) + "L/s " + String(totalWater)+"L    ");
  LCD16x2.setCursor(0, 1);
  LCD16x2.print(String(totalImp) + " imp             ");
  NbTopsFan = 0;

  prevCallTime = millis();
  sei();


}



void setup()
{
  Serial.begin(9600);

  LCD16x2.begin(16, 2);
  LCD16x2.command(0b101010);

  ////// датчик потока
  pinMode(WATER_PIN, INPUT);
  attachInterrupt(5, rpm, RISING);
  waterThread.onRun( calcWater );
  waterThread.setInterval(1000);
  ///////


}


void loop()
{



  if (waterThread.shouldRun())
    waterThread.run(); // запускаем поток
  byte pressedButton = getPressedButton();

  switch (pressedButton)
  {
    case 0:
      totalWater = 0;
      totalImp = 0;
      break;

  }


}

