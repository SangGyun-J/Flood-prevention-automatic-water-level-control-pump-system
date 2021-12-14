#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20); 
//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

const int button = 14;
const int button1 = 12;

void setup() {
    Serial.begin(115200);
    //NodeMCU 텍스 버튼
    pinMode(button, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    
    //PCF8574 LED
    pcf8574.pinMode(P0, OUTPUT); 
    pcf8574.pinMode(P1, OUTPUT); 
    pcf8574.begin(); //처음에 안써서 실행 안됨(주의)
}

void loop() {
  if (digitalRead(button) == LOW) {
    pcf8574.digitalWrite(P0, LOW);   
    pcf8574.digitalWrite(P1, HIGH);  
  } 

  if (digitalRead(button1) == LOW) {
    pcf8574.digitalWrite(P0, HIGH);   
    pcf8574.digitalWrite(P1, LOW);  
  }    
}
