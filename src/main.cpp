#include <OctoWS2811.h>
#include "FastLED.h"
#include "OctoWS2811FastLED.h"

FASTLED_USING_NAMESPACE

// Use  OctoWS2811 with fastled, see: https://blinkylights.blog/2021/02/03/using-teensy-4-1-with-fastled/
const int numPins = 8;
byte pinList[numPins] = {2, 3, 4, 5, 6, 7, 8, 9};
const int ledsPerStrip = 160;
CRGB rgbarray[numPins * ledsPerStrip];

// These buffers need to be large enough for all the pixels.
// The total number of pixels is "ledsPerStrip * numPins".
// Each pixel needs 3 bytes, so multiply by 3.  An "int" is
// 4 bytes, so divide by 4.  The array is created using "int"
// so the compiler will align it to 32 bit memory.
DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
int drawingMemory[ledsPerStrip * numPins * 3 / 4];
OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);





// #define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 160 
#define NUM_LEDS 160

//#define BRIGHTNESS          30
#define BRIGHTNESS          10
#define FRAMES_PER_SECOND  120


int potval = 0;
bool runleds = true;
//Forward Declare Functions
// void nextPattern();
void rainbow();
// void rainbowWithGlitter();
// void addGlitter( fract8 chanceOfGlitter);
// void confetti();
// void sinelon();
// void bpm();
// void juggle();
// void rainbowrain();


//period between trail movement for rainbow_rain, milliseconds - for framerate locking
unsigned long p = 14;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void my_fill_rainbow(CRGB *leds, int totalLeds, uint8_t initialhue, uint8_t deltahue, bool dir = 0) {
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;

    if (dir == 0) {
        // Forward direction
        for (int i = 0; i < totalLeds; i++) {
            leds[i] = hsv;
            hsv.hue += deltahue;
        }
    } else {
        // Reverse direction
        for (int i = totalLeds - 1; i >= 0; i--) {
            leds[i] = hsv;
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

//Instantiate rainbow rain for each strip

// RainbowRain rain1(leds1, 160);
// RainbowRain rain2(leds2, 160);


CTeensy4Controller<GRB, WS2811_800kHz> *pcontroller;
void setup() {
  delay(200); //delay for recovery
  octo.begin();
  pcontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octo);

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds(pcontroller, rgbarray, numPins * ledsPerStrip);

// FastLED.setMaxRefreshRate(120);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // FastLED.setTemperature( Tungsten40W  ); // Set Temperature

}


// // List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { rainbow };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current


const int specificStrip = 0; // For example, the 4th strip (zero-based)
const int startIdx = specificStrip * ledsPerStrip;
const int endIdx = (specificStrip + 1) * ledsPerStrip - 1;
void loop()
{
// // Example: Move a red dot along the specific strip
//     for (int i = startIdx; i <= endIdx; i++) {
//         // Clear the strip
//         for (int j = startIdx; j <= endIdx; j++) {
//             rgbarray[j] = CRGB::Black;
//         }

//         // Set the current pixel to red
//         rgbarray[i] = CRGB::Red;

//         // Update the LEDs
//         FastLED.show();

//         // Wait a little bit
//         // delay(50);
//     }


  if(runleds){
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
  }
  else{
    // fadeToBlackBy( leds, NUM_LEDS, 10);  
  }
  

  // send the 'leds' array out to the actual LED strip
  FastLED.show();

  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+1; 
  } // slowly cycle the "base color" through the rainbow


  // EVERY_N_MILLISECONDS ( 100 ){
  //   potval = analogRead(A9);
  //   if(potval <=250){
  //    runleds=false;
  //    FastLED.setBrightness(0);
  //  }
  //  else if(potval<=500){
  //    gCurrentPatternNumber=0;
  //    FastLED.setBrightness(BRIGHTNESS);
  //    runleds=true;
  //  }
  //  else if(potval<=750){  // rainbow dim
  //   gCurrentPatternNumber=1;
  //   FastLED.setBrightness(BRIGHTNESS);
  //   runleds=true;
  //  }
  //  else{   //rainbow rain
  //       gCurrentPatternNumber=2;
  //       FastLED.setBrightness(BRIGHTNESS);
  //       runleds=true;
  //  }
  // }




  gCurrentPatternNumber=0;
  FastLED.setBrightness(BRIGHTNESS);
  runleds=true;


  // EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
}

// #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// void nextPattern()
// {
//   // add one to the current pattern number, and wrap around at the end
//   gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
// }

void rainbow()
{
  // rainbow generator
   my_fill_rainbow(rgbarray, numPins * ledsPerStrip, gHue, 5, 0);

}

// void rainbowWithGlitter()
// {
//   // built-in FastLED rainbow, plus some random sparkly glitter
//   rainbow();
//   addGlitter(80);
// }

// void addGlitter( fract8 chanceOfGlitter)
// {
//   // if( random8() < chanceOfGlitter) {
//   //   leds[ random16(NUM_LEDS) ] += CRGB::White;
//   // }
// }

// void confetti()
// {
//   // // random colored speckles that blink in and fade smoothly
//   // fadeToBlackBy( leds, NUM_LEDS, 10);
//   // int pos = random16(NUM_LEDS);
//   // int colornum = random8(3);
//   // if(colornum==0){
//   //   // leds[pos] += CHSV( gHue + random8(64), 200, 255);
//   //   leds[pos] += CRGB::Aqua;
//   // }
//   // else if(colornum==1){
//   //   leds[pos] += CRGB::White;
//   // }
//   // else{
//   //   leds[pos] += leds[pos] += CRGB::DeepPink;
//   // }
  
// }

// void sinelon()
// {
//   // // a colored dot sweeping back and forth, with fading trails
//   // fadeToBlackBy( leds, NUM_LEDS, 20);
//   // int pos = beatsin16( 13, 0, NUM_LEDS-1 );
//   // leds[pos] += CHSV( gHue, 255, 192);
// }

// void bpm()
// {
//   // // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//   // uint8_t BeatsPerMinute = 62;
//   // CRGBPalette16 palette = PartyColors_p;
//   // uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//   // for( int i = 0; i < NUM_LEDS; i++) { //9948
//   //   leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
//   // }
// }

// void juggle() {
//   // eight colored dots, weaving in and out of sync with each other
//   // fadeToBlackBy( leds, NUM_LEDS, 20);
//   // byte dothue = 0;
//   // for( int i = 0; i < 8; i++) {
//   //   leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
//   //   dothue += 32;
//   // }
// }

// void rainbowrain()
// {
//   // fadeToBlackBy( leds, NUM_LEDS, 17);
//   // rain1.rain(1);
//   // rain2.rain();
// }
