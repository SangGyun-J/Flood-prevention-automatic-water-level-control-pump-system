#include <ESP8266WiFi.h>
#include "time.h"

const char* ssid       = "316ho jumin";   //Replace with your own SSID
const char* password   = "12340000";  //Replace with your own password

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 32400;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)


void setup() {
 Serial.begin(115200);


  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("CONNECTED to WIFI");
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  delay(1000);
  printLocalTime();
}

void printLocalTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  Serial.println(asctime(timeinfo));
  delay(1000);
}
