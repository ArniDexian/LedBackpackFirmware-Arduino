#ifndef FontDrawer_hpp
#define FontDrawer_hpp

#include "LEDMatrix.h"
#include "Ticker.h"
#include "Point.h"

enum TextLineBreakMode {
    TextLineBreakModeClipping,
    TextLineBreakModeCharWrapping,
    TextLineBreakModeScrollVertically,
    TextLineBreakModeScrollHorizontally,
};

// In progress so far
// struct AttributedText {
//     struct ColoredString {
//         const char* text;
//         const uint32_t color;
//     };
//     const ColoredString* string;
//     const uint16_t count;
// };

class FontDrawer {
    LEDMatrix* display;
    Ticker scrollTicker;
    Point2D offset;
    Point2D drawPosition;
    Point2D contentSize;
    // text being passed to draw function
    const char* text;
public:
    TextLineBreakMode lineBreakMode;
    uint32_t color;

    FontDrawer(LEDMatrix* display);

    void drawText(const char* text);
    void setToBegin();

    // Perform delta scrolling according lineBreakMode, intended for private usage
    void autoscroll();
    // Perform scrolling to top, intended for private usage
    void autoscrollTop();
    // need to call from Arduino's func loop for autoscrolling
    void update();

private:
    void drawText();
    void drawGlyph(const uint8_t *glyphPtr, uint8_t width, uint8_t height);
    void moveCursorToNewLine(uint8_t lineHeight, uint8_t interlineSpace);

    void delayScrollIfNeeded();
    void delayScroll(bool extraDelayed = false);
    void delayScrollTop();
    bool isScrollAtBeginning();
    bool isScrollAtEnd();

    static uint8_t getGlyphWidth(const uint8_t *glyphPtr, uint8_t width, uint8_t height);
};

#endif /* FontDrawer_hpp */
