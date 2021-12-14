#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20); 
//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

const int button1 = P0;
const int button2 = P1;
const int button3 = P2;
int redPin = P3;
int greenPin = P4;
int bluePin = P5;

void setup() {
    Serial.begin(115200);
  
    pcf8574.pinMode(button1, INPUT_PULLUP);
    pcf8574.pinMode(button2, INPUT_PULLUP);
    pcf8574.pinMode(button3, INPUT);
    pcf8574.pinMode(redPin, OUTPUT);
    pcf8574.pinMode(greenPin, OUTPUT);   
    pcf8574.pinMode(bluePin, OUTPUT);        
    pcf8574.begin(); //처음에 안써서 실행 안됨(주의)

    lcd.init(); // LCD 설정
    lcd.clear();      // LCD를 모두 지운다.
    lcd.backlight();  // 백라이트를 켠다. 
    // Arduino LCD, Welcome 표시 
    lcd.setCursor(0,0);
    lcd.print("Arduino LCD");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Welcome");
    delay(1000);
}

void loop() {    
  if(pcf8574.digitalRead(button1)==LOW){
    setColor(0, 1, 1); //빨
    Serial.println("빨간색");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Red Color");
    delay(1000);
  }
  else if(pcf8574.digitalRead(button2)==LOW){
    setColor(1, 0, 1); //녹
    Serial.println("녹색");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Green Color");
    delay(1000);
  } 
  else if(pcf8574.digitalRead(button3)==LOW){
    setColor(1, 1, 0); //파
    Serial.println("파란색");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Blue Color");
    delay(1000);
  }   
  else{    
    setColor(1, 1, 1); //off
    Serial.println("LED OFF");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LED OFF");
    delay(1000);
  }
}

void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
