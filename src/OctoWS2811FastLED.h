#ifndef OCTOWS2811FASTLED_H
#define OCTOWS2811FASTLED_H

#include <OctoWS2811.h>
#include "FastLED.h"

template <EOrder RGB_ORDER = GRB, uint8_t CHIP = WS2811_800kHz>
class CTeensy4Controller : public CPixelLEDController<RGB_ORDER, 8, 0xFF> {
public:
    CTeensy4Controller(OctoWS2811 *_pocto);
    virtual void init();
    virtual void showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels);

private:
    OctoWS2811 *pocto;
};

template <EOrder RGB_ORDER, uint8_t CHIP>
CTeensy4Controller<RGB_ORDER, CHIP>::CTeensy4Controller(OctoWS2811 *_pocto) : pocto(_pocto) {
    // Constructor implementation
}

template <EOrder RGB_ORDER, uint8_t CHIP>
void CTeensy4Controller<RGB_ORDER, CHIP>::init() {
    // init implementation (if any)
}

template <EOrder RGB_ORDER, uint8_t CHIP>
void CTeensy4Controller<RGB_ORDER, CHIP>::showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels) {
    uint32_t i = 0;
    while (pixels.has(1))
    {
        uint8_t r = pixels.loadAndScale0();
        uint8_t g = pixels.loadAndScale1();
        uint8_t b = pixels.loadAndScale2();
        pocto->setPixel(i++, r, g, b);
        pixels.stepDithering();
        pixels.advanceData();
    }
    pocto->show();   
}


#endif
