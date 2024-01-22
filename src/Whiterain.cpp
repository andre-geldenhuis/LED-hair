#include "WhiteRain.h"

WhiteRain::WhiteRain(CRGB *ledsArray, int numStrips, int ledsPerStrip)
    : leds(ledsArray), numStrips(numStrips), ledsPerStrip(ledsPerStrip) {
    for (int i = 0; i < maxStrips; i++) {
        raindrops[i].position = 0;
        raindrops[i].active = false; // Initially inactive
    }
}

void WhiteRain::update() {
    for (int i = 0; i < numStrips; i++) {
        if (!raindrops[i].active) {
            // Activate the raindrop
            raindrops[i].active = true;
            raindrops[i].position = 0; // Reset position
        }
        moveRaindrop(i);
        fadeTrail(i);
    }
}

void WhiteRain::moveRaindrop(int stripIndex) {
    Raindrop &drop = raindrops[stripIndex];
    if (drop.active) {
        int ledIndex = stripIndex * ledsPerStrip + drop.position;
        leds[ledIndex] = CRGB::White; // Set the LED to white

        // Increment the position
        drop.position++;

        // Reset the raindrop if it reaches the end of the strip
        if (drop.position >= ledsPerStrip) {
            drop.position = 0;
            drop.active = false; // Reset to inactive
        }
    }
}

void WhiteRain::fadeTrail(int stripIndex) {
    int startIdx = stripIndex * ledsPerStrip;
    int endIdx = startIdx + ledsPerStrip;
    for (int i = startIdx; i < endIdx; i++) {
        leds[i].fadeToBlackBy(4); // Adjust fade value as needed
    }
}
