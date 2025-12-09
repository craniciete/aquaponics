#include "sensors.h"

// WATER LEVEL
WaterLevelSensor::WaterLevelSensor(int p){ pin = p; }

int WaterLevelSensor::readRaw(){ 
    return analogRead(pin); 
}

int WaterLevelSensor::readPercentage(){ 
    long sum = 0;
    const int N = 10;
    for(int i=0;i<N;i++){
        sum += analogRead(pin);
        delay(5);
    }
    int avg = sum / N;
    return map(avg, 0, 4095, 0, 100);
}

String WaterLevelSensor::getStatus(int percentage){
    if(percentage < 30) return "Low";
    if(percentage < 70) return "Normal";
    return "High";
}

// pH SENSOR
PhSensor::PhSensor(int pin, float cal){ 
    _pin = pin; 
    _calibration = cal; 
}

void PhSensor::begin(){ 
    pinMode(_pin, INPUT); 
}

float PhSensor::readVoltage(){
    float sum = 0;
    const int N = 10;
    for(int i=0;i<N;i++){
        sum += analogRead(_pin) * (3.3 / 4095.0); 
        delay(5);
    }
    return sum / N;
}

float PhSensor::readPH(){
    float voltage = readVoltage();
    return -5.70 * voltage + _calibration;
}

String PhSensor::getStatus(float ph){
    if(ph < 6.5) return "Acidic";
    if(ph <= 7.5) return "Neutral";
    return "Alkaline";
}

// TURBIDITY SENSOR
TurbiditySensor::TurbiditySensor(int pin){ _pin = pin; }

void TurbiditySensor::begin(){ pinMode(_pin, INPUT); }

int TurbiditySensor::getRaw(){ 
    return analogRead(_pin); 
}

float TurbiditySensor::readVoltage(){
    float sum = 0;
    const int N = 10;
    for(int i=0;i<N;i++){
        sum += analogRead(_pin) * (3.3 / 4095.0);
        delay(5);
    }
    _voltage = sum / N;
    return _voltage;
}

String TurbiditySensor::getStatus(float v){
    if(v > 2.5) return "Clear";
    if(v > 1.5) return "Dirty";
    return "Cloudy";
}
