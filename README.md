# IoT를 활용한 수위 조절 시스템
## 진행기간 : 2021-04-13 ~ 2021-11-30

### 🌧 나는 장마가 싫어 팀
|팀원|역할|
|---|---|
|박동민 🔵 |팀장(Project Manager), H/W, S/W, 3D Print|
|장상균 🟢 |Web, App, DB, SMS, S/W, 3D Print|

### 사용기술
---
+ <img src="https://img.shields.io/badge/C++-00599C?style=flat-square&logo=C%2B%2B&logoColor=white"/> <img src="https://img.shields.io/badge/C-A8B9CC?style=flat-square&logo=C&logoColor=white"/> <img src="https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=Arduino&logoColor=white"/> <img src="https://img.shields.io/badge/Node.js-339933?style=flat-square&logo=Node.js&logoColor=white"/> - 개발 언어
+ <img src="https://img.shields.io/badge/GitHub-181717?style=flat-square&logo=GitHub&logoColor=white"/> - 형상관리
+ ![app icon](https://user-images.githubusercontent.com/58980007/146013473-85e258d5-2b7d-464c-8215-f642c8ae2b78.png) - Application 제작
+ <img src="https://img.shields.io/badge/Firebase-FFCA28?style=flat-square&logo=Firebase&logoColor=white"/> – Realtime Database
+ ![whois icon](https://user-images.githubusercontent.com/58980007/146014071-7b971618-8429-474b-a706-a41e6e683fa5.png) - SMS 서비스
+ <img src="https://img.shields.io/badge/Raspberry Pi-A22846?style=flat-square&logo=Raspberry Pi&logoColor=white"/> - LCD 패널
+ <img src="https://img.shields.io/badge/Autodesk-0696D7?style=flat-square&logo=Autodesk&logoColor=white"/> - Fusion 360

### 설명
---
+ 2021 한이음 ICT 멘토링에서 진행 했던 팀 프로젝트입니다.
+ 최근 이상기후 때문에 장마임에도 불구하고 비가 국지성 집중호우와 같이 짧은 시간에 많이 내려서 발생하는 침수 피해를 방지하는 시스템입니다.
+ 조건을 만족하면 담당자에게 위험 알림 메시지를 전송합니다.
+ Application를 통해 원격으로 제어할 수 있습니다.

### 기능
---
- Application을 이용한 펌프 원격 제어 (Firebase, App Inventer)
- 특정 조건을 만족 시 SMS 서비스 API를 통해 메시지 전송
- Application과 ESP8266의 서버를 통한 통신 및 데이터 전송
- ESP32와 레이저 센서를 통한 거리 측정
- Relay를 이용한 펌프 제어 및 작동

### 개발 기획
---
- 최근 이상기후 때문에 여름 장마철이 되면 저지대에 위치한 아파트들이 지하에서 물이 넘쳐서 피해를 보는 일을 뉴스를 통해 자주 보게 되었다. 기존에 설치되어 있는 펌프가 여러 제약 때문에 정상 작동하지 않아 생기는 침수 피해를 방지하기 위해 범람 방지 자동 수위 조절 펌프 시스템을 사용하여 물이 넘쳐흐르는 일이 없도록 제어할 필요가 있다.


## 소스코드
### Main System - esp8266_button.ino
```C
#include "Arduino.h"
#include "PCF8574.h"
#include "math.h"
PCF8574 pcf20(0x20); 

//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int initial = 0; //초기 거리 저장 변수
int current = 0; //현재 거리 저장 변수
int count = 0; //수위 조건 충족 변수
int mes = 0; // 메세지 전송 변수
int avg = 0; // 레이저센서값 평균 변수

//ESP8266
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// 데이터베이스,와이파이 설정 
#include <FirebaseESP8266.h>  //https://github.com/mobizt/Firebase-ESP8266
#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
#define FIREBASE_Authorization_key ""
FirebaseData firebaseData;
FirebaseJson json;
// 데이터베이스 ON/OFF 표시 auto, pump 변수
String autoval; // AUTO모드 변수
String pumpval; // PUMP모드 변수
String ONval = "\\\"ON\\\""; //ON 값 저장
String OFFval = "\\\"OFF\\\""; // OFF 값 저장

const char* ssid = "wong";
const char* password = "12340000";
//const char* ssid = "iptimea704";
//const char* password = "rodemeng2019";
WiFiServer server(80);

//switch or led
int bluePin = P0;
int greenPin = P1;
int redPin = P2;
const int button1 = P3; //초기 값
const int button2 = P4; //Auto Mode
const int button3 = P5; //Pump

//펌프
const int pump = 14;

void setup() {
    Serial.begin(115200);
    
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting..");    
    }
    
    Serial.println("연결되었습니다.");

    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    //WIFI 연결 후 파이어베이스 연결 
    Firebase.begin(FIREBASE_HOST,FIREBASE_Authorization_key);
    Serial.println("Firebase connect");
    //파이어베이스 DB OFF로 초기화, 초기값 0설정
    Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", OFFval);
    Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", OFFval);
    Firebase.setFloat(firebaseData, "/testPump_in_server_DB/INI", initial);
    autoval = OFFval; // 시작할 때 auto OFF
    pumpval = OFFval; // 시작할 때 pump OFF
    
    pinMode(pump, OUTPUT);
  
    pcf20.pinMode(button1, INPUT_PULLUP);
    pcf20.pinMode(button2, INPUT);
    pcf20.pinMode(button3, INPUT);
    pcf20.pinMode(redPin, OUTPUT);
    pcf20.pinMode(greenPin, OUTPUT);   
    pcf20.pinMode(bluePin, OUTPUT);        
    pcf20.begin(); //처음에 안써서 실행 안됨(주의)

    lcd.init(); // LCD 설정
    lcd.clear();      // LCD를 모두 지운다.
    lcd.backlight();  // 백라이트를 켠다. 
    // 시스템, 팀명, 이름 표시     
    lcd.setCursor(0,0);
    lcd.print("Flood Protection");
    delay(1500);
    lcd.setCursor(0,1);
    lcd.print("Auto Pump System");
    delay(1500);
    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("I do not like"); 
//    delay(1500);
//    lcd.setCursor(0,1);
//    lcd.print("The rainy season");
//    delay(1500);
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("Dong Min Park");
//    delay(1500);
//    lcd.setCursor(0,1);
//    lcd.print("Sang Gyun Jang");
//    delay(1500);
}


void loop() {  
  setColor(1, 0, 1); //녹    

  WiFiClient client = server.available();
  //레이저센서값 받아오기
  Firebase.getInt(firebaseData, "/testPump_in_server_DB/DISTANCE");
  avg = firebaseData.intData();

  if(pcf20.digitalRead(button1)==LOW){
    initial = avg;
    Firebase.setFloat(firebaseData, "/testPump_in_server_DB/INI", avg);
    Serial.print("초기 값은 "); 
    Serial.print(initial); 
    Serial.println(" mm 입니다.");
    setColor(1, 0, 0); //청록    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Initial Distance");
    lcd.setCursor(0,1); 
    lcd.print(initial);
    lcd.print(" mm");
    delay(1000);    
  }

  Firebase.getString(firebaseData, "/testPump_in_server_DB/AUTO");
  autoval = firebaseData.stringData();     
  Firebase.getString(firebaseData, "/testPump_in_server_DB/PUMP");
  pumpval = firebaseData.stringData();
    
  if(autoval == ONval){
    pcf20.digitalWrite(button2, HIGH);
    }
  if(autoval == OFFval){
    pcf20.digitalWrite(button2, LOW);
    }
  if(pumpval == ONval){
    pcf20.digitalWrite(button3, HIGH);
    }
  if(pumpval == OFFval){
    pcf20.digitalWrite(button3, LOW);
    }
    
  if(pcf20.digitalRead(button2)==HIGH){ //자동모드
    Serial.println("자동모드 입니다.");
    setColor(1, 1, 0); //파   
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode");
    if(autoval == OFFval && pumpval == OFFval){
      Firebase.setString(firebaseData, "/testPump_in_server_DB/APPAUTO", ONval);
    }
    delay(1000);  

    /////////// 자동모드이면 자동으로 펌프 작동///////////////
    if(avg < (initial/10*6)){
      Serial.println("펌프 작동");
      Serial.println("수위가 50%이상 찼습니다.");
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨      
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");
      if(autoval == ONval){
        Firebase.setString(firebaseData, "/testPump_in_server_DB/NOWMAIN", ONval); 
      }
      else{
        Firebase.setString(firebaseData, "/testPump_in_server_DB/NOWMAIN", ONval); 
      }
      delay(1000);
    }
    else if(initial != 0 && avg > (initial/2)){
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");
      if(digitalRead(pump) == LOW && autoval == ONval){
        Firebase.setString(firebaseData, "/testPump_in_server_DB/NOWMAIN", OFFval);     
      }else if(digitalRead(pump) == LOW && digitalRead(button2) == HIGH){
        Firebase.setString(firebaseData, "/testPump_in_server_DB/NOWMAIN", OFFval); 
      }
      delay(1000);
    }

    if(avg < (initial/10*4)){
      Serial.println("수위가 60%이상 찼습니다.");
      count++;
    }
    if(count==5){
      mes=1;
      count=0;
    }
    //---------------------------- 문자 전송 툴
    //문자 입력시에 띄어쓰기는 %20으로 작성해야함. 아니면 작동X
    //Ex) 문자%20테스트입니다. -> 문자 테스트입니다. 
    String message = "http://www.all-tafp.org/EmmaSMS_PHP_UTF-8_JSON/example.php?sms_msg=수위60%이상!%20수위60%이상!%20지금%20즉시%20현장으로%20와주시기바랍니다.&sms_to=010-8338-2385&sms_from=010-8338-2385&sms_date=";
    if(mes == 1){
      Serial.println("메세지 보냄");
      Serial.println("수위 60%이상 즉시 현장으로!!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("send mes");
      delay(3000);
      lcd.clear();        
      if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
        HTTPClient http;  //Declare an object of class HTTPClient
        
        http.begin(message);  //Specify request destination
        int httpCode = http.GET(); 
      
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
    }//mes
  
    if(mes==1){
      mes=0;
    }
  }//button2(AUTO) == HIGH
  
  else {  //수동모드
    Serial.println("수동모드 입니다.");
    setColor(1, 1, 1); //off    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode OFF");
    if(autoval == OFFval && pumpval == OFFval){
     Firebase.setString(firebaseData, "/testPump_in_server_DB/APPAUTO", OFFval); 
    }     
    delay(1000);

    /////////// 수동모드이면 수동으로 펌프 작동///////////////
    if(pcf20.digitalRead(button3)==HIGH && pcf20.digitalRead(button2)==LOW){      
      Serial.println("펌프 작동");      
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");
      if(autoval == OFFval && pumpval == OFFval){
        Firebase.setString(firebaseData, "/testPump_in_server_DB/APPPUMP", ONval); 
      } 
      
      delay(1000);
    }
    else if(pcf20.digitalRead(button3)==LOW){ 
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");
      if(autoval == OFFval && pumpval == OFFval){
        Firebase.setString(firebaseData, "/testPump_in_server_DB/APPPUMP", OFFval); 
      }   
      delay(1000);
    } 
  }//수동모드
} //loop

void setColor(int red, int green, int blue)
{
  pcf20.digitalWrite(redPin, red);
  pcf20.digitalWrite(greenPin, green);
  pcf20.digitalWrite(bluePin, blue); 
}
```
메인 시스템 소스코드이다. 펌프를 제어하고 Firebase와 실시간으로 데이터를 주고 받고 Application을 통해 원격으로 제어하며 위험 상황시 SMS를 전송한다.

### Laser Sensor - esp32_laser_Nopcf.ino
```C
#include "Arduino.h"
#include "PCF8574.h"
#include "math.h"

PCF8574 pcf21(0x21);
//PCF8574 pcf8574(0x20, 21, 22)); //PCF8574(주소 값, SDA, SCL)


//VL53L1X
#include <VL53L1X.h> //Download this library from https://github.com/pololu/vl53l1x-arduino
int avg = 0; // 레이저센서값 평균 변수

/////////// 2m짜리 레이저 테스트/////////////
//#include <VL53L0X.h>
//VL53L0X sensor;
//////////////////////////////////////


//레이저센서 5개
VL53L1X sensor_A; //Create the sensor object
VL53L1X sensor_B; //Create the sensor object
VL53L1X sensor_C; //Create the sensor object
VL53L1X sensor_D; //Create the sensor object
VL53L1X sensor_E; //Create the sensor object
 
int startTime = millis(); //used for our timing loop
int mInterval = 100; //refresh rate of 10hz
 
#define XSHUT_A 27 // GPIO27 ESP32 순서대로 넣은거임
#define XSHUT_B 26 // GPIO26
#define XSHUT_C 25 // GPIO25
#define XSHUT_D 33 // GPIO33
#define XSHUT_E 32 // GPIO32
//#define Raspberry_pi P5 //pcf21
 
#define M_INTERVAL 50

//ESP8266
#include <Wire.h>
//#include <WiFiClient.h>
#include <WiFi.h>

//#include <ESP8266HTTPClient.h>

// 데이터베이스,와이파이 설정 
#include <FirebaseESP32.h>  //https://github.com/mobizt/Firebase-ESP32
#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
#define FIREBASE_Authorization_key "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv"
FirebaseData firebaseData;
FirebaseJson json;
// 데이터베이스 ON/OFF 표시 auto, pump 변수
String autoval; // AUTO모드 변수
String pumpval; // PUMP모드 변수
String ONval = "\\\"ON\\\""; //ON 값 저장
String OFFval = "\\\"OFF\\\""; // OFF 값 저장

const char* ssid = "wong";
const char* password = "12340000";
//const char* ssid = "iptimea704";
//const char* password = "rodemeng2019";
WiFiServer server(80);



void setup() {
    Serial.begin(115200);
    
    Wire.begin();
//    Wire.setClock(400000); // use 400 kHz I2C
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting..");    
    }
    
    Serial.println("연결되었습니다.");

    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    //WIFI 연결 후 파이어베이스 연결 
    Firebase.begin(FIREBASE_HOST,FIREBASE_Authorization_key);
    Serial.println("Firebase connect");
    //파이어베이스 DB OFF로 초기화, 초기값 0설정
//    Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", OFFval);
//    autoval = OFFval; // 시작할 때 auto OFF
//    pumpval = OFFval; // 시작할 때 pump OFF
    
    

    //Set the pin mode to output
    pinMode(XSHUT_A ,OUTPUT);
    pinMode(XSHUT_B ,OUTPUT);
    pinMode(XSHUT_C ,OUTPUT);
    pinMode(XSHUT_D ,OUTPUT);
    pinMode(XSHUT_E ,OUTPUT);

    //Turn all TOF's off
    digitalWrite(XSHUT_A, LOW);
    digitalWrite(XSHUT_B, LOW);
    digitalWrite(XSHUT_C, LOW);
    digitalWrite(XSHUT_D, LOW);
    digitalWrite(XSHUT_E, LOW);
  
//레이저 2M
/////////////////////////////////////////////////////////////////
//  sensor.setTimeout(500);
//  if (!sensor.init())
//  {
//    Serial.println("Failed to detect and initialize sensor!");
//    while (1) {}
//  }
  
  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  //  sensor.startContinuous();
/////////////////////////////////////////////////////////////////

//    //-----------------------------------------------------------------
//    //FIRST WE WILL CONFIGURE AND SETUP SENSOR_A
//    -----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_A, HIGH); //Turn sensor_A on
    delay(50);
    
    sensor_A.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_A.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_A is not responding, check your wiring");
    }
    else
    {
        sensor_A.setAddress(42); //Set the sensors I2C address
        sensor_A.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_A.setMeasurementTimingBudget(4000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_A.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }   
 
//    //-----------------------------------------------------------------
//    //NOW CONFIGURE AND SETUP SENSOR_B
//    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_B, HIGH); //Turn sensor_B on
    delay(50);
    
    sensor_B.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_B.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_B is not responding, check your wiring");
    }
    else
    {
        sensor_B.setAddress(43); //Set the sensors I2C address
        sensor_B.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_B.setMeasurementTimingBudget(4000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_B.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }
 
      //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_C
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_C, HIGH); //Turn sensor_C on
    delay(50);
    
    sensor_C.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_C.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_C is not responding, check your wiring");
    }
    else
    {
        sensor_C.setAddress(44); //Set the sensors I2C address
        sensor_C.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_C.setMeasurementTimingBudget(4000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_C.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    } 
 
    //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_D
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_D, HIGH); //Turn sensor_D on
    delay(50);
    
    sensor_D.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_D.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_D is not responding, check your wiring");
    }
    else
    {
        sensor_D.setAddress(45); //Set the sensors I2C address
        sensor_D.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_D.setMeasurementTimingBudget(4000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_D.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }

     //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_E
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_E, HIGH); //Turn sensor_E on
    delay(50);
    
    sensor_E.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_E.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_E is not responding, check your wiring");
    }
    else
    {
        sensor_E.setAddress(46); //Set the sensors I2C address
        sensor_E.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_E.setMeasurementTimingBudget(4000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_E.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }
     
}//setup


void loop() {  
  WiFiClient client = server.available();
  
// 레이저 2M 짜리
//    int laser1 = sensor.readRangeContinuousMillimeters();   
     
  int laser1 = sensor_A.read();
  int laser2 = sensor_B.read();
  int laser3 = sensor_C.read();
  int laser4 = sensor_D.read();
  int laser5 = sensor_E.read();

  Serial.print("laser1 : ");
  Serial.println(laser1);
  Serial.print("laser2 : ");
  Serial.println(laser2);
  Serial.print("laser3 : ");
  Serial.println(laser3);
  Serial.print("laser4: ");
  Serial.println(laser4);
  Serial.print("laser5 : ");
  Serial.println(laser5);

  //평균
  avg = (laser1 + laser2 + laser3 + laser4 + laser5)/5;
//  avg = (laser1 + laser2 + laser4 + laser5)/4;
//    avg = laser5;
//   avg = laser3;
//    avg = laser2;
//    avg = laser1;
//    avg = (laser1 + laser2 + laser4)/3;
  Serial.print("평균 : ");
  Serial.println(avg);
  //Firebase 데이터베이스에 레이져 평균 거리값 저장(실시간)
  Firebase.setFloat(firebaseData, "/testPump_in_server_DB/DISTANCE", avg);
  delay(1500);
//    Data 업로드 및 시각화하기 위한 코드 

//    센서 없이 노말 펌프 사용할 때
//      Firebase.setFloat(firebaseData, "/database_test/Normal_pump", avg);
//    Firebase.pushFloat(firebaseData, "/database_test/Normal_pump", avg);

//    레이저센서 및 오토펌프 사용할 때
//    Firebase.pushFloat(firebaseData, "/database_test/Auto_pump", avg);
  
  //분산
//  int var = (pow((laser1-avg),2) + pow((laser2-avg),2) + pow((laser3-avg),2) + pow((laser4-avg),2) + pow((laser5-avg),2))/5;
//  int var = (pow((laser1-avg),2) + pow((laser2-avg),2) + pow((laser4-avg),2) + pow((laser5-avg),2))/4;
//  int var = (pow((laser1-avg),2) + pow((laser2-avg),2) + pow((laser4-avg),2))/3;
  
  //표준편차
//  int StandDev = sqrt(var);
//  Firebase.setFloat(firebaseData, "/testPump_in_server_DB/STANDARD", StandDev);
//  Serial.print("표준편차 : ");
//  Serial.println(StandDev);
    

} //loop
```
5개의 레이저 센서 평균값을 Firebase로 보내는 코드이다.

### Application - App Inventor
![inventor2](https://user-images.githubusercontent.com/58980007/145999386-4e4cdfd7-8a4a-4342-9a62-4c7a27c2847f.PNG)
![inventor1](https://user-images.githubusercontent.com/58980007/145999483-0095e9e3-127b-4149-9b68-cfc4ae9b1971.PNG)
App Inventor를 통해 Application을 제작하였다.

### Web - index.html
![index](https://user-images.githubusercontent.com/58980007/146000300-ad32b4bb-4355-434b-8034-5ab0cc3b89b7.PNG)

메인 시스템 및 APP 작동 여부와 상태 확인이 가능한 홈페이지다.

### Web - app.js
![app](https://user-images.githubusercontent.com/58980007/146000198-76086454-f646-4b09-8f2d-68da35908952.png)
![firebase](https://user-images.githubusercontent.com/58980007/146000266-84975eb4-ca36-492b-aadb-11698370652b.PNG)

홈페이지에서 Firebase와 상호작용하기 위한 소스이다.

### SMS Service Code
![3](https://user-images.githubusercontent.com/58980007/145999893-7f6f9dbb-a3e7-448d-b96c-8f37d2c033f1.PNG)
SMS 전송을 하기 위한 소스코드이다.


## 3D Design
![3d](https://user-images.githubusercontent.com/58980007/146006765-3c6f6b63-7992-491e-8442-5a40ba2b6c8b.PNG)


## 완성사진
### Main System
![Main](https://user-images.githubusercontent.com/58980007/146001744-c7d1b735-e68d-48a5-b9f2-cabb238bf0c0.PNG)

### Laser Sensor
![Laser](https://user-images.githubusercontent.com/58980007/146003161-2e02dd0f-5390-4da5-ab3a-b780ac9ceeff.PNG)

### Application
![앱](https://user-images.githubusercontent.com/58980007/146003291-01a3b7d4-38ff-40a7-9daf-6f99c88fa3c2.PNG)

### Web
![WebPage](https://user-images.githubusercontent.com/58980007/146005969-112d15bc-854b-4828-acae-9df68e256653.PNG)

### SMS
![SMS](https://user-images.githubusercontent.com/58980007/146004098-4f9075f2-7573-4192-9899-3969d9dc244b.jpg)
