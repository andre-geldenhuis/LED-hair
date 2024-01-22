#include <OctoWS2811.h>
#include "FastLED.h"
#include "OctoWS2811FastLED.h"
#include "RainbowEffect.h"
#include "RainbowRain.h"
#include "WhiteRain.h"

#include <Wire.h>
#include <mpu6050.h>

MPU6050 imu;


const int bufferSize = 1000;
float buffer[bufferSize];
int bufferIndex = 0;
float bufferSum = 0;

const unsigned long debounceInterval = 250; // Debounce interval in milliseconds
unsigned long lastStepTime = 0;

void addToBuffer(float value) {
    // Subtract the oldest value from the sum and replace it with the new value
    bufferSum -= buffer[bufferIndex];
    buffer[bufferIndex] = value;
    bufferSum += value;

    // Update the buffer index (circular buffer)
    bufferIndex = (bufferIndex + 1) % bufferSize;
}

float calculateMean() {
    return bufferSum / bufferSize;
}


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

#define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 160 
#define NUM_LEDS 160 * NUM_STRIPS 

#define BRIGHTNESS          10
#define FRAMES_PER_SECOND  120

RainbowEffect rainbow(rgbarray, numPins * ledsPerStrip);
RainbowRain rainEffect(rgbarray, numPins, ledsPerStrip);
WhiteRain whiteRainEffect(rgbarray, numPins, ledsPerStrip);


// Wrapper function for the rainbow effect
void callRainbow() {
    rainbow.update(); // Use 'rainbow' instead of 'rainbowEffect'
}

void callRainEffect() {
    rainEffect.update(); // Call the update method of the rainEffect instance
    fadeToBlackBy(rgbarray, numPins * ledsPerStrip, 10); // Gradually dim the LEDs
}

// Wrapper function for the WhiteRain effect
void callWhiteRainEffect() {
    whiteRainEffect.update(); // Call the update method on the object instance
}

int potval = 0;
bool runleds = true;


//period between trail movement for rainbow_rain, milliseconds - for framerate locking
unsigned long p = 14;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CTeensy4Controller<GRB, WS2811_800kHz> *pcontroller;
void setup() {
  delay(200); //delay for recovery
  octo.begin();
  pcontroller = new CTeensy4Controller<GRB, WS2811_800kHz>(&octo);

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds(pcontroller, rgbarray, numPins * ledsPerStrip);

FastLED.setMaxRefreshRate(120);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // FastLED.setTemperature( Tungsten40W  ); // Set Temperature

    Wire1.begin();
    Serial.begin(115200);
    if (!imu.begin(AFS_2G, GFS_250DPS)) {
        Serial.println("MPU6050 is online...");
    }
    else {
        Serial.println("Failed to init MPU6050");
        while (true) 
            ;
    }

    // Initialize buffer
    for (int i = 0; i < bufferSize; ++i) {
        buffer[i] = 0;
    }

}


// // List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = { callRainbow, callRainEffect, callWhiteRainEffect };

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
    callWhiteRainEffect();
  }
  else{ 
    fadeToBlackBy( rgbarray, NUM_LEDS, 10);  
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
 
  int16_t ax, ay, az, gx, gy, gz;
  if (imu.getMotion6Counts(&ax, &ay, &az, &gx, &gy, &gz)) {
      float magnitude = sqrt(ax * ax + ay * ay + az * az);
      addToBuffer(magnitude);

      // Check if the buffer is fully populated
      if (buffer[bufferSize - 1] != 0) {
          float mean = calculateMean();

          // Step detection logic
          unsigned long currentTime = millis();
          if (magnitude > mean * 1.05 && (currentTime - lastStepTime) >= debounceInterval) {
              // Count a step
              lastStepTime = currentTime;
              Serial.println("Step:");
          }
      }
  }

  // EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically
}

// #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// void nextPattern()
// {
//   // add one to the current pattern number, and wrap around at the end
//   gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
// }