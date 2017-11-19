#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

void setup() {
  Serial.begin(115200);
  while(! Serial);
  //Serial.setDebugOutput(true);
  if (! lis.begin(0x18)) {
    Serial.println("Couldn't start LIS3DH.");
    while (1);
  }
  Serial.println("LIS3DH started.");
  lis.setRange(LIS3DH_RANGE_2_G);
  lis.setClick(1, LIS_THRESH);

  pinMode(VIBE, INPUT_PULLUP); //setup pin for the vibration sensor

  DrumTime->onMessage("freetime", freetime);//listen to toggle accounting
  DrumTime->onMessage("balance", balance_sub);//listen to adjust outside of on-board accounting
  
  Serial.print("Connecting to DrumTime AIO");
  pixels.begin();
  pixels.setBrightness(25);
  pixels.show();
  io.connect();

  uint8_t p = 1;
  while(io.status() < AIO_CONNECTED) {
    if (io.status() == AIO_DISCONNECTED) {break;}
    Serial.print(".");
    pixOff();
    pixSingle_blu(p);
    if (p != 6) {
      p++;
    } else {
      p = 1;
    }
    delay(200);
  }
  pixOff();
  if (io.status() == AIO_CONNECTED) {
    pixSingle_blu(0);
  } else {
    pixSingle_red(0);
  }
  Serial.println();
  Serial.println(io.statusText());

  //Get current freetime and balance
  if (io.status() == AIO_CONNECTED) {
    Serial.println("Getting current balance...");
    int result = queryFeed(feedBuffer, "drumtime.balance");
    if(result == -1) {
      Serial.print(F("Balance failed: "));
      Serial.println(String(result));      
    } else {
      balance_start = atol(feedBuffer);
      Serial.print(F("Balance is: "));
      Serial.println((balance_start));
    }
    Serial.println("Getting freetime status...");
    result = queryFeed(feedBuffer, "drumtime.freetime");
    if(result == -1) {
      Serial.print(F("FreeTime failed: "));
      Serial.println(String(result));      
    } else {
      freetime_start = feedBuffer;
      Serial.print(F("FreeTime is: "));
      Serial.println(freetime_start);
    }
  } else {
    Serial.println("AIO not connected. Retained message retrieval failed.");
  }
  iorunMillis = millis();
  telemMillis = millis();
  pubMillis = millis();
  preMillis = millis();
  minuteMillis = millis();
}
