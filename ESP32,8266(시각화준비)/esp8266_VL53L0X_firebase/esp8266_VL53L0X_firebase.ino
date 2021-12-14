//#include <FirebaseESP8266.h> 

#include  <ESP8266WiFi.h>
// https://github.com/FirebaseExtended/firebase-arduino 
#include <FirebaseArduino.h>
// arduinoJson.h 버전 5.13.1로 해야함. 
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
// 시간 라이브러리
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;

#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com" //--> URL address of your Firebase Realtime Database.
#define FIREBASE_AUTH "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv" //--> Your firebase database secret code.

//#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
//#define FIREBASE_Authorization_key "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv"
//#define WIFI_SSID "iptimea704" // Change the name of your WIFI
//#define WIFI_PASSWORD "rodemeng2019" // Change the password of your WIFI
#define WIFI_SSID "316ho jumin" // Change the name of your WIFI
#define WIFI_PASSWORD "12340000" // Change the password of your WIFI


//----------------------------------------Define NTP Client to get time
// See the UTC time offset list here : https://en.wikipedia.org/wiki/List_of_UTC_time_offsets
// > UTC +09:00(한국) -> 9 * 60 * 60 = 32400
const long utcOffsetInSeconds = 32400;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
//----------------------------------------

String DBnm = "database_test";
String TD = "Distance";
String HD = "Humidity";

//<FirebaseESP8266.h> 라이브러리 
//FirebaseData firebaseData;
//FirebaseJson json;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
  
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  timeClient.begin();  
//  Firebase.begin(FIREBASE_HOST,FIREBASE_Authorization_key);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
  //--------------------------time--------------------------------
  timeClient.update();
    String hr, mn, sc;
//    String formattedTime = timeclient.getFormattedTime();
    if (timeClient.getHours() < 10) {
      hr = "0" + String(timeClient.getHours());
    }
    else {
      hr = String(timeClient.getHours());
    }
    
    if (timeClient.getMinutes() < 10) {
      mn = "0" + String(timeClient.getMinutes());
    }
    else {
      mn = String(timeClient.getMinutes());
    }
  
    if (timeClient.getSeconds() < 10) {
      sc = "0" + String(timeClient.getSeconds());
    }
    else {
      sc = String(timeClient.getSeconds());
    }
    
    String TimeNow = hr + ":" + mn + ":" + sc;
    Serial.println(TimeNow);
//--------------------------time--------------------------------

  float Laser_val = sensor.readRangeContinuousMillimeters();
  Serial.println(Laser_val);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  
  String strDis = String(Laser_val, 0);
  String DBaddH = DBnm + "/" + TimeNow + "/" + TD;
  Serial.println(strDis);
  Firebase.pushFloat("/distance_test/"+TimeNow ,Laser_val); //--> Command or code for sending Temperature data in the form of a String data type to the Firebase Realtime Database.
  if (Firebase.failed()) {
        Serial.print("upload fail :(");
        Serial.println(Firebase.error());  
        delay(500);
        return;
    }

//  Firebase.pushFloat("/database_test/"+ TimeNow +"/DISTANCE", Laser_val);
//  Firebase.pushFloat(firebaseData, "/database_test/DISTANCE", Laser_val);
  delay(2000);
} // loop
