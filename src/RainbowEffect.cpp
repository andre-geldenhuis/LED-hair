#include "RainbowEffect.h"

RainbowEffect::RainbowEffect(CRGB *ledsArray, int numLeds) : leds(ledsArray), totalLeds(numLeds) {}

void RainbowEffect::update() {
    fillRainbow(5, 0);
}

void RainbowEffect::fillRainbow(uint8_t deltahue, bool dir) {
    CHSV hsv;
    hsv.hue = gHue;
    hsv.val = 255;
    hsv.sat = 240;

    if (dir == 0) {
        for (int i = 0; i < totalLeds; i++) {
            leds[i] = hsv;
            hsv.hue += deltahue;
        }
    } else {
        for (int i = totalLeds - 1; i >= 0; i--) {
            leds[i] = hsv;
            hsv.hue += deltahue;
        }
    }
}
