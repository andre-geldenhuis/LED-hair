#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NUM_STRIPS 1
#define NUM_LEDS_PER_STRIP 320
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS)

#define MODE_POT 34
#define MOD_POT 35
#define BRIGHT_POT 32

CRGB leds[NUM_LEDS];

// Dual rainbow rain substrips
CRGBSet leds_1(leds, 0, 99);
CRGBSet leds_2(leds, 100, 199);

// Quad rainbow rain substrips
CRGBSet leds_41(leds, 0, 49);
CRGBSet leds_42(leds, 50, 99);
CRGBSet leds_43(leds, 100, 149);
CRGBSet leds_44(leds, 150, 199);

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 120

int modeval = 0;
int modval = 0;
int brightval = 0;
bool runleds = true;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int hueIncrement = 0;
int hueUpdateInterval = 20; // milliseconds

// Forward declare functions
void nextPattern();
void rainbow();
void confetti();
void rainbowrain();
void huefill();
void whiteroom();
void updateHueParameters();

// Define ARRAY_SIZE macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Move the declaration of 'p' before the 'RainbowRain' class
unsigned long p = 14; // period between trail movement for rainbow_rain, milliseconds

// Custom fill_rainbow function with direction
void my_fill_rainbow(struct CRGB* pFirstLED, int numToFill, uint8_t initialhue, uint8_t deltahue, bool dir = 0)
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    if (dir == 1) {
        for (int i = 0; i < numToFill; i++) {
            pFirstLED[i] = hsv;
            hsv.hue += deltahue;
        }
    } else {
        for (int i = numToFill - 1; i >= 0; i--) {
            pFirstLED[i] = hsv;
            hsv.hue += deltahue;
        }
    }
}

class RainbowRain
{
    unsigned long previous_millis = millis();
    uint8_t rainpos = 0;
    struct CRGB* pleds;
    uint8_t num_per_strip;

public:
    RainbowRain(struct CRGB* ipleds, uint8_t inum_per_strip)
        : pleds(ipleds), num_per_strip(inum_per_strip)
    {
    }

    void rain(bool dir = 0)
    {
        if (millis() - previous_millis >= p) {
            previous_millis = millis();
            if (dir == 0) {
                rainpos++;
                if (rainpos >= num_per_strip) {
                    rainpos = 0;
                }
            } else {
                if (rainpos == 0) {
                    rainpos = num_per_strip - 1;
                } else {
                    rainpos--;
                }
            }
            pleds[rainpos] += CHSV(gHue, 255, 192);
        }
    }
};

void huefill()
{
    CHSV hsv;
    hsv.hue = map(modval, 0, 4095, 0, 255);
    hsv.val = 255;
    hsv.sat = 240;
    fill_solid(leds, NUM_LEDS, hsv);
}

// Instantiate rainbow rain for each strip
RainbowRain rain1(leds_1, 100);
RainbowRain rain2(leds_2, 100);

// Quad Rain option
RainbowRain rain41(leds_41, 50);
RainbowRain rain42(leds_42, 50);
RainbowRain rain43(leds_43, 50);
RainbowRain rain44(leds_44, 50);

void setup()
{
    delay(200); // 200ms delay for recovery

    FastLED.addLeds<WS2811, 18, RGB>(leds, NUM_LEDS).setCorrection(Typical8mmPixel);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setTemperature(Tungsten40W); // Set Temperature
    Serial.begin(115200);
}

// List of patterns to cycle through. Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { confetti, rainbow, rainbowrain, huefill, whiteroom };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop()
{
    if (runleds) {
        // Call the current pattern function once, updating the 'leds' array
        gPatterns[gCurrentPatternNumber]();
    } else {
        fadeToBlackBy(leds, NUM_LEDS, 10);
    }

    // Update gHue based on the current pattern and modval
    EVERY_N_MILLISECONDS(hueUpdateInterval)
    {
        gHue += hueIncrement;
    }

    // Send the 'leds' array out to the actual LED strip
    FastLED.show();

    // Insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    EVERY_N_MILLISECONDS(100)
    {
        // modeval = analogRead(MODE_POT);
        // modval = analogRead(MOD_POT);
        // brightval = analogRead(BRIGHT_POT);
        runleds = true;
        FastLED.setBrightness(255);

        // if (brightval <= 200) {
        //     runleds = false;
        //     FastLED.setBrightness(0);
        // } else {
        //     FastLED.setBrightness(map(brightval, 0, 4095, 0, 255));
        //     runleds = true;
        // }
        gCurrentPatternNumber = 2;
        // switch (modeval) {
        // case 0 ... 1000:
        //     gCurrentPatternNumber = 0;
        //     break;
        // case 1001 ... 2000:
        //     gCurrentPatternNumber = 1;
        //     break;
        // case 2001 ... 3000:
        //     gCurrentPatternNumber = 2;
        //     break;
        // case 3001 ... 3500:
        //     gCurrentPatternNumber = 3;
        //     break;
        // default:
        //     gCurrentPatternNumber = 4;
        //     break;
        // }

        // Update hue parameters based on the current pattern and modval
        updateHueParameters();
    }
}

void updateHueParameters()
{
    switch (gCurrentPatternNumber) {
    case 1: // rainbow pattern
    {
        int8_t huerate = 0;
        switch (modval) {
        case 0 ... 500:
            huerate = 0;
            break;
        case 501 ... 1000:
            huerate = 1;
            break;
        case 1001 ... 1500:
            huerate = 2;
            break;
        case 1501 ... 2000:
            huerate = 3;
            break;
        case 2001 ... 2500:
            huerate = -3;
            break;
        case 2501 ... 3000:
            huerate = -2;
            break;
        case 3001 ... 3500:
            huerate = -1;
            break;
        default:
            huerate = 1;
            break;
        }
        hueIncrement = huerate;
        hueUpdateInterval = 20;
        break;
    }
    case 2: // rainbowrain pattern
        hueIncrement = 1;
        hueUpdateInterval = 1;
        break;
    default:
        hueIncrement = 0;
        hueUpdateInterval = 1000; // No hue change
        break;
    }
}

void nextPattern()
{
    // Add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
    bool split = false;

    switch (modval) {
    case 3001 ... 3500:
        split = true;
        break;
    default:
        split = false;
        break;
    }

    if (!split) {
        // FastLED's built-in rainbow generator
        my_fill_rainbow(leds, NUM_LEDS, gHue, 4, 0);
    } else {
        my_fill_rainbow(leds_1, NUM_LEDS / 2, gHue, 5, 0);
        my_fill_rainbow(leds_2, NUM_LEDS / 2, gHue, 5, 1);
    }
}

void confetti()
{
    // Random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    int colornum = random8(3);
    if (colornum == 0) {
        leds[pos] += CRGB::Aqua;
    } else if (colornum == 1) {
        leds[pos] += CRGB::White;
    } else {
        leds[pos] += CRGB::DeepPink;
    }
}

void rainbowrain()
{
    bool quad = false;

    switch (modval) {
    case 0 ... 500:
        p = 20;
        fadeToBlackBy(leds, NUM_LEDS, 10);
        break;
    case 501 ... 1000:
        p = 30;
        fadeToBlackBy(leds, NUM_LEDS, 8);
        break;
    case 1001 ... 1500:
        p = 40;
        fadeToBlackBy(leds, NUM_LEDS, 4);
        break;
    case 1501 ... 2000:
        p = 50;
        fadeToBlackBy(leds, NUM_LEDS, 1);
        break;
    case 2001 ... 2500:
        p = 20;
        quad = true;
        fadeToBlackBy(leds, NUM_LEDS, 10);
        break;
    case 2501 ... 3000:
        p = 30;
        quad = true;
        fadeToBlackBy(leds, NUM_LEDS, 8);
        break;
    case 3001 ... 3500:
        p = 40;
        fadeToBlackBy(leds, NUM_LEDS, 4);
        quad = true;
        break;
    default:
        p = 50;
        quad = true;
        fadeToBlackBy(leds, NUM_LEDS, 1);
        break;
    }

    if (!quad) {
        rain1.rain(1);
        rain2.rain();
    } else {
        rain41.rain(1);
        rain42.rain();
        rain43.rain(1);
        rain44.rain();
    }
}

void whiteroom()
{
    fill_solid(leds, NUM_LEDS, CRGB::White);
    switch (modval) {
    case 0 ... 1000:
        FastLED.setTemperature(Candle); // Set Temperature
        break;
    case 1001 ... 2000:
        FastLED.setTemperature(Tungsten40W); // Set Temperature
        break;
    case 2001 ... 3000:
        FastLED.setTemperature(Tungsten100W); // Set Temperature
        break;
    default:
        FastLED.setTemperature(0xFF7029);
        break;
    }
}
