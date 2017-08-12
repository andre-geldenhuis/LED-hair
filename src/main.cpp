#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 60
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

//#define BRIGHTNESS          30
#define BRIGHTNESS          30
#define FRAMES_PER_SECOND  120

//Forward Declare Functions
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();

// #Custom rainbow
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

void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED there's 60 NEOPIXEL leds on pin 3, starting at index 0 in the led array
  FastLED.addLeds<NEOPIXEL, 3>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 11, starting at index 60 in the led array
  FastLED.addLeds<NEOPIXEL, 4>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 5, starting at index 120 in the led array
  FastLED.addLeds<NEOPIXEL, 5>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 6>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 23>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 22>(leds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 21>(leds, 6 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // tell FastLED there's 60 NEOPIXEL leds on pin 6, starting at index 180 in the led array
  FastLED.addLeds<NEOPIXEL, 20>(leds, 7 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

// FastLED.addLeds<LED_TYPE,6,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { rainbow, confetti, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+3; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  my_fill_rainbow( leds, NUM_LEDS, gHue, 7, 0);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
