#include "Point.h"

Point2D::Point2D(int16_t x, int16_t y): x(x), y(y) {}

void Point2D::addX(int16_t x) {
    this->x += x;
}

void Point2D::addY(int16_t y) {
    this->y += y;
}

void Point2D::addXY(int16_t x, int16_t y) {
    this->x += x;
    this->y += y;
}

void Point2D::nullify() {
    x = 0;
    y = 0;
}
