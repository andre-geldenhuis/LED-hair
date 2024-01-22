#include "RainbowRain.h"

RainbowRain::RainbowRain(CRGB *ledsArray, int numStrips, int ledsPerStrip)
    : leds(ledsArray), numStrips(numStrips), ledsPerStrip(ledsPerStrip) {
    for (int i = 0; i < maxStrips; i++) {
        raindrops[i].lastUpdate = millis();
        raindrops[i].position = 0;
        raindrops[i].active = false; // Initially inactive
    }
}

void RainbowRain::update() {
    for (int i = 0; i < numStrips; i++) {
        if (!raindrops[i].active) {
            // Activate the raindrop at different times (staggered start)
            if (millis() - raindrops[i].lastUpdate > i * 1000) { // Staggered start
                raindrops[i].active = true;
                raindrops[i].position = 0; // Reset position
            }
        }
        if (raindrops[i].active) {
            moveRaindrop(i);
        }
    }
}

void RainbowRain::moveRaindrop(int stripIndex) {
    Raindrop &drop = raindrops[stripIndex];

    // Move the raindrop down the strip
    int ledIndex = stripIndex * ledsPerStrip + drop.position;
    leds[ledIndex] += CHSV(gHue, 255, 192); // Color the LED at the raindrop's position

    // Increment the position
    drop.position++;

    // Reset the raindrop if it reaches the end of the strip
    if (drop.position >= ledsPerStrip) {
        drop.position = 0;
        drop.active = false; // Reset to inactive
        drop.lastUpdate = millis(); // Reset the timer
    }
}
