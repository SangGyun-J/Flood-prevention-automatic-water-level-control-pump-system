//---------------------------------------------------------------------------------Send DHT11 Sensor Data with Time from the internet to the Firebase Relatime Database
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "" //--> URL address of your Firebase Realtime Database.
#define FIREBASE_AUTH "" //--> Your firebase database secret code.

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

#define DHTTYPE DHT11 //--> Defines the type of DHT sensor used (DHT11, DHT21, and DHT22), in this project the sensor used is DHT11.

//----------------------------------------SSID and Password of your WiFi router.
const char* ssid = ""; //--> Your wifi name or SSID.
const char* password = ""; //--> Your wifi password.
//----------------------------------------

const int DHTPin = 5; //--> The pin used for the DHT11 sensor is Pin D1 = GPIO5
DHT dht(DHTPin, DHTTYPE); //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);

//----------------------------------------Adjust the UTC
// You need to adjust the UTC offset for your timezone in milliseconds. 
// See the UTC time offset list here : https://en.wikipedia.org/wiki/List_of_UTC_time_offsets
// Here are some examples for different timezones :
// > For UTC -5.00 : -5 * 60 * 60 : -18000
// > For UTC +1.00 : 1 * 60 * 60 : 3600
// > For UTC +0.00 : 0 * 60 * 60 : 0

// This is my UTC adjustment calculation : 
// > UTC in my area : UTC +07:00
// > UTC +07:00 -> 7 * 60 * 60 = 25200
// > UTC +09:00(한국) -> 9 * 60 * 60 = 32400
const long utcOffsetInSeconds = 32400;
//----------------------------------------

//----------------------------------------Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
//----------------------------------------

String DBnm = "DHT11SensorDatawithTime";
String TD = "Temperature";
String HD = "Humidity";

int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

  dht.begin();  //--> Start reading DHT11 sensors
  delay(500);
  
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  //----------------------------------------
  timeClient.begin();
  //----------------------------------------

  //----------------------------------------Firebase Realtime Database Configuration.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //----------------------------------------
}

void loop() {
  // put your main code here, to run repeatedly:

  //----------------------------------------Here sending data to the Firebase Realtime Database is limited to 5 times sending.
  // Remove this condition if you want to send data continuously.
  if (count < 5) {
    count++;

    //----------------------------------------Get time from the internet and format the display.
    // Without the conditions below, the time display will be like this: 1:1:1, 1:50:5
    // With the conditions below, the time display will be like this: 01:01:01, 01:50:05
    timeClient.update();
    String hr, mn, sc;
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
    Serial.print(TimeNow);
    //----------------------------------------ㄹ

    //----------------------------------------Reading Temperature and Humidity
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    int h = dht.readHumidity(); //--> Read humidity.
    float t = dht.readTemperature(); //--> Read temperature as Celsius (the default). 
    // float tf = dht.readTemperature(true); //--> // Read temperature as Fahrenheit (isFahrenheit = true)
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(" Failed to load DHT sensor !");
      delay(1000);
      return;
    }
    //----------------------------------------
  
    Serial.print(F(", Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
  
    String strHum = String(h); //--> Convert Humidity value to String data type.
    String strTem = String(t); //--> Convert Temperature values to the String data type.
    
String DBnm = "DHT11SensorDatawithTime";
String TD = "Temperature";
String HD = "Humidity";

    //----------------------------------------Send Humidity data to the Firebase Realtime Database.
    String DBaddH = DBnm + "/" + TimeNow + "/" + HD; //--> Creating a Database path
    Firebase.setString(DBaddH,strHum); //--> Command or code for sending Humidity data in the form of a String data type to the Firebase Realtime Database.

    // Conditions for handling errors.
    if (Firebase.failed()) {
        Serial.print("setting Humidity failed :");
        Serial.println(Firebase.error());  
        delay(500);
        return;
    }
    //----------------------------------------

    //----------------------------------------Send Temperature data to the Firebase Realtime Database.
    String DBaddT = DBnm + "/" + TimeNow + "/" + TD; //--> Creating a Database path
    Firebase.setString(DBaddT,strTem); //--> Command or code for sending Temperature data in the form of a String data type to the Firebase Realtime Database.

    // Conditions for handling errors.
    if (Firebase.failed()) {
        Serial.print("setting Temperature failed :");
        Serial.println(Firebase.error());  
        delay(500);
        return;
    }
    //----------------------------------------
    
    Serial.println("Setting successful");
    Serial.println();
    
    delay(5000);
  }
  //----------------------------------------
}
