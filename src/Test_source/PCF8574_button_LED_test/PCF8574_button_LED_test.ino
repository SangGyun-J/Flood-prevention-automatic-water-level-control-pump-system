#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20); 
//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

const int button1 = P0;
const int button2 = P1;
const int button3 = P2;
int redPin = P3;
int greenPin = P4;
int bluePin = P5;

void setup() {
    pcf8574.pinMode(button1, INPUT_PULLUP);
    pcf8574.pinMode(button2, INPUT_PULLUP);
    pcf8574.pinMode(button3, INPUT);
    pcf8574.pinMode(redPin, OUTPUT);
    pcf8574.pinMode(greenPin, OUTPUT);   
    pcf8574.pinMode(bluePin, OUTPUT);        
    pcf8574.begin(); //처음에 안써서 실행 안됨(주의)
}

void loop() {
  if(pcf8574.digitalRead(button1)==LOW){
    setColor(0, 1, 1); //빨
  }
  else if(pcf8574.digitalRead(button2)==LOW){
    setColor(1, 0, 1); //녹
  } 
  else if(pcf8574.digitalRead(button3)==LOW){
    setColor(1, 1, 0); //파
  }   
  else{    
    setColor(1, 1, 1); //off
  }
  delay(1000);
}
void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
