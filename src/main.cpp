#include <Arduino.h>
#include <WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "secrets.h" // create this file with your wifi credentials and websocket server details
#include "oled.h"
#include "fx.h"

#define MSG_SIZE 256

WiFiMulti wifiMulti;
WebSocketsClient wsClient;

void initWIFI();
bool isWiFiConnected();
void checkWiFiConnection();
void onWSEvent(WStype_t type, uint8_t *payload, size_t length);
unsigned long previousMillis = 0;
const long interval = 5000;

void setPattern(const char* pattern) {

  if (strcmp(pattern, "off") == 0) {
    currentPattern = off_led;
  } else if (strcmp(pattern, "christmasSparklesRG") == 0) {
    currentPattern = christmasSparklesRG_led;
  } else if(strcmp(pattern, "christmasSparkles") == 0) {
    currentPattern = christmasSparkles_led;
  } else if(strcmp(pattern, "rainbow") == 0) {
    currentPattern = rainbow_led;
  } else if(strcmp(pattern, "rainbowWithGlitter") == 0) {
    currentPattern = rainbowWithGlitter_led;
  } else if(strcmp(pattern, "confetti") == 0) {
    currentPattern = confetti_led;
  } else if(strcmp(pattern, "sinelon") == 0) {
    currentPattern = sinelon_led;
  } else if(strcmp(pattern, "bpm") == 0) {
    currentPattern = bpm_led;
  } else if(strcmp(pattern, "juggle") == 0) {
    currentPattern = juggle_led;
  } 
}

void setup() {
  Serial.begin(115200);
  initOLED();
  initWIFI();
  delay(1000);
  wsClient.beginSSL(WS_HOST, WS_PORT, WS_URL, "", "wss");
  wsClient.onEvent(onWSEvent);
  initFX();
}

void loop() {
  checkWiFiConnection();
  wsClient.loop();

 

  //enum Pattern { off_led, christmasSparklesRG_led, christmasSparkles_led, rainbow_led, rainbowWithGlitter_led, confetti_led, sinelon_led, bpm_led, juggle_led };

   switch (currentPattern) {
    case off_led:
      off();
      break;
    case christmasSparklesRG_led:
      christmasSparklesRG();
      break;
    case christmasSparkles_led:
      christmasSparkles();
      break;
    case rainbow_led:
      rainbow();
      break;
    case rainbowWithGlitter_led:
      rainbowWithGlitter();
      break;
    case confetti_led:
      confetti();
      break;
    case sinelon_led:
      sinelon();
      break;
    case bpm_led:
      bpm();
      break;
    case juggle_led:
      juggle();
      break;
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void checkWiFiConnection() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    initWIFI();
    displayWifiAndReconnect(isWiFiConnected(), WiFi.localIP(), wsClient.isConnected() ? "WS OK!" : "WS Connecting...");
  }
}

void initWIFI() {
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttemptTime = millis();
  while (wifiMulti.run() != WL_CONNECTED && !isWiFiConnected()) {
    if (millis() - startAttemptTime >= 10000) { // 10 seconds timeout
      Serial.println("Failed to connect to WiFi");
      displayWiFiError();
      break;
    }
    displayWiFiError();
    delay(100);
  }
  displayWiFiInfo(WiFi.localIP(), wsClient.isConnected() ? "WS OK!" : "WS Connecting...");
}


void sendErrorMessage(const char *error) {
  char msg[MSG_SIZE];
  sprintf(msg, "{\"action\":\"msg\",\"type\":\"error\",\"body\":\"%s\"}", error);
  wsClient.sendTXT(msg);
}

void sendOkMessage() {
  wsClient.sendTXT("{\"action\":\"msg\",\"type\":\"status\",\"body\":\"ok\"}");
}

void handleMessage(uint8_t *payload) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, (const char*)payload);
 
//start ws message validation
  if (error) {
    sendErrorMessage(error.c_str());
    return;
  }

  if (!doc["action"].is<const char *>()) {
    sendErrorMessage("invalid message action format");
    return;
  }

  if (strcmp(doc["action"], "msg") == 0) {
    if (!doc["type"].is<const char *>()) {
      sendErrorMessage("invalid message type format");
      return;
    }
  }

    if (strcmp(doc["type"], "cmd") != 0 && strcmp(doc["type"], "info") != 0) {
      sendErrorMessage("invalid message type");
      return;
    }

  if (strcmp(doc["type"], "cmd") == 0) {
    if (!doc["body"].is<JsonObject>()) {
      sendErrorMessage("invalid command body");
      return;
    }  
  }
//end ws message validation

  JsonObject body = doc["body"];

  if(body["color"].is<const char *>()) {
    setSolidColor(body["color"]);
  } 
  if(body["effect"].is<const char *>()) {
    setPattern(body["effect"].as<const char*>());
  }
  if(body["brightness"].is<int>()) {
    setBrightness(body["brightness"]);
  }
  sendOkMessage();

}

void onWSEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WS Connected");
      break;
    case WStype_DISCONNECTED:
      Serial.println("WS Disconnected");
      break;
    case WStype_TEXT:
      Serial.printf("WS Message: %s\n", payload);
      handleMessage(payload);
      break;
  }
}
