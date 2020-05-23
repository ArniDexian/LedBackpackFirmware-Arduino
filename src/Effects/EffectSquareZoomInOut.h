#include "LEDMatrix.h"
#include "FastLED.h"

class EffectSquareZoomInOut {
   public:
    enum Type {
        ZoomIn,
        ZoomOut,
        ZoomInOut
    };

   private:
    Type type;
    LEDMatrix* matrix;
    CRGB color;
    int delayTime;

   public:
    EffectSquareZoomInOut(LEDMatrix* matrix, Type type = ZoomOut, CRGB color = CRGB::White, int delayTime = 200)
        : matrix(matrix), type(type), color(color), delayTime(delayTime) {}

    void run() {
        switch (type) {
            case ZoomIn:
                zoomIn();
                break;
            case ZoomOut:
                zoomOut();
                break;
            case ZoomInOut:
                zoomIn();
                zoomOut();
                break;
        }
    }

   private:
    void zoomIn() {
        for (int offset = 0; offset < matrix->rows / 2; offset += 1) {
            displaySquare(offset);
            delay(delayTime);
        }
    }

    void zoomOut() {
        for (int offset = matrix->rows / 2 - 1; offset >= 0; offset -= 1) {
            displaySquare(offset);
            delay(delayTime);
        }
    }

    void displaySquare(int offset) {
        matrix->clear();
        for (int i = 0; i < matrix->rows; i += 1) {
            for (int j = 0; j < matrix->columns; j += 1) {
                if (
                    ((i == offset || i == matrix->rows - offset - 1) 
                    || (j == offset || j == matrix->columns - offset - 1)) 
                    && i >= offset && i <= matrix->rows - offset - 1 && j >= offset 
                    && j <= matrix->columns - offset - 1) {
                    matrix->setColor(j, i, color);
                }
            }
        }
        matrix->flush();
    }
};