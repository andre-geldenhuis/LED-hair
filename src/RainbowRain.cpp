#include "RainbowRain.h"

RainbowRain::RainbowRain(CRGB *ledsArray, int numLeds)
    : leds(ledsArray), totalLeds(numLeds), rainpos(0) {
    previous_millis = millis();
}

void RainbowRain::rain(bool dir) {
    if (millis() - previous_millis >= p) {
        previous_millis = millis();
        if (dir == 0) {
            rainpos++;
            if (rainpos >= totalLeds) {
                rainpos = 0;
            }
        } else {
            if (rainpos == 0) {
                rainpos = totalLeds;
            }
            rainpos--;
        }
        leds[rainpos] += CHSV(gHue, 255, 192);
    }
}