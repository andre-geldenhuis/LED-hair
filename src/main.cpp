#include "FastLED.h"

FASTLED_USING_NAMESPACE


#define NUM_STRIPS 6
#define NUM_LEDS_PER_STRIP 37
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS

CRGBArray<NUM_LEDS> leds;

CRGBSet leds_1(leds, 0,                                                                    NUM_LEDS_PER_STRIP);
CRGBSet leds_2(leds, NUM_LEDS_PER_STRIP,                                                   NUM_LEDS_PER_STRIP);
CRGBSet leds_3(leds, NUM_LEDS_PER_STRIP*2,                                                 NUM_LEDS_PER_STRIP);
CRGBSet leds_4(leds, NUM_LEDS_PER_STRIP*3,                                                 NUM_LEDS_PER_STRIP);
CRGBSet leds_5(leds, NUM_LEDS_PER_STRIP*4,                                                 NUM_LEDS_PER_STRIP);
CRGBSet leds_6(leds, NUM_LEDS_PER_STRIP*5,                                                 NUM_LEDS_PER_STRIP);

#define BRIGHTNESS          15
#define FRAMES_PER_SECOND  120

//Forward Declare Functions
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void myconfetti(struct CRGB * pFirstLED);
void confetti();
void rainbowrain();

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//period between trail movement for rainbow_rain, milliseconds - for framerate locking
unsigned long p = 40;

byte prob=5; //probablility in percent of raindrop, 5 is 50%

void my_fill_rainbow( struct CRGB * pFirstLED, int numToFill,
                  uint8_t initialhue,
                  uint8_t deltahue,
                bool dir=0 )
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    if(dir==1){
      for( int i = 0; i < numToFill; i++) {
          pFirstLED[i] = hsv;
          hsv.hue += deltahue;
      }
    }else{
      for( int i = numToFill-1; i >= 0; i--) {
          pFirstLED[i] = hsv;
          hsv.hue += deltahue;
      }
    }
}

class RainbowRain
{
    //class members ini at startup of class
    unsigned long previous_millis = millis();
    uint8_t rainpos;
    bool running = false;

    struct CRGB * pleds; 
    uint8_t num_per_strip;

    //constructor
  public:
    RainbowRain(struct CRGB * ipleds, uint8_t inum_per_strip) {
      pleds=ipleds;
      num_per_strip=inum_per_strip;
    }

    void start(){
      running = true;
    }

    void rain(bool dir=0) {
      if (running){
        if (millis() - previous_millis >= p) {
          previous_millis = millis();
          if (dir == 0){
            rainpos++;
            if (rainpos >= num_per_strip) {
              rainpos = 0;
              running = false;
            }
          }
          else{
            rainpos--;
            if (rainpos >= num_per_strip) { //rainpos <= 0 # when rainpos goes -1 it wraps, not checking this lets us address the 0th led
              rainpos = num_per_strip-1;
              running = false;
            }
          }
        pleds[rainpos] += CHSV( gHue, 255, 192);  
      }
      else{
        // Do nothing as we are not running
      }
    }
  }
};

//Instantiate rainbow rain for each strip
RainbowRain rain1(leds_1, NUM_LEDS_PER_STRIP);
RainbowRain rain2(leds_2, NUM_LEDS_PER_STRIP);
RainbowRain rain3(leds_3, NUM_LEDS_PER_STRIP);
RainbowRain rain4(leds_4, NUM_LEDS_PER_STRIP);
RainbowRain rain5(leds_5, NUM_LEDS_PER_STRIP);
RainbowRain rain6(leds_6, NUM_LEDS_PER_STRIP);


void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED there's 60 NEOPIXEL leds on pin 3, starting at index 0 in the led array
  FastLED.addLeds<NEOPIXEL, 15>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 11, starting at index 60 in the led array
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 5, starting at index 120 in the led array
  FastLED.addLeds<NEOPIXEL, 0>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 12>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 14>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 27>(leds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  rain1.start();
  rain2.start();
  rain3.start();
  rain4.start();
  rain5.start();
  rain6.start();
  // rain7.start();
  // rain8.start();
}
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = {rainbowrain};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current


void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+1; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 60 ) { nextPattern(); } // change patterns periodically
  EVERY_N_MILLISECONDS( 800 ) { 
    if (random(10)>prob){
      rain1.start();
    }
  }
   EVERY_N_MILLISECONDS( 1200 ) { 
    if (random(10)>prob){
      rain2.start();
    }
  }
     EVERY_N_MILLISECONDS( 850 ) { 
    if (random(10)>prob){
      rain3.start();
    }
  }
  EVERY_N_MILLISECONDS( 750 ) { 
    if (random(10)>prob){
      rain4.start();
    }
  }
  EVERY_N_MILLISECONDS( 1300 ) { 
    if (random(10)>prob){
      rain5.start();
    }
  }
    EVERY_N_MILLISECONDS( 1280 ) { 
    if (random(10)>prob){
      rain6.start();
    }
  }
  
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  my_fill_rainbow( leds_1, NUM_LEDS_PER_STRIP, gHue, 4);
  my_fill_rainbow( leds_2, NUM_LEDS_PER_STRIP, gHue, 4,1);
  my_fill_rainbow( leds_3, NUM_LEDS_PER_STRIP, gHue, 4);
  my_fill_rainbow( leds_4, NUM_LEDS_PER_STRIP, gHue, 4,1);
  my_fill_rainbow( leds_5, NUM_LEDS_PER_STRIP, gHue, 4);
  my_fill_rainbow( leds_6, NUM_LEDS_PER_STRIP, gHue, 4,1);
}

void confetti()
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
int pos = random16(NUM_LEDS);
leds[pos] += CHSV( gHue + random8(64), 200, 255);

}

void rainbowrain()
{
  fadeToBlackBy( leds, NUM_LEDS, 30);
  rain1.rain(0);
  rain2.rain(0);
  rain3.rain(0);
  rain4.rain(0);
  rain5.rain(0);
  rain6.rain(0);
}
