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
      Firebase.setFloat(firebaseData, "/database_test/Normal_pump", avg);
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
