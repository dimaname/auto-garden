import controlP5.*;

import processing.serial.*;
Serial port;
int impulseN =0;
ControlP5 cp5;
PFont font;
void setup() {
  size(1000, 700);
  background(0);
  cp5 = new ControlP5(this);

  Records.append(0);

  // List all the available serial ports
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  font = loadFont("BankGothicBT-Medium-48.vlw");
  textFont(font, 32);
  port = new Serial(this, Serial.list()[0], 9600);
  // create a new button with name 'buttonA'
  cp5.addButton("Next")  
    .setPosition(800, 30)
    .setSize(150, 30);
}
String inBuffer = "0";

int start_pos = 40;
int player =0;
IntList Records = new IntList();




void draw() {

  background(0, 0, 0);
  fill(255, 255, 255);
  textSize(32);

  while (port.available() > 0) {
    String tmp = trim( port.readString());

    if (tmp.length() != 0)
      impulseN = int( tmp  );
    println(tmp + "   "+ impulseN );
  }


  int height = impulseN*8+10;


  if (Records.get(player) < height) {
    Records.set(player, height );
  }

  for (int i = 0; i < Records.size(); i++) {
    int maxH = Records.get(i);
    fill(#462929);
    rect(40+i*(150+40), 700, 150, -maxH );
  }
  textAlign(CENTER);
  fill(#39B7F5);
  text(impulseN, start_pos+65, 700 - height-15); 
  int redC = min(255, impulseN*5);
  fill(255, 255-redC, 255-redC);
  rect(start_pos, 700, 150, -height );
}
public void Next() {
  if (player == 4) {
    player=0;
    start_pos = 40;
    Records.clear();
     Records.append(0);
  } else {
    player++;
    Records.append(0);
    start_pos+=150+40;
  }
}