#ifndef Point_h
#define Point_h

#include <Arduino.h>

struct Point2D {
    int16_t x;
    int16_t y;

    Point2D(int16_t x, int16_t y);

    void addX(int16_t x);
    void addY(int16_t y);
    void addXY(int16_t x, int16_t y);

    /// set x, y to 0
    void nullify();
};

#endif /* Point_h */
