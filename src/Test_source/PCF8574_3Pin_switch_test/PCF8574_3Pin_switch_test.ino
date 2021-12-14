//PCF8574
#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20);

const int button = P6;
int redPin = P3;
int greenPin = P4;
int bluePin = P5;



void setup() {
  // put your setup code here, to run once:
  pcf8574.pinMode(button, INPUT);
  pcf8574.pinMode(redPin, OUTPUT);
  pcf8574.pinMode(greenPin, OUTPUT);   
  pcf8574.pinMode(bluePin, OUTPUT);        
  pcf8574.begin(); //처음에 안써서 실행 안됨(주의)
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(pcf8574.digitalRead(button)==HIGH){
    setColor(0,1,1);  
  }
  else{
    setColor(1, 1, 1); //off
  }
  
  
}

void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
