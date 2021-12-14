//펌프 제어
const int pump = 14;

void setup() {
    pinMode(pump, OUTPUT);
}

void loop() {    
  digitalWrite(pump, HIGH);
  delay(1000);
  digitalWrite(pump, LOW);
  delay(1000);
}
