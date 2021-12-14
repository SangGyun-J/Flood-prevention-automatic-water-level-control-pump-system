#include <VL53L1X.h> //Download this library from https://github.com/pololu/vl53l1x-arduino
#include <Wire.h>
#include "PCF8574.h"
PCF8574 pcf21(0x21);
 
VL53L1X sensor_A; //Create the sensor object
VL53L1X sensor_B; //Create the sensor object
VL53L1X sensor_C; //Create the sensor object
VL53L1X sensor_D; //Create the sensor object
VL53L1X sensor_E; //Create the sensor object
 
int startTime = millis(); //used for our timing loop
int mInterval = 100; //refresh rate of 10hz
 
#define XSHUT_A P0 //pcf21
#define XSHUT_B P1 //pcf21
#define XSHUT_C P2 //pcf21
#define XSHUT_D P3 //pcf21
#define XSHUT_E P4 //pcf21
 
#define M_INTERVAL 50
 
void setup() {
 
    Serial.begin(115200);
    
    Wire.begin(); //Setup your I2C interface
    Wire.setClock(400000); // use 400 kHz I2C
 
    //Set the pin mode to output
    pcf21.pinMode(XSHUT_A ,OUTPUT);
    pcf21.pinMode(XSHUT_B ,OUTPUT);
    pcf21.pinMode(XSHUT_C ,OUTPUT);
    pcf21.pinMode(XSHUT_D ,OUTPUT);
    pcf21.pinMode(XSHUT_E ,OUTPUT);
    pcf21.begin();
    
    //Turn all TOF's off
    pcf21.digitalWrite(XSHUT_A, LOW);
    pcf21.digitalWrite(XSHUT_B, LOW);
    pcf21.digitalWrite(XSHUT_C, LOW);
    pcf21.digitalWrite(XSHUT_D, LOW);
    pcf21.digitalWrite(XSHUT_E, LOW);

//     for (int s=0; s<5; s++){
//     delay(50);
//      pcf21.digitalWrite(XSHUT_laserStr[s], HIGH); //Turn sensor_A on
//      delay(50);
//
//      sensor_laserStr[s].setTimeout(500); //Set the sensors timeout
//    
//      if (!sensor_laserStr[s].init())//try to initilise the sensor
//      {
//          //Sensor does not respond within the timeout time
//          Serial.println("Sensor_laserStr[s] is not responding, check your wiring");
//      }
//      else
//      {
//          sensor_laserStr[s].setAddress(laserNum[s]); //Set the sensors I2C address
//          sensor_laserStr[s].setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
//          sensor_laserStr[s].setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
//          sensor_laserStr[s].startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
//      }   
//    }
 
    //-----------------------------------------------------------------
    //FIRST WE WILL CONFIGURE AND SETUP SENSOR_A
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_A, HIGH); //Turn sensor_A on
    delay(50);
    
    sensor_A.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_A.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_A is not responding, check your wiring");
    }
    else
    {
        sensor_A.setAddress(42); //Set the sensors I2C address
        sensor_A.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_A.setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_A.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }   
 
    //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_B
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_B, HIGH); //Turn sensor_B on
    delay(50);
    
    sensor_B.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_B.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_B is not responding, check your wiring");
    }
    else
    {
        sensor_B.setAddress(43); //Set the sensors I2C address
        sensor_B.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_B.setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_B.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }
 
      //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_C
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_C, HIGH); //Turn sensor_C on
    delay(50);
    
    sensor_C.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_C.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_C is not responding, check your wiring");
    }
    else
    {
        sensor_C.setAddress(44); //Set the sensors I2C address
        sensor_C.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_C.setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_C.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    } 
 
    //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_D
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_D, HIGH); //Turn sensor_D on
    delay(50);
    
    sensor_D.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_D.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_D is not responding, check your wiring");
    }
    else
    {
        sensor_D.setAddress(45); //Set the sensors I2C address
        sensor_D.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_D.setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_D.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }

     //-----------------------------------------------------------------
    //NOW CONFIGURE AND SETUP SENSOR_E
    //-----------------------------------------------------------------
    delay(50);
    digitalWrite(XSHUT_E, HIGH); //Turn sensor_E on
    delay(50);
    
    sensor_E.setTimeout(500); //Set the sensors timeout
    
    if (!sensor_E.init())//try to initilise the sensor
    {
        //Sensor does not respond within the timeout time
        Serial.println("Sensor_E is not responding, check your wiring");
    }
    else
    {
        sensor_E.setAddress(46); //Set the sensors I2C address
        sensor_E.setDistanceMode(VL53L1X::Long); //Set the sensor to maximum range of 4 meters
        sensor_E.setMeasurementTimingBudget(40000); //Set its timing budget in microseconds longer timing budgets will give more accurate measurements
        sensor_E.startContinuous(M_INTERVAL); //Sets the interval where a measurement can be requested in milliseconds
    }
}
 
char buf[40];
 
void loop() {
    //We have to be careful here. If we request a measurement before the measurement has been taken your
    //code will be blovked until the measurement is complete. In order to stop this from happening we
    //must ensure that time between measurement requests is greater than the timing budget and the argument 
    //given in the startContinuous() function. In our case our measurement time must be greater than 50mS.
 
    if((millis()- startTime) > mInterval)
    {
        sprintf(buf, "%4d,%4d,%4d",sensor_A.read(),sensor_B.read(),sensor_C.read());
        Serial.println(buf); //Get a reading in millimeters
        
        int laser1 = sensor_A.read();
        int laser2 = sensor_B.read();
        int laser3 = sensor_C.read();
        int laser4 = sensor_D.read();
        int laser5 = sensor_E.read();
        Serial.print("laser1 : ");
        Serial.println(laser1);
        Serial.print("laser2 : ");
        Serial.println(laser2);
        Serial.print("laser3 : ");
        Serial.println(laser3);
        Serial.print("laser4: ");
        Serial.println(laser4);
        Serial.print("laser5 : ");
        Serial.println(laser5);

        //평균
        int avg = (laser1 + laser2 + laser3 + laser4 + laser5)/5;
        Serial.print("평균 : ");
        Serial.println(avg);

        //분산
        int var = (pow((laser1-avg),2) + pow((laser2-avg),2) + pow((laser3-avg),2) + pow((laser4-avg),2) + pow((laser5-avg),2))/5;
        
        //표준편차
        int StandDev = sqrt(var);
        Serial.print("표준편차 : ");
        Serial.println(StandDev);
        
        startTime = millis();
    }
}
