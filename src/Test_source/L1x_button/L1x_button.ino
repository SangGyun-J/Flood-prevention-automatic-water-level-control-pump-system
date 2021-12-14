#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

const int button1 = 14;
const int button2 = 12;
int switch1 = 0;
SFEVL53L1X distanceSensor; //레이저센서 거리값

//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

  
LiquidCrystal_I2C lcd(0x27,16,2);
void setup(void)
{

  //스위치 입력을 위해서 버튼 풀업을 시켜준다.
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  
  Wire.begin();
  Serial.begin(115200);
  Serial.println("VL53L1X Qwiic Test");

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
  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");
}

void loop(void)
{
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1000);
  }
  
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  Serial.print("Distance(mm): ");
  Serial.print(distance);
  lcd.setCursor(0,0); 
  lcd.print("Distance (mm): "); 
  
  lcd.clear(); 
  lcd.print("Distance (mm): ");
  lcd.setCursor(0,1); 
  lcd.print(distance);

  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;

if(digitalRead(button1) == LOW){
    switch1 = distance;
    Serial.print("/n현재 초기 값은 "); Serial.print(switch1);
    Serial.println(" mm 입니다.");
    
    lcd.clear();    //lcd를 모두 지운다.
    lcd.setCursor(0,0);
    lcd.print("now Distance(mm):");
    lcd.setCursor(0,1);
    lcd.print(switch1);
    delay(2000);
    
  };
  Serial.print("\tDistance(ft): ");
  Serial.print(distanceFeet, 2);

  Serial.println();
}
