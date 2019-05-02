/*
   Panasonic PIR
   Uses IFTTT webhook + twitter to tweet a status containing detection and time
   Uses Panasonic industrial grade PIR sensors and ESP8266-12E
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

int inputPin = A0;               // choose the input pin (from PIR sensor)
int pirState = false;            // we start, assuming no motion detected
int val = 0;                     // volt read
int volt_threshold = 500;

void setup() {
    pinMode(inputPin, INPUT);     // declare sensor as input
    Serial.begin(115200);
    Serial.println("start");
    WiFi.hostname("largecheesepizza"); // name network device
    WiFi.begin("router", "password"); // psk acceptable for passphrase
    WiFi.mode(WIFI_STA);
    delay(3000);
}

void loop() {
    val = analogRead(inputPin); // 1024 if triggered, otherwise hovers around 15  
    if (val > volt_threshold) {            // check if the input is above threshold
        if (!pirState) {
            // we have just turned on
            Serial.println("Motion detected!");
            while(!pirState){
               if ((WiFi.status() == WL_CONNECTED)) {
                   String finaltrigger = "/trigger/{event_name}/with/key/{maker_key}?value1=" + String(random(1000)); // randomint avoids duplicate status error
                   USE_SERIAL.println(WiFi.localIP());
                   HTTPClient http;
                   USE_SERIAL.print("Transmitting...\n");
                   http.begin("maker.ifttt.com", 80, finaltrigger); //HTTP    
                   int httpCode = http.GET(); // use return code if want to check response
                   http.end();
                   USE_SERIAL.println("Transmission completed.");
                   pirState = true; // blocks re-entry after successful transmission, and while continously true for motion detected
               }
               delay(100);
            }
            delay(2000);
        }    
    } 
    else {
        if (pirState) {
            // we have just turned of
            Serial.println("Motion ended!");
            pirState = false;
            delay(3000);
        }
    }  
}
