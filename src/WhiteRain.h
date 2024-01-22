#ifndef WHITERAIN_H
#define WHITERAIN_H

#include "FastLED.h"

class WhiteRain {
public:
    WhiteRain(CRGB *leds, int numStrips, int ledsPerStrip, int moveFrequency); // Added moveFrequency
    void update();

private:
    struct Raindrop {
        uint8_t position;
        bool active;
    };

    static const int maxStrips = 8;
    Raindrop raindrops[maxStrips];
    CRGB *leds;
    int numStrips;
    int ledsPerStrip;
    int moveFrequency; // Move raindrop every N updates
    int moveCounter;   // Counter for updates
    void moveRaindrop(int stripIndex, bool moveDrop);
    void fadeTrail(int stripIndex);
};

#endif // WHITERAIN_H
