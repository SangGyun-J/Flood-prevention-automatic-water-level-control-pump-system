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
int initial = 0; //초기 거리 저장 변수
int current = 0; //현재 거리 저장 변수
int count = 0; //수위 조건 충족 변수
int mes = 0; // 메세지 전송 변수

//ESP8266
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// 와이파이 설정 
const char* ssid = "502";
const char* password = "06060606";
WiFiServer server(80);

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

    server.begin();
    Serial.println("Server started");

    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

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
    // 시스템, 회사, 이름 표시     
    lcd.setCursor(0,0);
    lcd.print("Flood Protection");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Auto Pump System");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Rodem Technology");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Hwan Gyu Ryu");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Dong Min Park");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("Sang Gyun Jang");
    delay(2000);    

    if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
    {
      Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
      while (1)
        ;
    }
    Serial.println("Sensor online!");
}

const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head>"
                                 "<meta name=\"viewport\"content=\"width=device-width,initial-scale=1,user-scalable=no\"/>" 
                                 "<link rel=\"icon\" href=\"data:,\">";
const char HTTP_STYLE[] PROGMEM = "<style>" 
                                  "body{text-align:center;font-family:verdana;}" 
                                  "button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%} " 
                                  "</style>";
const char HTTP_HEAD_END[] PROGMEM = "</head><body><div style=\"text-align:center;display:inline-block;min-width:260px;\">" 
                                     "<h2>Flood Protection Auto Pump System</h2>";
const char BUTTON_TYPE[] PROGMEM = "<p>Initial Distance</p><a href=\"/Initial=Setting\"\"><button style=\"width:40%;background-color: #ff69b4;\">Setting</button></a></p>"
                                   "<p>Current Distance</p><a href=\"/Current=Measurement\"\"><button style=\"width:40%;background-color:#ff69b4;\">Measurement</button></a></p>"; 
const char BUTTON_A_ON[] PROGMEM = "<p>Auto Mode</p><a href=\"/Auto=ON\"><button style=\"width:60%;background-color:#12cb3d;\">ON</button></a></p>"; 
const char BUTTON_A_OFF[] PROGMEM = "<p>Auto Mode</p><a href=\"/Auto=OFF\"><button style=\"width:60%;background-color:#1fa3ec;\">OFF</button></a></p>"; 
const char BUTTON_B_ON[] PROGMEM = "<p>Pump</p><a href=\"/Pump=ON\"><button style=\"width:60%;background-color:#12cb3d;\">ON</button></a></p>"; 
const char BUTTON_B_OFF[] PROGMEM = "<p>Pump</p><a href=\"/Pump=OFF\"><button style=\"width:60%;background-color:#1fa3ec;\">OFF</button></a></p>"; 
const char HTTP_END[] PROGMEM = "</div></body></html>";

bool Auto = LOW;
bool Pump = LOW;
                                                          
void loop() {  
  setColor(1, 0, 1); //녹    

  WiFiClient client = server.available();
  if (client) {
    boolean current_line_is_blank = true;
    while (client.connected()){
      if (client.available()){
        // Read the first line of the request
        String request = client.readStringUntil('\n');
        Serial.println(request);        

        distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
        while (!distanceSensor.checkForDataReady())
        {
          delay(1000);
        }
      
        int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
        distanceSensor.clearInterrupt();
        distanceSensor.stopRanging();
        
        //초기 값                
        if (request.indexOf("/Initial=Setting") != -1)  {
          initial = distance;
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

        //현재 값        
        if (request.indexOf("/Current=Measurement") != -1)  {
          current = distance;
          Serial.print("현재 값은 "); 
          Serial.print(current); 
          Serial.println(" mm 입니다.");
          setColor(1, 0, 0); //청록
          lcd.clear(); 
          lcd.setCursor(0,0);
          lcd.print("Current Distance");
          lcd.setCursor(0,1); 
          lcd.print(current);
          lcd.print(" mm");
          delay(1000);              
        }
        
        //자동, 수동        
        if (request.indexOf("/Auto=OFF") != -1)  {  
          Serial.println(F("Auto on")); 
          pcf8574.digitalWrite(button3, HIGH); 
          Auto = HIGH;          
        }
        if (request.indexOf("/Auto=ON") != -1)  { 
          Serial.println(F("Auto off")); 
          pcf8574.digitalWrite(button3, LOW); 
          Auto = LOW;
        }        

        //펌프        
        if (request.indexOf("/Pump=OFF") != -1)  {
          Serial.println(F("Pump on")); 
          pcf8574.digitalWrite(button4, HIGH); 
          Pump = HIGH;             
        }
        if (request.indexOf("/Pump=ON") != -1)  {
          Serial.println(F("Pump off")); 
          pcf8574.digitalWrite(button4, LOW); 
          Pump = LOW;
        } 
             
        // Set ledPin according to the request
        //digitalWrite(ledPin, value);
       
        // Return the response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
//        client.println("Refresh: 1");
        client.println(""); //  do not forget this one
        String page; 
        page = (const __FlashStringHelper *)HTTP_HEAD; 
        page += (const __FlashStringHelper *)HTTP_STYLE; 
        page += (const __FlashStringHelper *)HTTP_HEAD_END; 
        page += (const __FlashStringHelper *)BUTTON_TYPE;         
        if (Auto == HIGH) { 
          page += (const __FlashStringHelper *)BUTTON_A_ON; 
          } 
        else { 
          page += (const __FlashStringHelper *)BUTTON_A_OFF; 
          } 
        if (Pump == HIGH) { 
          page += (const __FlashStringHelper *)BUTTON_B_ON; 
          } 
        else { 
          page += (const __FlashStringHelper *)BUTTON_B_OFF; 
          }
        page += (const __FlashStringHelper *)HTTP_END; 
        client.print(page); 
        client.println();

        client.println("<br><br>");

        //초기 값
        client.print("Initial Distance is now: ");
        client.print(initial);         
        client.println("<br><br>");

        //현재 값
        client.print("Current Distance is now: ");
        client.print(current);
        client.println("<br><br>");
        
        //자동, 수동
        client.println("Auto Mode is now: ");
        if(Auto == HIGH) {
          client.print("On");
        } else {
          client.print("Off");
        }
        client.println("<br><br>");      

        //펌프
        client.print("Pump is now: ");
        if(Pump == HIGH) {
          client.print("On");          
        } else {
          client.print("Off");
        }
        client.println("<br><br>");

        client.println("<br><br>");
        client.println("(Auto Mode ON -> Pump X)");
        client.println("<br><br>");
        client.println("(Auto Mode OFF -> Pump O)");       
      }
      delay(100);
      client.stop();
    }
  }
 
  // Wait until the client sends some data
//  Serial.println("new client");
//  while(!client.available()){
//    delay(1);
//  }
 
  
 
//  delay(1);
//  Serial.println("Client disonnected");
//  Serial.println("");
    
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
    initial = distance;
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
  
  if(pcf8574.digitalRead(button2)==LOW){
      current = distance;
      Serial.print("현재 값은 "); 
      Serial.print(current); 
      Serial.println(" mm 입니다.");
      setColor(1, 0, 0); //청록    
      lcd.clear(); 
      lcd.setCursor(0,0);
      lcd.print("Current Distance");
      lcd.setCursor(0,1); 
      lcd.print(current);
      lcd.print(" mm");
      delay(1000);    
    }
  
  if(pcf8574.digitalRead(button3)==HIGH){ //자동모드
    Serial.println("자동모드 입니다.");
    setColor(1, 1, 0); //파   
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("Auto Mode");    
    delay(1000);  

    /////////// 자동모드이면 자동으로 펌프 작동///////////////
    if(distance < (initial/2)){
      Serial.println("펌프 작동");
      Serial.println("수위가 50%이상 찼습니다.");
      digitalWrite(pump, HIGH);
      setColor(0, 1, 1); //빨      
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump ON");    
      delay(1000);
    }
    else if(initial != 0 && distance > (initial/2)){
      Serial.println("펌프 중지");
      digitalWrite(pump, LOW);
      setColor(1, 1, 1); //off
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Pump OFF");    
      delay(1000);
    }

    if(distance < (initial/10*4)){
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
    delay(1000);

    /////////// 수동모드이면 수동으로 펌프 작동///////////////
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
