#ifndef RAINBOWRAIN_H
#define RAINBOWRAIN_H

#include "FastLED.h"

extern uint8_t gHue; // Access to the global hue variable
extern unsigned long p; // Access to the global interval variable

class RainbowRain {
public:
    RainbowRain(CRGB *leds, int totalLeds);
    void rain(bool dir = 0);

private:
    CRGB *leds;
    int totalLeds;
    unsigned long previous_millis;
    uint8_t rainpos;
};


#endif // RAINBOWRAIN_H


