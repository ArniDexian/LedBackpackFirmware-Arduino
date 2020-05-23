#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include "FastLED.h"

#ifndef LEDS_COUNT
#define LEDS_COUNT 144
#endif

#ifndef LEDS_DATA_PIN
#define LEDS_DATA_PIN 3
#endif

class LEDMatrix {
    CRGB ledColors[LEDS_COUNT];

   public:
    const uint8_t rows;
    const uint8_t columns;
    uint8_t brightness;

    LEDMatrix(uint8_t rows, uint8_t columns, uint8_t brightness = 128, uint8_t voltage = 5, uint32_t milliaps = 1000);

    void setColor(int x, int y, CRGB color);
    void flush();
    void clear(bool _flush = true);
};

#endif