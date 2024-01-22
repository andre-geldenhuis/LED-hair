#ifndef RAINBOWRAIN_H
#define RAINBOWRAIN_H

#include "FastLED.h"

extern uint8_t gHue; // Access to the global hue variable

class RainbowRain {
public:
    RainbowRain(CRGB *leds, int numStrips, int ledsPerStrip);
    void update();

private:
    struct Raindrop {
        unsigned long lastUpdate;
        uint8_t position;
        bool active;
    };

    static const int maxStrips = 8; // Maximum number of strips
    Raindrop raindrops[maxStrips];   // Array of raindrops
    CRGB *leds;
    int numStrips;
    int ledsPerStrip;
    void moveRaindrop(int stripIndex);
};

#endif // RAINBOWRAIN_H
