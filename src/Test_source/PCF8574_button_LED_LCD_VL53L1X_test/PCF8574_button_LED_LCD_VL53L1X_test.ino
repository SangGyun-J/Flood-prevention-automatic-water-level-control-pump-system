//PCF8574
#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20); 
//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//VL53L1X
#include "SparkFun_VL53L1X.h"
SFEVL53L1X distanceSensor; //레이저센서 거리값
int switch1 = 0; //초기 값 저장 변수

//switch or led
const int button1 = P0;
const int button2 = P1;
const int button3 = P2;
int redPin = P3;
int greenPin = P4;
int bluePin = P5;

void setup() {
    Serial.begin(115200);
    Serial.println("연결되었습니다.");
  
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

    if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
    {
      Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
      while (1)
        ;
    }
}

void loop() {    
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1000);
  }

  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  Serial.print("Distance(mm): ");
  Serial.println(distance);
   
  if(pcf8574.digitalRead(button1)==LOW){
    switch1 = distance;
    Serial.print("현재 초기 값은 "); 
    Serial.print(switch1); 
    Serial.println(" mm 입니다.");
    setColor(0, 1, 1); //빨
    Serial.println("LED 색깔은 빨간색 입니다.");
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Now Distance (mm): ");
    lcd.setCursor(0,1); 
    lcd.print(switch1);
    delay(1000);
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Red Color");
    delay(1000);
  }
  else if(pcf8574.digitalRead(button2)==LOW){
    Serial.print("현재 값은 "); 
    Serial.print(distance); 
    Serial.println(" mm 입니다.");
    setColor(1, 0, 1); //녹
    Serial.println("LED 색깔은 녹색 입니다.");
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("New Distance (mm): ");
    lcd.setCursor(0,1); 
    lcd.print(distance);
    delay(1000);
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Green Color");
    delay(1000);
  } 
  else if(pcf8574.digitalRead(button3)==LOW){
    Serial.println("자동모드 입니다.");
    setColor(1, 1, 0); //파
    Serial.println("LED 색깔은 파란색 입니다.");
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode");
    lcd.setCursor(0,1); 
    lcd.print("Blue Color");
    delay(1000);    
  }   
  else{    
    Serial.println("수동모드 입니다.");
    setColor(1, 1, 1); //off
    Serial.println("LED OFF");
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode OFF");
    lcd.setCursor(0,1); 
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
