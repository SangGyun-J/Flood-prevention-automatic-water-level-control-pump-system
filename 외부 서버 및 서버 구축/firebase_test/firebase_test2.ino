#include <FirebaseESP8266.h>
#include  <ESP8266WiFi.h>

#include <Wire.h>

#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
#define WIFI_SSID "iptimea704" // Change the name of your WIFI
#define WIFI_PASSWORD "rodemeng2019" // Change the password of your WIFI
//#define WIFI_SSID "NEXT_8004N" // Change the name of your WIFI
//#define WIFI_PASSWORD "12340000" // Change the password of your WIFI
//#define WIFI_SSID "우옹" // Change the name of your WIFI
//#define WIFI_PASSWORD "12340000" // Change the password of your WIFI
#define FIREBASE_Authorization_key "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv"

FirebaseData firebaseData;
FirebaseJson json;
int data;
int ini = 0;
int percent; 
int avg;
void setup() {
////////////////////레이저 센서//////////////////// 
 Serial.begin(115200);
 Wire.begin();

   WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
   Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST,FIREBASE_Authorization_key);
  
}
String autoval;
String pumpval;
String ONval = "\\\"ON\\\"";
String OFFval = "\\\"OFF\\\"";
void loop() {
 //data = Firebase.getInt(firebaseData, "/testPump_in_server_DB/DISTANCE");
  Firebase.getInt(firebaseData, "/testPump_in_server_DB/DISTANCE");
  int dis = firebaseData.intData();
  
  Firebase.getString(firebaseData, "/testPump_in_server_DB/AUTO");
  autoval = firebaseData.stringData(); 
  
  Firebase.getString(firebaseData, "/testPump_in_server_DB/PUMP");
  pumpval = firebaseData.stringData();
  
//  Serial.println(data);
//  Serial.println("auto = " + autoval);
//  Serial.println("Pump = " + pumpval);
//  
  //OFFval = "\\\"OFF\\\"";
  //String ONval = "\\\"ON\\\"";
  if(autoval == OFFval){ //이게 정확한 조건문 표현식이니 이거 쓸것!
    Serial.println("오토모드 OFF 맞음");
  }
   if(autoval == ONval){ //이게 정확한 조건문 표현식이니 이거 쓸것!
    Serial.println("오토모드 ON 맞음");
  }

//  Firebase.getFloat(firebaseData, "/testPump_in_server_DB/DISTANCE");
//  avg = firebaseData.floatData();
//  percent = (ini/avg) * 100;
//  Serial.println("퍼센트 : " + percent);
  
//  Serial.println(dis);
  Serial.println("AUTO mode : "+ autoval+ "  Pump mode : " + pumpval);
  String aa = "\\\"ON\\\""; //이렇게 넣으면 인벤터에서 들어간대로 입력됨!
  //String bb = "\\\"OFF\\\"";
  //Firebase.setString(firebaseData, "/testPump_in_server_DB/AUTO", aa);
  
  Serial.println(aa);
//  Firebase.setFloat(firebaseData, "/testPump_in_server_DB/DISTANCE", Laser_val);
  delay(2000);
}
