#include <WiFi.h>
#include <HTTPClient.h>
#include "sensors.h"
#include "time.h"

// FIREBASE URL
String fbCurrent = "https://aquaponics-48aea-default-rtdb.asia-southeast1.firebasedatabase.app/aquaponics/current.json";
String fbLogs    = "https://aquaponics-48aea-default-rtdb.asia-southeast1.firebasedatabase.app/aquaponics/logs.json";
String fbMode    = "https://aquaponics-48aea-default-rtdb.asia-southeast1.firebasedatabase.app/aquaponics/mode.json";

// WIFI CREDENTIAL
const char* ssid = "Carlo_Plus";
const char* password = "12345678911";

// SENSORS
WaterLevelSensor water(34);
PhSensor ph(35, 21.34);
TurbiditySensor turb(32);

// TIMING
unsigned long lastSendCurrent = 0;
unsigned long lastSendLog     = 0;

const unsigned long intervalCurrent = 1000;   // 1 second
const unsigned long intervalLog     = 600000; // 10 minutes

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8 * 3600;
const int   daylightOffset_sec = 0;

// SYSTEM MODE-
enum SystemMode {AUTO, MAINTENANCE, SLEEP};
SystemMode currentMode = AUTO;

void setup() {
    Serial.begin(115200);

    ph.begin();
    turb.begin();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    WiFi.setSleep(true);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Waiting for NTP time...");
    while(time(nullptr) < 1609459200){
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nNTP time initialized!");
}

void updateMode() {
    if(WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(fbMode);
    int httpCode = http.GET();
    if(httpCode == 200) {
        String payload = http.getString();
        payload.trim();
        if(payload == "\"AUTO\"") currentMode = AUTO;
        else if(payload == "\"MAINTENANCE\"") currentMode = MAINTENANCE;
        else if(payload == "\"SLEEP\"") currentMode = SLEEP;
    }
    http.end();
}

// REALTIME DATA
void sendCurrent(int wl, float phVal, float turbVolt, String wlStatus, String phStatus, String turbStatus, time_t ts){
    String json = "{";
    json += "\"water\":" + String(wl) + ",";
    json += "\"water_status\":\"" + wlStatus + "\",";
    json += "\"ph\":" + String(phVal,2) + ",";
    json += "\"ph_status\":\"" + phStatus + "\",";
    json += "\"turbidity\":" + String(turbVolt,2) + ",";
    json += "\"turb_status\":\"" + turbStatus + "\",";
    json += "\"timestamp\":" + String((long)ts);
    json += "}";

    HTTPClient http;
    http.begin(fbCurrent);
    http.addHeader("Content-Type","application/json");
    int code = http.PUT(json);
    if(code > 0) Serial.println("Current updated successfully");
    else Serial.println("Error updating current");
    http.end();
}

// 10 MINUTES DATA
void sendLog(int wl, float phVal, float turbVolt, String wlStatus, String phStatus, String turbStatus, time_t ts){
    String json = "{";
    json += "\"water\":" + String(wl) + ",";
    json += "\"water_status\":\"" + wlStatus + "\",";
    json += "\"ph\":" + String(phVal,2) + ",";
    json += "\"ph_status\":\"" + phStatus + "\",";
    json += "\"turbidity\":" + String(turbVolt,2) + ",";
    json += "\"turb_status\":\"" + turbStatus + "\",";
    json += "\"timestamp\":" + String((long)ts);
    json += "}";

    HTTPClient http;
    http.begin(fbLogs);
    http.addHeader("Content-Type","application/json");
    int code = http.POST(json);
    if(code > 0) Serial.println("Log sent successfully");
    else Serial.println("Error sending log");
    http.end();
}

void loop() {
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi disconnected. Reconnecting...");
        WiFi.reconnect();
        delay(1000);
        return;
    }

    updateMode();

    int wl = water.readPercentage();
    float phVal = ph.readPH();
    float turbVolt = turb.readVoltage();

    String wlStatus   = water.getStatus(wl);
    String phStatus   = ph.getStatus(phVal);
    String turbStatus = turb.getStatus(turbVolt);

    time_t ts = time(nullptr);
    unsigned long now = millis();

    // AUTO
    if(currentMode == AUTO){
        if(now - lastSendCurrent >= intervalCurrent){
            lastSendCurrent = now;
            sendCurrent(wl, phVal, turbVolt, wlStatus, phStatus, turbStatus, ts);
            Serial.printf("Water:%d%%, pH:%.2f, Turb:%.2f\n", wl, phVal, turbVolt);
        }
        if(now - lastSendLog >= intervalLog){
            lastSendLog = now;
            sendLog(wl, phVal, turbVolt, wlStatus, phStatus, turbStatus, ts);
        }
    }

    // MAINTENANCE
    else if(currentMode == MAINTENANCE){
        // manual control
    }

    // SLEEP
    else if(currentMode == SLEEP){
        if(now - lastSendLog >= intervalLog){
            lastSendLog = now;
            sendLog(wl, phVal, turbVolt, wlStatus, phStatus, turbStatus, ts);
        }
    }

    delay(20);
}
