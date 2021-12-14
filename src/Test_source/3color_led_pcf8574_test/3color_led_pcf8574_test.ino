#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20);

int redPin = P3;
int greenPin = P4;
int bluePin = P5;

void setup() {
    pcf8574.pinMode(redPin, OUTPUT);
    pcf8574.pinMode(greenPin, OUTPUT);   
    pcf8574.pinMode(bluePin, OUTPUT);      
    pcf8574.begin();
}

void loop() {
  setColor(0, 1, 1); //빨
  delay(1000);
  setColor(1, 0, 1); //녹
  delay(1000);
  setColor(1, 1, 0); //파
  delay(1000);
  setColor(0, 0, 1); //노
  delay(1000);
  setColor(0, 1, 0); //자홍
  delay(1000);
  setColor(1, 0, 0); //청록
  delay(1000);    
}

void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
