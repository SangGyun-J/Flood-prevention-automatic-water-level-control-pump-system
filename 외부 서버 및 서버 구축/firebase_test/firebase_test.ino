#include <FirebaseESP8266.h>
#include  <ESP8266WiFi.h>

#include <Wire.h>
#include <VL53L1X.h>
VL53L1X sensor;

#define FIREBASE_HOST "esp8266-sensor-readings-23fd0-default-rtdb.firebaseio.com"
//#define WIFI_SSID "iptimea704" // Change the name of your WIFI
//#define WIFI_PASSWORD "rodemeng2019" // Change the password of your WIFI
#define WIFI_SSID "우옹" // Change the name of your WIFI
#define WIFI_PASSWORD "12340000" // Change the password of your WIFI
#define FIREBASE_Authorization_key "GzVRIYoR2qjjvtMmrpS2Z4Esl089LWlCxYxcQRFv"

FirebaseData firebaseData;
FirebaseJson json;
void setup() {
////////////////////레이저 센서//////////////////// 
 Serial.begin(115200);
 Wire.begin();
// Wire.setClock(400000); // use 400 kHz I2C
// 
// sensor.setTimeout(500);
//  if (!sensor.init())
//  {
//    Serial.println("Failed to detect and initialize sensor!");
//    while (1);
//  }
// sensor.setDistanceMode(VL53L1X::Long);
// sensor.setMeasurementTimingBudget(50000);
//  
// sensor.startContinuous(50);// 
////////////////////레이저 센서//////////////////// 
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

void loop() {
  float Laser_val = sensor.read(); //레이저센서값 변수에 저장
  
  Serial.print ("Distance(mm) : ");
  Serial.print(sensor.read());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();

  Firebase.setFloat(firebaseData, "/database_test/DISTANCE", Laser_val);
  delay(1000);
}
