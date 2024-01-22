#ifndef RAINBOWEFFECT_H
#define RAINBOWEFFECT_H

#include "FastLED.h"

extern uint8_t gHue; // Declare the external global variable

class RainbowEffect {
public:
    RainbowEffect(CRGB *leds, int totalLeds);
    void update();

private:
    CRGB *leds;
    int totalLeds;
    void fillRainbow(uint8_t deltahue, bool dir = 0);
};

#endif // RAINBOWEFFECT_H
