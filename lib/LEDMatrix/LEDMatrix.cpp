#include "LEDMatrix.h"
#include <FastLED.h>

LEDMatrix::LEDMatrix(uint8_t rows, uint8_t columns, uint8_t brightness, uint8_t voltage, uint32_t milliaps)
    : rows(rows), columns(columns), brightness(brightness) {
        // TODO: - stop if rows * columns > LEDS_COUNT
    FastLED.addLeds<NEOPIXEL, LEDS_DATA_PIN>((CRGB*)ledColors, LEDS_COUNT);
    FastLED.setBrightness(brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(voltage, milliaps);
}

void LEDMatrix::setColor(int x, int y, CRGB color) {
    if ((y & 1) == 0) {
        ledColors[y * columns + x] = color;
    } else {
        ledColors[y * columns + (columns - x - 1)] = color;
    }
}

void LEDMatrix::flush() {
    FastLED.show(brightness);
}

void LEDMatrix::clear(bool _flush) {
    FastLED.clear(_flush);
}