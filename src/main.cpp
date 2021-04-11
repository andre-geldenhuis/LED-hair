#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NUM_STRIPS 1
#define NUM_LEDS_PER_STRIP 200
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS

#define MODE_POT 34
#define MOD_POT 35
#define BRIGHT_POT 32
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

CRGBSet leds_1(leds, 0,                                                                    99);
CRGBSet leds_2(leds, 100,                                                   NUM_LEDS_PER_STRIP);

//#define BRIGHTNESS          30
#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120


int modeval = 0;
int modval = 0;
int brightval = 0;
bool runleds = true;
//Forward Declare Functions
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void rainbowrain();
void huefill();


//period between trail movement for rainbow_rain, milliseconds - for framerate locking
unsigned long p = 14;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

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

    struct CRGB * pleds; 
    uint8_t num_per_strip;

    //constructor
  public:
    RainbowRain(struct CRGB * ipleds, uint8_t inum_per_strip) {
      pleds=ipleds;
      num_per_strip=inum_per_strip;
    }

    void rain(bool dir=0) {
      if (millis() - previous_millis >= p) {
        previous_millis = millis();
        if (dir == 0){
          rainpos++;
          if (rainpos >= num_per_strip) {
            rainpos = 0;
          }
        }
        else{
          rainpos--;
          if (rainpos >= num_per_strip) { //rainpos <= 0 # when rainpos goes -1 it wraps, not checking this lets us address the 0th led
            rainpos = num_per_strip-1;
          }
        }
      pleds[rainpos] += CHSV( gHue, 255, 192);  
      }
    }
};


void huefill()
{
    CHSV hsv;
    hsv.hue = (map(modval, 0, 4095, 0, 255));
    hsv.val = 255;
    hsv.sat = 240;
    fill_solid(leds, NUM_LEDS, hsv);
}

//Instantiate rainbow rain for each strip
RainbowRain rain1(leds_1, 100);
RainbowRain rain2(leds_2, 100);
void setup() {
  delay(200); // 3 second delay for recovery

  // tell FastLED there's 60 NEOPIXEL leds on pin 3, starting at index 0 in the led array
  FastLED.addLeds<NEOPIXEL, 18>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setTemperature( Tungsten40W  ); // Set Temperature
  Serial.begin(115200);

}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { confetti, rainbow, rainbowrain, huefill };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop()
{
  if(runleds){
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
  }
  else{
    fadeToBlackBy( leds, NUM_LEDS, 10);  
  }
  

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+3; 
  } // slowly cycle the "base color" through the rainbow
  EVERY_N_MILLISECONDS(300){
    Serial.println(modval);
  }
  EVERY_N_MILLISECONDS ( 100 ){
    modeval   = analogRead(MODE_POT);
    modval    = analogRead(MOD_POT);
    brightval = analogRead(BRIGHT_POT);
    if(brightval <=200){
     runleds=false;
     FastLED.setBrightness(0);
    }
    else{
      FastLED.setBrightness(map(brightval, 0, 4095, 0, 255));
      runleds=true;
    }

    switch(modeval){
      case 0 ...1000:
        gCurrentPatternNumber=0;
        break;
      case 1001 ...2000:
        gCurrentPatternNumber=1;
        break;
      case 2001 ...3000:
        gCurrentPatternNumber=2;
        break;
      default:
        gCurrentPatternNumber=3;
        break;
    }
  }
  // EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
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
  my_fill_rainbow( leds, NUM_LEDS, gHue, 4, 0);
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
  int colornum = random8(3);
  if(colornum==0){
    // leds[pos] += CHSV( gHue + random8(64), 200, 255);
    leds[pos] += CRGB::Aqua;
  }
  else if(colornum==1){
    leds[pos] += CRGB::White;
  }
  else{
    leds[pos] += leds[pos] += CRGB::DeepPink;
  }
  
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

void rainbowrain()
{
  fadeToBlackBy( leds, NUM_LEDS, 17);
  rain1.rain(1);
  rain2.rain();
}
