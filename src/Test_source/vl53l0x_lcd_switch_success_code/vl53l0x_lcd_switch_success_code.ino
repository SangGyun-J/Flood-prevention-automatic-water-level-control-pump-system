#include <Wire.h>
// I2C LCD 라이브러리 설정
#include <LiquidCrystal_I2C.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X TOF = Adafruit_VL53L0X();

// 2번핀을 스위치 입력으로 설정
const int inputPin = 14;
const int inputPin1 = 12;

int distance = 0; //초기 높이 설정값
int distance1 = 0;
int distance2 = 0;

// LCD I2C address 설정 PCF8574:0x27, PCF8574A:0x3F
LiquidCrystal_I2C lcd(0x27,16,2);  // LCD address:0x27, 16X2 LCD or 0x3f

// 현재의 시간을 저장하기 위한 변수
long startTime = 0;
// 실제 스위치가 눌린 후 지연되는 시간
long swCountTimer = 0;

void setup()
{
  // 스위치 입력을 위하여 2번핀을 입력으로 설정하고 풀업시킨다
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(inputPin1, INPUT_PULLUP);
  
  // 9600 bps로 시리얼 통신 설정
  Serial.begin(9600);
  Serial.println("VL53L0X test");
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("VL53L0X test");
  delay(250);

  // LCD 백라이트를 두 번 점멸  
  lcd.noBacklight();  
  delay(250);
  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(250);
  lcd.backlight();
  delay(1000);

  if (!TOF.begin()) { // VL53L0X 기본 I2C 주소:0x29, 디버그 모드:false로 센서 준비.
    lcd.clear();
    lcd.setCursor(0,0); //Start at character 0 on line 0
    lcd.print("Failed to boot");
    lcd.setCursor(0,1);
    lcd.print("VL53L0X"); 
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  // Open Serial Monitor, Type to display 표시
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("VL53L0X");
  lcd.setCursor(0,1);
  lcd.print("start");  
}

void loop()
{
  VL53L0X_RangingMeasurementData_t measure;
  
  TOF.rangingTest(&measure, false); // true를 주면 디버그용 데이터를 받아옴
 
  // 이번 측정의 상태값. 장치 의존적인 값. 일반적으로 4면 에러. 0이면 측정값이 정상임을 나타냄.
  
  delay(250);

   if(measure.RangeStatus != 4) { 
    lcd.clear();
    delay(250);
    lcd.setCursor(0,0); 
    lcd.print("Distance (mm): "); 
    lcd.setCursor(0,1); 
    lcd.print(measure.RangeMilliMeter);
    
  } 
  else {
    lcd.clear();
    delay(250);
    lcd.setCursor(0,0); 
    lcd.print(" out of range ");
  }

  // 스위치 입력이 발생하였을 경우 실행
  if(digitalRead(inputPin1) == LOW){
    distance = measure.RangeMilliMeter;
    Serial.print("현재 초기 값은 "); Serial.print(distance);
    Serial.println(" mm 입니다.");
    
    lcd.clear();    //lcd를 모두 지운다.
    lcd.setCursor(0,0);
    lcd.print("now Distance(mm):");
    lcd.setCursor(0,1);
    lcd.print(distance);
    delay(2000);
    
  };

  // 스위치 입력이 발생하였을 경우 실행
  if(digitalRead(inputPin) == LOW){
    // 시리얼 통신으로 값을 출력한다. 
    if(measure.RangeStatus != 4) {  
      Serial.print("Distance (mm): "); 
      Serial.println(measure.RangeMilliMeter);
    } 
    else {
      Serial.println(" out of range ");
    }    
        
    lcd.clear();    //lcd를 모두 지운다.
    lcd.setCursor(0,0);
    lcd.print("Distance (mm):");
    lcd.setCursor(0,1);
    lcd.print(measure.RangeMilliMeter);//measure.RangeMilliMeter 값을 출력
    delay(2000);

    if(distance > measure.RangeMilliMeter){
      distance1 = distance - measure.RangeMilliMeter;
      distance2 = distance - distance1;
      Serial.print("처음보다 "); Serial.print(distance1);
      Serial.println(" mm 높아졌습니다.");
      Serial.print("남은 높이는 "); Serial.print(distance2);
      Serial.println(" mm 입니다.");

      lcd.clear();    //lcd를 모두 지운다.
      lcd.setCursor(0,0);
      lcd.print("beginning (mm):");
      lcd.setCursor(0,1);
      lcd.print(distance1);//measure.RangeMilliMeter 값을 출력
      delay(2000);
      
      lcd.clear();    //lcd를 모두 지운다.
      lcd.setCursor(0,0);
      lcd.print("remain (mm):");
      lcd.setCursor(0,1);
      lcd.print(distance2);//measure.RangeMilliMeter 값을 출력
      delay(2000);
    }
  };
}
