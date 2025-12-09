#ifndef AQUAPONICS_SENSORS_H
#define AQUAPONICS_SENSORS_H

#include <Arduino.h>

// WATER LEVEL SENSOR
class WaterLevelSensor {
  private:
    int pin;

  public:
    WaterLevelSensor(int p);
    int readRaw();
    int readPercentage(); 
    String getStatus(int percentage); 
};

// pH SENSOR
class PhSensor {
  private:
    int _pin;
    float _calibration;

  public:
    PhSensor(int pin, float cal);
    void begin();
    float readVoltage();  
    float readPH();     
    String getStatus(float ph);  
};

// TURBIDITY
class TurbiditySensor {
  private:
    int _pin;
    float _voltage;

  public:
    TurbiditySensor(int pin);
    void begin();
    int getRaw();
    float readVoltage();        
    String getStatus(float voltage); 
};

#endif
