#include <ESP8266WiFi.h>
//PCF8574
#include "Arduino.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x20); 
 
const char* ssid = "iptimea704";
const char* password = "rodemeng2019";
int redPin = P3;
int greenPin = P4;
int bluePin = P5;
 
int ledPin = 14; // GPIO13---D7 of NodeMCU
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  pcf8574.pinMode(redPin, OUTPUT);
    pcf8574.pinMode(greenPin, OUTPUT);   
    pcf8574.pinMode(bluePin, OUTPUT);        
    pcf8574.begin(); //처음에 안써서 실행 안됨(주의)
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\n');
  Serial.println(request);
  client.flush();
 
  // Match the request
  //초기 값
  int Now = LOW;
  if (request.indexOf("/Now=ON") != -1)  {
    setColor(1, 0, 0); //청록
    Now = HIGH;
  }
  if (request.indexOf("/Now=OFF") != -1)  {
    setColor(1, 1, 1); //off
    Now = LOW;
  }
  
  //자동, 수동
  int Auto = LOW;
  if (request.indexOf("/Auto=ON") != -1)  {
    setColor(1, 1, 0); //파
    Auto = HIGH;
  }
  if (request.indexOf("/Auto=OFF") != -1)  {
    setColor(1, 1, 1); //off
    Auto = LOW;
  }

  //현재 값
  int New = LOW;
  if (request.indexOf("/New=ON") != -1)  {
    setColor(1, 0, 0); //청록
    New = HIGH;
  }
  if (request.indexOf("/New=OFF") != -1)  {
    setColor(1, 1, 1); //off
    New = LOW;
  }

  //펌프
  int Pump = LOW;
  if (request.indexOf("/Pump=ON") != -1)  {
    setColor(0, 1, 1); //빨
    Pump = HIGH;
  }
  if (request.indexOf("/Pump=OFF") != -1)  {
    setColor(1, 1, 1); //off
    Pump = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  //초기 값
  client.print("Now Distance is now: ");
  if(Now == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  
  //자동, 수동
  client.println("Auto Mode is now: ");
  if(Auto == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");

  //현재 값
  client.print("New Distance is now: ");
  if(New == HIGH) {
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
  client.println("<a href=\"/Now=ON\"\"><button>On </button></a>");
  client.println("<a href=\"/Now=OFF\"\"><button>Off </button></a><br />");
  client.println("<a href=\"/Auto=ON\"\"><button>On </button></a>");
  client.println("<a href=\"/Auto=OFF\"\"><button>Off </button></a><br />");
  client.println("<a href=\"/New=ON\"\"><button>On </button></a>");
  client.println("<a href=\"/New=OFF\"\"><button>Off </button></a><br />");
  client.println("<a href=\"/Pump=ON\"\"><button>On </button></a>");
  client.println("<a href=\"/Pump=OFF\"\"><button>Off </button></a><br />");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
void setColor(int red, int green, int blue)
{
  pcf8574.digitalWrite(redPin, red);
  pcf8574.digitalWrite(greenPin, green);
  pcf8574.digitalWrite(bluePin, blue); 
}
