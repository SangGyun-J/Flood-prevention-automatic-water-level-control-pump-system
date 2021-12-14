#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

const int button1 = 14;
const int button2 = 12;
int switch1 = 0;
int count = 0;
int mes = 0;
SFEVL53L1X distanceSensor; //레이저센서 거리값

// 와이파이 설정 
const char* ssid = "iptimea704";
const char* password = "rodemeng2019";

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
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
    
  }
  Serial.println("연결되었습니다.");

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
    
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Distance (mm): ");
  lcd.setCursor(0,1); 
  lcd.print(distance);

  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;

  if(digitalRead(button1) == LOW){
      switch1 = distance;
      Serial.print("현재 초기 값은 "); Serial.print(switch1);
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

  if(distance<(switch1/10*7)){
    Serial.println("수위가 70%이상 찼습니다.");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("half the water level");
    count++;
    lcd.setCursor(0,1);
    lcd.print(count);
    Serial.println(count);
    delay(2000);
  }

  if(count==5){
    mes=1;
    count=0;
  }

  //---------------------------- 문자 전송 툴
      //문자 입력시에 띄어쓰기는 %20으로 작성해야함. 아니면 작동X
      //Ex) 문자%20테스트입니다. -> 문자 테스트입니다. 
      String message = "http://www.all-tafp.org/EmmaSMS_PHP_UTF-8_JSON/example.php?sms_msg=수위70%이상!%20수위70%이상!%20지금%20즉시%20현장으로%20와주시기바랍니다.&sms_to=010-8338-2385&sms_from=010-8338-2385&sms_date=";
      if(mes == 1){
        Serial.println("메세지 보냄");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("send mes");
        delay(3000);
        lcd.clear();
        
        
        if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
          HTTPClient http;  //Declare an object of class HTTPClient
      
          http.begin(message);  //Specify request destination
          int httpCode = http.GET(); 
          //밑에는 http 문서 그대로 시리얼에 출력해서 확인용.
      /*if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload  */
          Serial.println("코드 실행");
          for(int i=40; i>0; i--){
          lcd.setCursor(0,0);
          lcd.print("waiting signal");
          lcd.setCursor(0,1);
          lcd.print(i);
          delay(1000);
          lcd.clear();
        }
           
     } 
  }

  if(mes==1){
    mes=0;
  }
}
