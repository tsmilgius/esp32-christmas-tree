#include <FastLED.h>
#include <arduino.h>

#define NUM_LEDS 195
#define DATA_PIN 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 255

CRGB leds[NUM_LEDS];

uint16_t ledsData[NUM_LEDS][4];
uint16_t pick;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

enum Pattern { off_led, christmasSparklesRG_led, christmasSparkles_led, rainbow_led, rainbowWithGlitter_led, confetti_led, sinelon_led, bpm_led, juggle_led };
Pattern currentPattern = off_led;

void christmasSparklesRG() {  // Red and Green only
  //"Background" color for non-sparkling pixels.  Can be set to black for no bg color.
  CRGB sparkleBgColor = CHSV(0, 0, 0);  // black
  //CRGB sparkleBgColor = CHSV(50, 30, 30);  // dim white
  
 
  EVERY_N_MILLISECONDS(40){
    if( random8() < 110 ) {  // How much to sparkle!  Higher number is more.
      pick = random16(NUM_LEDS);
      if (ledsData[pick][3] == 0 ) {
        ledsData[pick][3] = 65;  // Used to tag pixel as sparkling
        uint8_t randomPick = random8(2);
        if (randomPick == 0) {
          ledsData[pick][0] = 16;  // sparkle hue (red)
          ledsData[pick][1] = 253;  // sparkle saturation
          ledsData[pick][2] = 242;  // sparkle value
        }
        if (randomPick == 1) {
          ledsData[pick][0] = 96;   // sparkle hue (green)
          ledsData[pick][1] = 230;  // sparkle saturation
          ledsData[pick][2] = 255;  // sparkle value
        }
        leds[pick] = CHSV(ledsData[pick][0], ledsData[pick][1], ledsData[pick][2]);
      }
    }
    for (uint16_t i=0; i < NUM_LEDS; i++) {
      if (ledsData[i][3] == 0) {  // if not sparkling, set to "back ground" color
        leds[i] = sparkleBgColor;
      } else {
        CHSV hsv = rgb2hsv_approximate(leds[i]);  // Used to get approximate Hue
        EVERY_N_MILLISECONDS(50) { ledsData[i][0] = hsv.hue - 1; }  // slightly shift hue
        ledsData[i][2] = scale8(ledsData[i][2], 253);  // slowly darken
        leds[i] = CHSV(ledsData[i][0], ledsData[i][1], ledsData[i][2]);
        ledsData[i][3] = ledsData[i][3] - 1;  // countdown sparkle tag
      }
    }
  }
 FastLED.show();
}

void christmasSparkles() {
  //"Background" color for non-sparkling pixels.
  //CRGB sparkleBgColor = CHSV(50, 30, 40);  // dim white
  CRGB sparkleBgColor = CHSV(96, 200, 100);  // dim green
  
  EVERY_N_MILLISECONDS(40){
    if( random8() < 60 ) {  // How much to sparkle!  Higher number is more.
      pick = random16(NUM_LEDS);
      if (ledsData[pick][3] == 0 ) {
        ledsData[pick][3] = 35;  // Used to tag pixel as sparkling
        uint8_t randomPick = random8(5);
        if (randomPick == 0) {
          ledsData[pick][0] = 178;   // sparkle hue (blue)
          ledsData[pick][1] = 244;  // sparkle saturation
          ledsData[pick][2] = 210;  // sparkle value
        }
        if (randomPick == 1) {
          ledsData[pick][0] = 10;  // sparkle hue (red)
          ledsData[pick][1] = 255;  // sparkle saturation
          ledsData[pick][2] = 240;  // sparkle value
        }
        if (randomPick == 2) {
          ledsData[pick][0] = 0;  // sparkle hue (white-ish)
          ledsData[pick][1] = 25;  // sparkle saturation
          ledsData[pick][2] = 255;  // sparkle value
        }
        if (randomPick == 3) {
          ledsData[pick][0] = 35;   // sparkle hue (orange)
          ledsData[pick][1] = 235;  // sparkle saturation
          ledsData[pick][2] = 245;  // sparkle value
        }
        if (randomPick == 4) {
          ledsData[pick][0] = 190;  // sparkle hue (purple)
          ledsData[pick][1] = 255;  // sparkle saturation
          ledsData[pick][2] = 238;  // sparkle value
        }
        leds[pick] = CHSV(ledsData[pick][0], ledsData[pick][1], ledsData[pick][2]);
      }
    }
    for (uint16_t i=0; i < NUM_LEDS; i++) {
      if (ledsData[i][3] == 0) {  // if not sparkling, set to "back ground" color
        leds[i] = sparkleBgColor;
      } else {
        CHSV hsv = rgb2hsv_approximate(leds[i]);  // Used to get approximate Hue
        EVERY_N_MILLISECONDS(38) { ledsData[i][0] = hsv.hue - 1; }  // slightly shift hue
        ledsData[i][2] = scale8(ledsData[i][2], 245);  // slowly darken
        leds[i] = CHSV(ledsData[i][0], ledsData[i][1], ledsData[i][2]);
        ledsData[i][3] = ledsData[i][3] - 1;  // countdown sparkle tag
      }
    }
  }
   FastLED.show();
}

void off() {
   FastLED.clear();
   FastLED.show();
}

void setSolidColor(const char* color) {
   static int rgb[3];
   sscanf(color, "RGB(%d, %d, %d)", &rgb[0], &rgb[1], &rgb[2]);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(rgb[0], rgb[1], rgb[2]); 
  }
  FastLED.show();
}

void rainbow() {
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
}

void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
  FastLED.show();
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  FastLED.show();
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  FastLED.show();
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue + random8(255), 255, 192);
  FastLED.show();
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1, 0, 5000 )] |= CHSV(dothue, 200, 255); // Increase the period to slow down
    dothue += 32;
  }
  FastLED.show();
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = LavaColors_p; //ForestColors_p, OceanColors_p, HeatColors_p, LavaColors_p, CloudColors_p, PartyColors_p, RainbowColors_p, RainbowStripeColors_p, OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, PartyColors_p, HeatColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  FastLED.show();
}

void initFX() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}

void setBrightness(int brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
}