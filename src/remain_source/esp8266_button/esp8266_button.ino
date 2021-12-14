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
