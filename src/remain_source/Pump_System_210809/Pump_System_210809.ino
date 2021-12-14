#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf20(0x20); 
PCF8574 pcf21(0x21);
//PCF8574 pcf8574(0x20, D2(5), D1(4)); //PCF8574(주소 값, SDA, SCL)

//LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//VL53L1X
#include <VL53L1X.h> //Download this library from https://github.com/pololu/vl53l1x-arduino

//#include "SparkFun_VL53L1X.h"
//SFEVL53L1X distanceSensor; //레이저센서 거리값
int initial = 0; //초기 거리 저장 변수
int current = 0; //현재 거리 저장 변수
int count = 0; //수위 조건 충족 변수
int mes = 0; // 메세지 전송 변수
int avg = 0; // 레이저센서값 평균 변수

//레이저센서 5개
VL53L1X sensor_A; //Create the sensor object
VL53L1X sensor_B; //Create the sensor object
VL53L1X sensor_C; //Create the sensor object
VL53L1X sensor_D; //Create the sensor object
VL53L1X sensor_E; //Create the sensor object
 
int startTime = millis(); //used for our timing loop
int mInterval = 100; //refresh rate of 10hz
 
#define XSHUT_A P0 //pcf21
#define XSHUT_B P1 //pcf21
#define XSHUT_C P2 //pcf21
#define XSHUT_D P3 //pcf21
#define XSHUT_E P4 //pcf21
#define Raspberry_pi P5 //pcf21
 
#define M_INTERVAL 50

//ESP8266
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// 데이터베이스,와이파이 설정 
#include <FirebaseESP8266.h>  //https://github.com/mobizt/Firebase-ESP8266
#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
#define FIREBASE_Authorization_key "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv"
FirebaseData firebaseData;
FirebaseJson json;
// 데이터베이스 ON/OFF 표시 auto, pump 변수
String autoval; // AUTO모드 변수
String pumpval; // PUMP모드 변수
String ONval = "\\\"ON\\\""; //ON 값 저장
String OFFval = "\\\"OFF\\\""; // OFF 값 저장
//String appauto = "\\\"OFF\\\""; // 
//String apppump = "\\\"OFF\\\"";
int app = 0; //앱에서 AUTO 버튼이 ON인지 OFF인지 판별하는 변수 

const char* ssid = "우옹";
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

//char XSHUT_laserStr[5] = {'XSHUT_A', 'XSHUT_B', 'XSHUT_C', 'XSHUT_D', 'XSHUT_E'};
//char *sensor_laserStr[] = {"sensor_A", "sensor_B", "sensor_C", "sensor_D", "sensor_E"};
//char sensor_timeout[5] = {'sensor_A.setTimeout', 'sensor_B.setTimeout', 'sensor_C.setTimeout', 'sensor_D.setTimeout', 'sensor_E.setTimeout'};

//int laserNum[5] = {42, 43, 44, 45, 46};

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
    //파이어베이스 DB OFF로 초기화, APPAUTO,PUMP는 어플
    Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", OFFval);
    Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", OFFval);
//    Firebase.setString(firebaseData, "/testPump_in_server_DB/APPAUTO", OFFval);
//    Firebase.setString(firebaseData, "/testPump_in_server_DB/APPPUMP", OFFval);
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

    //Set the pin mode to output
    pcf21.pinMode(XSHUT_A ,OUTPUT);
    pcf21.pinMode(XSHUT_B ,OUTPUT);
    pcf21.pinMode(XSHUT_C ,OUTPUT);
    pcf21.pinMode(XSHUT_D ,OUTPUT);
    pcf21.pinMode(XSHUT_E ,OUTPUT);
    pcf21.pinMode(Raspberry_pi ,OUTPUT);
    pcf21.begin();

    //Turn all TOF's off
    pcf21.digitalWrite(XSHUT_A, LOW);
    pcf21.digitalWrite(XSHUT_B, LOW);
    pcf21.digitalWrite(XSHUT_C, LOW);
    pcf21.digitalWrite(XSHUT_D, LOW);
    pcf21.digitalWrite(XSHUT_E, LOW);
  
    //Raspberry_pi, Raspberry_pi_LCD ON
    pcf21.digitalWrite(Raspberry_pi, HIGH);

    //배열 활용 반복
//    for (int s=0; s<5; s++){
//      delay(50);
//      pcf21.digitalWrite(XSHUT_laserStr[s], HIGH); //Turn sensor_A on
//      delay(50);
//      sprintf(i, "%s", sensor_laserStr[s]);
//      i.setTimeout(500); //Set the sensors timeout
//      //sensor_laserStr[s].setTimeout(500);
//      //sensor_A.setTimeout(500);
//      if (!sensor_laserStr[s].init())//try to initilise the sensor
//      {
//          //Sensor does not respond within the timeout time
//          Serial.print(sensor_laserStr[s]);
//          Serial.println(" is not responding, check your wiring");
//      }
//      else
//      {
//          sensor_laserStr[s].setAddress(laserNum[s]); //Set the sensors I2C address
//          sensor_laserStr[s].setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
//          sensor_laserStr[s].setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
//          sensor_laserStr[s].startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
//      }   
//    }

    //-----------------------------------------------------------------
    //FIRST WE WILL CONFIGURE AND SETUP SENSOR_A
    //-----------------------------------------------------------------
    delay(50);
    pcf21.digitalWrite(XSHUT_A, HIGH); //Turn sensor_A on
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
 
    //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_B
    //-----------------------------------------------------------------
    delay(50);
    pcf21.digitalWrite(XSHUT_B, HIGH); //Turn sensor_B on
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
    pcf21.digitalWrite(XSHUT_C, HIGH); //Turn sensor_C on
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
    pcf21.digitalWrite(XSHUT_D, HIGH); //Turn sensor_D on
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
    pcf21.digitalWrite(XSHUT_E, HIGH); //Turn sensor_E on
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

    lcd.init(); // LCD 설정
    lcd.clear();      // LCD를 모두 지운다.
    lcd.backlight();  // 백라이트를 켠다. 
    // 시스템, 팀명, 이름 표시     
    lcd.setCursor(0,0);
    lcd.print("Flood Protection");
    delay(100);
    lcd.setCursor(0,1);
    lcd.print("Auto Pump System");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("I do not like"); 
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("the rainy season");
    delay(500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Dong Min Park");
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("Sang Gyun Jang");
    delay(500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Kyung Hoon Ko");
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("ST. Road");
    delay(500);    
}

bool Auto = LOW;
bool Pump = LOW;
char buf[40];

void loop() {  
  setColor(1, 0, 1); //녹    

  WiFiClient client = server.available();

  
//  if (client) {
//    boolean current_line_is_blank = true;
//    while (client.connected()){
//      if (client.available()){
//        // Read the first line of the request
//        String request = client.readStringUntil('\n');
//        Serial.println(request);        
//                
//        //자동, 수동        
//        if (request.indexOf("/Auto=ON") != -1)  {  
//          Serial.println(F("Auto on")); 
//          pcf20.digitalWrite(button2, HIGH); 
//          Auto = HIGH;
//          Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", ONval);          
//        }
//        if (request.indexOf("/Auto=OFF") != -1)  { 
//          Serial.println(F("Auto off")); 
//          pcf20.digitalWrite(button2, LOW); 
//          Auto = LOW;
//          Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", OFFval);
//        }        
//
//        //펌프        
//        if (request.indexOf("/Pump=ON") != -1)  {
//          Serial.println(F("Pump on"));
//          pcf20.digitalWrite(button3, HIGH); 
//          Pump = HIGH;
//          Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", ONval);             
//        }
//        if (request.indexOf("/Pump=OFF") != -1)  {
//          Serial.println(F("Pump off")); 
//          pcf20.digitalWrite(button3, LOW); 
//          Pump = LOW;
//          Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", OFFval);
//        } 
//                  
//      }
//      delay(100);
//      client.stop();
//    }
//  }

//  if((millis()- startTime) > mInterval)
//    {

//        startTime = millis();
//    }

        sprintf(buf, "%4d,%4d,%4d,%4d,%4d",sensor_A.read(),sensor_B.read(),sensor_C.read(),sensor_D.read(),sensor_E.read());
        //Serial.println(buf); //Get a reading in millimeters
        
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
        Serial.print("평균 : ");
        Serial.println(avg);
        //Firebase 데이터베이스에 레이져 평균 거리값 저장(실시간)
        Firebase.setFloat(firebaseData, "/testPump_in_server_DB/DISTANCE", avg);
        
        //분산
        int var = (pow((laser1-avg),2) + pow((laser2-avg),2) + pow((laser3-avg),2) + pow((laser4-avg),2) + pow((laser5-avg),2))/5;
        
        //표준편차
        int StandDev = sqrt(var);
        Serial.print("표준편차 : ");
        Serial.println(StandDev);
    

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
    
  if(pcf20.digitalRead(button2)==HIGH){ //자동모드
    Serial.println("자동모드 입니다.");
    setColor(1, 1, 0); //파   
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode");
    //Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", ONval);    
    delay(1000);  

    /////////// 자동모드이면 자동으로 펌프 작동///////////////
    if(avg < (initial/2)){
      Serial.println("펌프 작동");
      Serial.println("수위가 50%이상 찼습니다.");
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨      
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");    
      delay(1000);
    }
    else if(initial != 0 && avg > (initial/2)){
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");    
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
  else {                                     //수동모드
    Serial.println("수동모드 입니다.");
    setColor(1, 1, 1); //off    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode OFF");
    //Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", OFFval);    
    delay(1000);

    /////////// 수동모드이면 수동으로 펌프 작동///////////////
    if(pcf20.digitalRead(button3)==HIGH){      
      Serial.println("펌프 작동");      
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");
      //Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", ONval);
      delay(1000);
    }
    else if(pcf20.digitalRead(button3)==LOW){ 
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");
      //Firebase.setString(firebaseData, "/testPump_in_server_DB/PUMP", OFFval);    
      delay(1000);
    } 
  }
    //데이터베이스값 반복적으로 받아옴 ON/OFF

  Firebase.getString(firebaseData, "/testPump_in_server_DB/PUMP");
  pumpval = firebaseData.stringData();
  ////////////////////////////////////////////////////////
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
}

void setColor(int red, int green, int blue)
{
  pcf20.digitalWrite(redPin, red);
  pcf20.digitalWrite(greenPin, green);
  pcf20.digitalWrite(bluePin, blue); 
}
