#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define WS_HOST "your-ws-api-gate-way"
#define WS_PORT 443
#define WS_URL "/dev"

#define JSON_DOC_SIZE 2048
#define MSG_SIZE 256

#define NUM_LEDS 200
#define DATA_PIN 4

#define DHTPIN 2 
#define DHTTYPE DHT22 

#define WIFI_LED_PIN 18
#define CONNECTED_LED_PIN 19
#define WS_CONNECTED_LED_PIN 21

WiFiMulti wifiMulti;
WebSocketsClient wsClient;
CRGB leds[NUM_LEDS];
DHT dht(DHTPIN, DHTTYPE);

enum Pattern { SOLID_BLACK, SOLID_RED, SOLID_ORANGE, SOLID_YELLOW, SOLID_GREEN, SOLID_BLUE, SOLID_INDIGO, SOLID_VIOLET, SOLID_BLUE_VIOLET, RAINBOW, BLINK, CHASE, THEATER_CHASE, CUSTOM_CHASE, WARM_WHITE_TO_RED };
Pattern currentPattern = SOLID_BLACK;

unsigned long lastDHTReadTime = 0;
unsigned long lastPatternUpdateTime = 0;
unsigned long patternInterval = 0;
int customChaseStep = 0;
int warmWhiteToRedStep = 0;

void sendErrorMessage(const char *error) {
  char msg[MSG_SIZE];
  sprintf(msg, "{\"action\":\"msg\",\"type\":\"error\",\"body\":\"%s\"}", error);
  wsClient.sendTXT(msg);
}

void sendOkMessage() {
  wsClient.sendTXT("{\"action\":\"msg\",\"type\":\"status\",\"body\":\"ok\"}");
}

uint8_t toMode(const char *val) {
  if (strcmp(val, "output") == 0) {
    return OUTPUT;
  }
  if (strcmp(val, "input_pullup") == 0) {
    return INPUT_PULLUP;
  }
  return INPUT;
}

void setAllBlack() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void setAllRed() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}

void setAllOrange() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::DarkOrange;
  }
  FastLED.show();
}

void setAllYellow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();
}

void setAllGreen() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
}

void setAllBlue() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
}

void setAllIndigo() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(75, 0, 130); 
  }
  FastLED.show();
}

void setAllViolet() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Violet;
  }
  FastLED.show();
}

void setAllBlueViolet() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::BlueViolet;
  }
  FastLED.show();
}

void showRainbow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 256 / NUM_LEDS), 255, 255);
  }
  FastLED.show();
}

void blinkPattern() {
  static bool on = false;
  if (millis() - lastPatternUpdateTime >= 500) {
    lastPatternUpdateTime = millis();
    on = !on;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = on ? CRGB::DarkOrange : CRGB::Black;
    }
    FastLED.show();
  }
}

void chasePattern() {
  static int pos = 0;
  if (millis() - lastPatternUpdateTime >= 50) {
    lastPatternUpdateTime = millis();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
    leds[pos] = CRGB::White;
    pos = (pos + 1) % NUM_LEDS;
    FastLED.show();
  }
}

void lightUpPattern() {
  const int firstArrow[] = {0, 27, 52, 75, 96, 115, 132, 147, 160, 171, 180, 187};
const int secondArrow[] = {3, 30, 55, 78, 99, 118, 135, 150, 163, 174, 182, 188};
//const int thirdArrow[] = {6, 33, 58, 81, 102, 121, 138, 153};
//const int fourthArrow[] = {9, 36, 61, 84, 105};
// const int fifthArrow[] = {12, 39};
// //const int sixthArrow[] = {14};
// const int seventhArrow[] = {17, 42, 65, 86};
// const int eightArrow[] = {20, 45, 68, 89, 108, 125, 140};
 const int ninthArrow[] = {23, 48, 71, 92, 111, 128, 143, 156, 167, 176, 184, 190};
 const int tenthArrow[] = {26, 51, 74, 95, 114, 131, 146, 159, 170, 179, 186, 191};
  static int currentPixel = 0;
  static unsigned long lastUpdateTime = 0;

  // Clear all LEDs initially
  if (currentPixel == 0) {
    FastLED.clear();
  }


  // Check if 30 milliseconds have passed
  if (millis() - lastUpdateTime >= 50) {
    lastUpdateTime = millis();

    // Set the current pixel to white
    leds[firstArrow[currentPixel]] = CRGB::Red;
    leds[secondArrow[currentPixel]] = CRGB::Orange;
    //leds[thirdArrow[currentPixel]] = CRGB::OrangeRed;
    //leds[fourthArrow[currentPixel]] = CRGB::Yellow;
    // leds[fifthArrow[currentPixel]] = CRGB::Green;
    //leds[sixthArrow[currentPixel]] = CRGB::DarkGreen;
    // leds[seventhArrow[currentPixel]] = CRGB::DarkCyan;
    // leds[eightArrow[currentPixel]] = CRGB::DarkBlue;
    leds[ninthArrow[currentPixel]] = CRGB::DarkViolet;
    leds[tenthArrow[currentPixel]] = CRGB::Indigo;

    // Show the updated LED array
    FastLED.show();

    // Move to the next pixel
    currentPixel++;

    // Reset if all pixels are lit
    if (currentPixel >= sizeof(firstArrow) / sizeof(firstArrow[0])) {
      currentPixel = 0;
    }
     if (currentPixel >= sizeof(secondArrow) / sizeof(secondArrow[0])) {
      currentPixel = 0;
    }
    //  if (currentPixel >= sizeof(thirdArrow) / sizeof(thirdArrow[0])) {
    //   currentPixel = 0;
    // }
    // if (currentPixel >= sizeof(fourthArrow) / sizeof(fourthArrow[0])) {
    //   currentPixel = 0;
    //  }
    // if (currentPixel >= sizeof(fifthArrow) / sizeof(fifthArrow[0])) {
    //   currentPixel = 0;
    // }
    // if (currentPixel >= sizeof(sixthArrow) / sizeof(sixthArrow[0])) {
    //   currentPixel = 0;
    // }
    // if (currentPixel >= sizeof(seventhArrow) / sizeof(seventhArrow[0])) {
    //   currentPixel = 0;
    // }
    // if (currentPixel >= sizeof(eightArrow) / sizeof(eightArrow[0])) {
    //   currentPixel = 0;
    // }
    if (currentPixel >= sizeof(ninthArrow) / sizeof(ninthArrow[0])) {
      currentPixel = 0;
    }
    if (currentPixel >= sizeof(tenthArrow) / sizeof(tenthArrow[0])) {
      currentPixel = 0;
    }
  }
}

void lightUpWithTrailAndBurst() {
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Ensure all LEDs start off
  FastLED.show();
  int row_start[] = {0, 27, 52, 75, 96, 115, 132, 147, 160, 171, 180, 192};
  int row_end[] =   {26, 51, 74, 95, 114, 131, 146, 159, 170, 179, 191, 194};
  
  for (int row = 0; row < 12; row++) {
    if (row % 2 == 0) {
      // Left to right
      for (int i = row_start[row]; i <= row_end[row]; i++) {
        leds[i] = CRGB::White;
        if (i > row_start[row]) leds[i - 1] = CRGB::Blue; // Trail effect
        if (i > row_start[row] + 1) leds[i - 2] = CRGB::Black; // Fade out
        FastLED.show();
        delay(10); // Adjust delay for trail speed
      }
    } else {
      // Right to left
      for (int i = row_end[row]; i >= row_start[row]; i--) {
        leds[i] = CRGB::White;
        if (i < row_end[row]) leds[i + 1] = CRGB::Blue; // Trail effect
        if (i < row_end[row] - 1) leds[i + 2] = CRGB::Black; // Fade out
        FastLED.show();
        delay(10); // Adjust delay for trail speed
      }
    }
  }

  fill_solid(leds, NUM_LEDS, CRGB::Red);  // Burst color
  FastLED.show();
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(20); // Adjust delay for fade out speed
  }
}

void sinelon(CRGB *leds) {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(12, 255, 255);
  FastLED.show();
}

void meetInMiddlePattern() {
  static int pos1 = 0;
  static int pos2 = NUM_LEDS - 1;
  static bool burst = false;
  static unsigned long lastPatternUpdateTime = 0;

  if (millis() - lastPatternUpdateTime >= 50) {
    lastPatternUpdateTime = millis();

    if (!burst) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      leds[pos1] = CRGB::Red;
      leds[pos2] = CRGB::Green;

      pos1++;
      pos2--;

      if (pos1 >= pos2) {
        burst = true;
      }
    } else {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((i * 256 / NUM_LEDS + millis() / 10) % 256, 255, 255);
      }
    }

    FastLED.show();
  }
}

void theaterChasePattern() {
  static int offset = 0;
  if (millis() - lastPatternUpdateTime >= 100) {
    lastPatternUpdateTime = millis();
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ((i + offset) % 3 == 0) ? CRGB::White : CRGB::Black;
    }
    offset = (offset + 1) % 3;
    FastLED.show();
  }
}


void customChasePattern() {
  static int numLeds = 27;
  static int increments[] = {25, 23, 21, 19, 17, 15, 13, 11, 9, 7};
  static int numIncrements = sizeof(increments) / sizeof(increments[0]);

  if (millis() - lastPatternUpdateTime >= 300) {
    lastPatternUpdateTime = millis();
    if (warmWhiteToRedStep < numIncrements) {
      for (int i = 0; i < numLeds + increments[warmWhiteToRedStep]; i++) {
        leds[i] = CRGB::Yellow; // Warm white / yellow color
      }
      FastLED.show();
      numLeds += increments[warmWhiteToRedStep];
      warmWhiteToRedStep++;
    } else {
      for (int i = numLeds; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
      }
      FastLED.show();
      warmWhiteToRedStep = 0;
      numLeds = 27;
    }
  }
}

void handleMessage(uint8_t *payload) {
  StaticJsonDocument<JSON_DOC_SIZE> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    sendErrorMessage(error.c_str());
    return;
  }

  if (!doc.containsKey("action") || !doc["action"].is<const char *>()) {
    sendErrorMessage("invalid message action format");
    return;
  }

  if (strcmp(doc["action"], "msg") == 0) {
    if (!doc.containsKey("type") || !doc["type"].is<const char *>()) {
      sendErrorMessage("invalid message type format");
      return;
    }

    if (strcmp(doc["type"], "cmd") == 0) {
      if (!doc.containsKey("body") || !doc["body"].is<JsonObject>()) {
        sendErrorMessage("invalid command body");
        return;
      }

      JsonObject body = doc["body"];

      if (body.containsKey("type") && strcmp(body["type"], "pinMode") == 0) {
        if (!body.containsKey("mode") || !body["mode"].is<const char *>()) {
          sendErrorMessage("invalid pinMode mode type");
          return;
        }

        if (strcmp(body["mode"], "input") != 0 &&
            strcmp(body["mode"], "input_pullup") != 0 &&
            strcmp(body["mode"], "output") != 0) {
          sendErrorMessage("invalid pinMode mode value");
          return;
        }

        pinMode(body["pin"], toMode(body["mode"]));
        sendOkMessage();
        return;
      }

      if (body.containsKey("type") && strcmp(body["type"], "digitalWrite") == 0) {
        digitalWrite(body["pin"], body["value"]);
        sendOkMessage();
        return;
      }

      if (body.containsKey("type") && strcmp(body["type"], "digitalRead") == 0) {
        auto value = digitalRead(body["pin"]);
        char msg[MSG_SIZE];
        sprintf(msg, "{\"action\":\"msg\",\"type\":\"output\",\"body\":%d}", value);
        wsClient.sendTXT(msg);
        return;
      }

      if (body.containsKey("type") && strcmp(body["type"], "solidColor") == 0) {
        if (!body.containsKey("solidColor") || !body["solidColor"].is<const char *>()) {
          sendErrorMessage("invalid solidColor format");
          return;
        }

        const char* color = body["solidColor"];
        if (strcmp(color, "black") == 0) {
          currentPattern = SOLID_BLACK;
        } else if (strcmp(color, "red") == 0) {
          currentPattern = SOLID_RED;
        } else if (strcmp(color, "orange") == 0) {
          currentPattern = SOLID_ORANGE;
        } else if (strcmp(color, "yellow") == 0) {
          currentPattern = SOLID_YELLOW;
        } else if (strcmp(color, "green") == 0) {
          currentPattern = SOLID_GREEN;
        } else if (strcmp(color, "blue") == 0) {
          currentPattern = SOLID_BLUE;
        } else if (strcmp(color, "indigo") == 0) {
          currentPattern = SOLID_INDIGO;
        } else if (strcmp(color, "violet") == 0) {
          currentPattern = SOLID_VIOLET;
        } else if (strcmp(color, "blueViolet") == 0) {
          currentPattern = SOLID_BLUE_VIOLET;
        } else {
          sendErrorMessage("unsupported color");
        }
        sendOkMessage();
        return;
      }

      if (body.containsKey("type") && strcmp(body["type"], "pattern") == 0) {
        if (!body.containsKey("pattern") || !body["pattern"].is<const char *>()) {
          sendErrorMessage("invalid pattern format");
          return;
        }

        const char* pattern = body["pattern"];
        if (strcmp(pattern, "rainbow") == 0) {
          currentPattern = RAINBOW;
        } else if (strcmp(pattern, "blink") == 0) {
          currentPattern = BLINK;
        } else if (strcmp(pattern, "chase") == 0) {
          currentPattern = CHASE;
        } else if (strcmp(pattern, "theaterChase") == 0) {
          currentPattern = THEATER_CHASE;
        } else if (strcmp(pattern, "customChase") == 0) {
          currentPattern = CUSTOM_CHASE;
        } else if (strcmp(pattern, "warmWhiteToRed") == 0) {
          currentPattern = WARM_WHITE_TO_RED;
        } else {
          sendErrorMessage("unsupported pattern");
        }
        sendOkMessage();
        return;
      }

      sendErrorMessage("unsupported command type");
      return;
    }

    sendErrorMessage("unsupported message type");
    return;
  }

  sendErrorMessage("unsupported action");
  return;
}

void onWSEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WS Connected");
      digitalWrite(WS_CONNECTED_LED_PIN, HIGH);
      break;
    case WStype_DISCONNECTED:
      Serial.println("WS Disconnected");
      digitalWrite(WS_CONNECTED_LED_PIN, LOW);
      break;
    case WStype_TEXT:
      Serial.println("Handling message");
      Serial.printf("WS Message: %s\n", payload);
      handleMessage(payload);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(CONNECTED_LED_PIN, OUTPUT);
  pinMode(WS_CONNECTED_LED_PIN, OUTPUT);
  digitalWrite(WIFI_LED_PIN, HIGH);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();

  dht.begin();

  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    digitalWrite(WIFI_LED_PIN, HIGH);
    delay(100);
  }
  digitalWrite(WIFI_LED_PIN, LOW);
  digitalWrite(CONNECTED_LED_PIN, HIGH);

  Serial.println("Connected to WIFI");

  wsClient.beginSSL(WS_HOST, WS_PORT, WS_URL, "", "wss");
  wsClient.onEvent(onWSEvent);
}

void loop() {
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);

  wsClient.loop();

  unsigned long currentTime = millis();

  if (currentTime - lastDHTReadTime >= 5000) {
    lastDHTReadTime = currentTime;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    char msg[MSG_SIZE];
    sprintf(msg, "{\"action\":\"msg\",\"type\":\"sensor\",\"body\":{\"temperature\":%.2f,\"humidity\":%.2f}}", temperature, humidity);
    Serial.println(msg);
    wsClient.sendTXT(msg);
  }

  switch (currentPattern) {
    case SOLID_BLACK:
      setAllBlack();
      break;
    case SOLID_RED:
      setAllRed();
      break;
    case SOLID_ORANGE:
      setAllOrange();
      break;
    case SOLID_YELLOW:
      setAllYellow();
      break;
    case SOLID_GREEN:
      setAllGreen();
      break;
    case SOLID_BLUE:
      setAllBlue();
      break;
    case SOLID_INDIGO:
      setAllIndigo();
      break;
    case SOLID_VIOLET:
      setAllViolet();
      break;
    case SOLID_BLUE_VIOLET:
      setAllBlueViolet();
      break;
    case RAINBOW:
      showRainbow();
      break;
    case BLINK:
      blinkPattern();
      break;
    case CHASE:
      chasePattern();
      break;
    case THEATER_CHASE:
      theaterChasePattern();
      break;
    case CUSTOM_CHASE:
      customChasePattern();
      break;
  }
}