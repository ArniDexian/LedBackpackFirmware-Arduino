#include "LEDMatrix.h"

class EffectInlineRun {
    LEDMatrix* matrix;
    int delayTime;
    CRGB color;

   public:
    EffectInlineRun(LEDMatrix* matrix, CRGB color = CRGB::Red, int delayTime = 200) 
        : matrix(matrix), color(color), delayTime(delayTime) {}

    void run() {
        for (int i = 0; i < matrix->rows; i++) {
            for (int j = 0; j < matrix->columns; j++) {
                matrix->setColor(j, i, color);
                matrix->flush();
                delay(delayTime);
                matrix->clear();
            }
        }
    }
};