#include "FastLED.h"
#include "secrets.h"
// in secrets.h
// #define WIFI_SSID "ssid"
// #define WIFI_PASSWORD "pass"


//#Temp Wifi code
//#################################
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 20);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);

AsyncWebServer server(80);
//############################

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

//Dual rainbow rain substrips
CRGBSet leds_1(leds, 0,                                                                    99);
CRGBSet leds_2(leds, 100,                                                   NUM_LEDS_PER_STRIP);

//Quad rainbow rain substrips
CRGBSet leds_41(leds, 0,                                                                    50);
CRGBSet leds_42(leds, 50,                                                   100);
CRGBSet leds_43(leds, 100,                                                                    150);
CRGBSet leds_44(leds, 150,                                                   NUM_LEDS_PER_STRIP);

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
void whiteroom();


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

//Quad Rain option
//Instantiate rainbow rain for each strip
RainbowRain rain41(leds_41, 50);
RainbowRain rain42(leds_42, 50);
RainbowRain rain43(leds_43, 50);
RainbowRain rain44(leds_44, 50);

void setup() {
  delay(200); // 3 second delay for recovery

  // tell FastLED there's 60 NEOPIXEL leds on pin 3, starting at index 0 in the led array
  //FastLED.addLeds<NEOPIXEL, 18>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<WS2811, 18, RGB>(leds, NUM_LEDS_PER_STRIP).setCorrection(Typical8mmPixel);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setTemperature( Tungsten40W  ); // Set Temperature
  Serial.begin(115200);

  // Temp Wifi Stuff
  //#######################################
  WiFi.config(local_IP, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
  //###############################################
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, confetti, sinelon, juggle, bpm };
SimplePatternList gPatterns = { confetti, rainbow, rainbowrain, huefill, whiteroom };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop()
{
  AsyncElegantOTA.loop();
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
        // FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      case 1001 ...2000:
        gCurrentPatternNumber=1;
        // FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      case 2001 ...3000:
        gCurrentPatternNumber=2;
        // FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      case 3001 ...3500:
        gCurrentPatternNumber=3;
        // FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      default:
        gCurrentPatternNumber=4;
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
  // server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(200, "text/plain", String(modval));
  // });
  int8_t huerate=0;
  bool split=false;
  
    switch(modval){
      case 0 ...500:
        huerate=0;
        break;
      case 501 ...1000:
        huerate=1;
        break;
      case 1001 ...1500:
        huerate=2;
        break;
      case 1501 ...2000:
        huerate=3;
        break;
      case 2001 ...2500:
        huerate=-3;
        break;
      case 2501 ...3000:
        huerate=-2;
        break;
      case 3001 ...3500:
        huerate=-1;
        break;
      default:
        huerate=1;
        split=true;
        break;
    }
    // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+huerate; } 

  if(!split){
  // FastLED's built-in rainbow generator
    my_fill_rainbow( leds, NUM_LEDS, gHue, 4, 0);
  }
  else{
    my_fill_rainbow( leds_1, NUM_LEDS/2, gHue, 5, 0);
    my_fill_rainbow( leds_2, NUM_LEDS/2, gHue, 5, 1);
  }
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
  bool quad=false;
  
    switch(modval){
      case 0 ...500:
        p = 20;
        fadeToBlackBy( leds, NUM_LEDS, 10);
        break;
      case 501 ...1000:
        p = 30;
        fadeToBlackBy( leds, NUM_LEDS, 8);
        break;
      case 1001 ...1500:
        p = 40;
        fadeToBlackBy( leds, NUM_LEDS, 4);
        break;
      case 1501 ...2000:
        p = 50;
        fadeToBlackBy( leds, NUM_LEDS, 1);
        break;
      case 2001 ...2500:
        p = 20;
        quad=true;
        fadeToBlackBy( leds, NUM_LEDS, 10);
        break;
      case 2501 ...3000:
         p = 30;
         quad=true;
         fadeToBlackBy( leds, NUM_LEDS, 8);
        break;
      case 3001 ...3500:
         p = 40;
         fadeToBlackBy( leds, NUM_LEDS, 4);
         quad=true;
        break;
      default:
         p = 50;
         quad=true;
         fadeToBlackBy( leds, NUM_LEDS, 1);
        break;
    }
    // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue+1; }
  
  if(!quad){
    rain1.rain(1);
    rain2.rain();
  }
  else{
    rain41.rain(1);
    rain42.rain();
    rain43.rain(1);
    rain44.rain();
  }
}

void whiteroom(){
    // CHSV hsv;
    // hsv.hue = 0;
    // hsv.val = 255;
    // hsv.sat = 0;

    fill_solid(leds, NUM_LEDS, CRGB::White);
  switch(modval){
      case 0 ...1000:
        FastLED.setTemperature( Candle  ); // Set Temperature
        break;
      case 1001 ...2000:
          FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      case 2001 ...3000:
        FastLED.setTemperature( Tungsten40W  ); // Set Temperature
        break;
      default:
        FastLED.setTemperature(0xFF7029);
        break;
    }
}