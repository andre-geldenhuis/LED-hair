#ifndef WHITERAIN_H
#define WHITERAIN_H

#include "FastLED.h"

class WhiteRain {
public:
    WhiteRain(CRGB *leds, int numStrips, int ledsPerStrip);
    void update();

private:
    struct Raindrop {
        uint8_t position;
        bool active;
    };

    static const int maxStrips = 8; // Maximum number of strips
    Raindrop raindrops[maxStrips];   // Array of raindrops
    CRGB *leds;
    int numStrips;
    int ledsPerStrip;
    void moveRaindrop(int stripIndex);
    void fadeTrail(int stripIndex);
};

#endif // WHITERAIN_H
