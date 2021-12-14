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
int count = 0; //수위 조건 충족 변수
int mes = 0; // 메세지 전송 변수

//ESP8266
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// 와이파이 설정 
const char* ssid = "iptimea704";
const char* password = "rodemeng2019";

//switch or led
int bluePin = P0;
int greenPin = P1;
int redPin = P2;
const int button1 = P3; //초기 값
const int button2 = P4; //현재 값
const int button3 = P5; //Auto Mode
const int button4 = P6; //ON-OFF

//펌프
const int pump = 14;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting..");    
    }
    
    Serial.println("연결되었습니다.");

    pinMode(pump, OUTPUT);
  
    pcf8574.pinMode(button1, INPUT_PULLUP);
    pcf8574.pinMode(button2, INPUT_PULLUP);
    pcf8574.pinMode(button3, INPUT);
    pcf8574.pinMode(button4, INPUT);
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
    Serial.println("Sensor online!");
}

void loop() {  
  setColor(1, 0, 1); //녹
    
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
    Serial.print("초기 값은 "); 
    Serial.print(switch1); 
    Serial.println(" mm 입니다.");
    setColor(1, 0, 0); //청록    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Now Distance (mm): ");
    lcd.setCursor(0,1); 
    lcd.print(switch1);
    delay(1000);    
  }
  
  if(pcf8574.digitalRead(button3)==LOW){ //자동모드
    Serial.println("자동모드 입니다.");
    setColor(1, 1, 0); //파   
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode");    
    delay(1000);  

    /////////// 자동모드이면 자동으로 펌프 작동///////////////
    if(distance < (switch1/2)){
      Serial.println("펌프 작동");
      Serial.println("수위가 50%이상 찼습니다.");
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨      
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");    
      delay(1000);
    }
    else if(switch1 != 0 && distance > (switch1/2)){
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");    
      delay(1000);
    }

    if(distance < (switch1/10*4)){
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
  else{                                     //수동모드
    Serial.println("수동모드 입니다.");
    setColor(1, 1, 1); //off    
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode OFF");    
    delay(1000);

    /////////// 수동모드이면 수동으로 펌프 작동///////////////
    if(pcf8574.digitalRead(button2)==LOW){
      Serial.print("현재 값은 "); 
      Serial.print(distance); 
      Serial.println(" mm 입니다.");
      setColor(1, 0, 0); //청록    
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("New Distance (mm): ");
      lcd.setCursor(0,1); 
      lcd.print(distance);
      delay(1000);    
    }

    if(pcf8574.digitalRead(button4)==HIGH){      
      Serial.println("펌프 작동");      
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");    
      delay(1000);
    }
    else if(pcf8574.digitalRead(button4)==LOW){
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");    
      delay(1000);      
    } 
  }  
}

void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
