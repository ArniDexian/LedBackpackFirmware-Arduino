#include "FontDrawer.h"
#include "../Assets/fonts.h"
#include <Arduino.h>

#define WHITESPACE_WIDTH 1
#define SPACE_X_OFFSET 1
#define SPACE_Y_OFFSET 1
#define SCROLL_Y_OFFSET -1
#define SCROLL_X_OFFSET -1
#define SCROLL_MIN_DELAY 200
#define SCROLL_EXTRA_DELAY 1000

// MARK: - callback actions

void scrollAction(void* object) {
    ((FontDrawer*)object)->autoscroll();
}

void scrollTopAction(void* object) {
    ((FontDrawer*)object)->autoscrollTop();
}

// MARK: - implementation

FontDrawer::FontDrawer(LEDMatrix* display)
: display(display), scrollTicker(scrollAction, 1, 0, MILLIS), offset(0, 0), drawPosition(0, 0),
contentSize(0, 0), lineBreakMode(TextLineBreakModeScrollVertically), color(0xFF0000) {
    scrollTicker.userInfo = this;
}

void FontDrawer::drawText(const char* _text) {
    this->text = _text;
    drawText();

    delayScrollIfNeeded();
}

void FontDrawer::drawText() {
    if (!text) { return; }

    const uint8_t* fontData = font5x5;

    uint8_t height = pgm_read_byte(&(fontData[0]));
    uint8_t width = pgm_read_byte(&(fontData[1]));
    uint16_t bytesPerGlyph = (width * height + 8 - 1) / 8;
    uint16_t glyphsCount = pgm_read_byte(&(fontData[2]));

    //iterate over given symbols
    for(const char *it = text; *it; it++) {
        const uint8_t* fontBlobBegin = &(fontData[3]);

        if (*it == '\n') {
            moveCursorToNewLine(height, SPACE_Y_OFFSET);
        }

        if (*it == ' ') {
            drawPosition.addX(WHITESPACE_WIDTH);
        }

        //iterate over available glyphs in the font
        for (uint16_t i = 0; i < glyphsCount; i += 1) {
            // every glyph takes 1 byte per ANSII symb + bytesPerGlyph
            const uint8_t* glyphBeginPtr = &fontBlobBegin[i * (bytesPerGlyph + 1)];
            char glyphChar = pgm_read_byte(glyphBeginPtr);

            if (glyphChar == *it) {
                drawGlyph(glyphBeginPtr, width, height);

                // as we found our glyph stop to iterate over all glyphs
                break;
            }
        }
    }

    //flush our word
    display->flush();
}

void FontDrawer::drawGlyph(const uint8_t *glyphPtr, uint8_t width, uint8_t height) {
    uint8_t currentByte = '\0';
    uint8_t mostRightTrueBit = 0;
    uint8_t bitsPerGlyph = width * height;

    switch (lineBreakMode) {
        case TextLineBreakModeClipping: break;
        case TextLineBreakModeScrollHorizontally:
            break;

        case TextLineBreakModeCharWrapping:
        case TextLineBreakModeScrollVertically:
            uint8_t glyphWidth = FontDrawer::getGlyphWidth(glyphPtr, width, height);
            if (drawPosition.x + glyphWidth > display->columns) {
                moveCursorToNewLine(height, SPACE_Y_OFFSET);
            }

            // don't waste cycles on drawing invisible part
            if ((drawPosition.y + height + offset.y < 0)
                || (drawPosition.y + offset.y > display->rows)) {
                drawPosition.addX(glyphWidth + SPACE_X_OFFSET);
                return;
            }
            break;
    }

    for (uint8_t iBit = 0, iByte = 0; iBit < bitsPerGlyph; iBit += 1) {
        uint8_t bytesBit = iBit % 8;
        if (bytesBit == 0) {
            currentByte = pgm_read_byte((const uint8_t*)(glyphPtr + 1 + iByte++));
        }
        bool value = currentByte >> (7 - bytesBit) & 1;
        if (!value) {
            continue;
        }

        uint8_t bitPosX = iBit % width;
        mostRightTrueBit = max(mostRightTrueBit, bitPosX);

        // data represented top down, but screen draws bottom up
        // as it's most convenient display text naturally i.e. top down
        // calculate that's coordinate and then transform to screen draws:
        uint16_t displayPositionX = drawPosition.x + bitPosX + offset.x;
        if (displayPositionX >= display->columns) {
            continue;
        }

        uint8_t bitPosY = iBit / width;
        // here we transform to screen coordinates
        uint16_t displayPositionY = display->rows - (drawPosition.y + bitPosY + offset.y) - 1;
        if (displayPositionY >= display->rows) {
            continue;
        }

        display->setColor(displayPositionX, displayPositionY, color);
    }
    // move cursor to the next symbol with SPACE_X_OFFSET pixel space
    drawPosition.addX(mostRightTrueBit + 1 + SPACE_X_OFFSET);
    contentSize.x = max(contentSize.x, drawPosition.x - SPACE_X_OFFSET);
}

void FontDrawer::moveCursorToNewLine(uint8_t lineHeight, uint8_t interlineSpace) {
    drawPosition.addY(lineHeight + interlineSpace);
    drawPosition.x = 0;
    contentSize.y = drawPosition.y + lineHeight;
}

void FontDrawer::setToBegin() {
    drawPosition.nullify();
    contentSize.nullify();
    display->clear(false);
}

// MARK: - Scrolling logic

void FontDrawer::update() {
    scrollTicker.update();
}

bool FontDrawer::isScrollAtBeginning() {
    return offset.x == 0 && offset.y == 0;
}

bool FontDrawer::isScrollAtEnd() {
    switch (lineBreakMode) {
        case TextLineBreakModeScrollVertically:
            return offset.y + contentSize.y <= display->rows;
        case TextLineBreakModeScrollHorizontally:
            return offset.x + contentSize.x <= display->columns;
        default: return false;
    }
}

void FontDrawer::autoscroll() {
    switch (lineBreakMode) {
        case TextLineBreakModeScrollVertically: {
            // scroll down
            if (offset.y + SCROLL_Y_OFFSET + contentSize.y >= display->rows) {
                offset.y += SCROLL_Y_OFFSET;

                setToBegin();
                drawText();

                if (isScrollAtEnd()) {
                    delayScrollTop();
                } else {
                    delayScroll(false);
                }
            }
        }
            break;

        case TextLineBreakModeScrollHorizontally: {
            if (offset.x + SCROLL_X_OFFSET + contentSize.x >= display->columns) {
                offset.x += SCROLL_X_OFFSET;

                setToBegin();
                drawText();

                if (isScrollAtEnd()) {
                    delayScrollTop();
                } else {
                    delayScroll(false);
                }
            }
        }
            break;
        default: break;
    }
}

void FontDrawer::autoscrollTop() {
    switch (lineBreakMode) {
        case TextLineBreakModeScrollVertically:
            offset.y = 0;
            break;
        case TextLineBreakModeScrollHorizontally:
            offset.x = 0;
            break;
        default: break;
    }

    setToBegin();
    drawText();
    delayScrollIfNeeded();
}

// MARK: - scrolling delay* functions

void FontDrawer::delayScrollIfNeeded() {
    if (lineBreakMode == TextLineBreakModeScrollVertically
        || lineBreakMode == TextLineBreakModeScrollHorizontally) {
        delayScroll(isScrollAtBeginning());
    }
}

void FontDrawer::delayScroll(bool extraDelayed) {
    uint32_t interval = extraDelayed ? SCROLL_EXTRA_DELAY : SCROLL_MIN_DELAY;
    scrollTicker.setCallback(scrollAction);
    scrollTicker.fireOnce(interval);
}

void FontDrawer::delayScrollTop() {
    scrollTicker.setCallback(scrollTopAction);
    scrollTicker.fireOnce(SCROLL_EXTRA_DELAY);
}

// MARK: - helper static functions

/// Get actual glyph width
/// @param glyphPtr pointer to the glyph
/// @param width max width preserved for the glyph
/// @param height max height preserved for the glyph
uint8_t FontDrawer::getGlyphWidth(const uint8_t *glyphPtr, uint8_t width, uint8_t height) {
    uint8_t glyphWidth = 0;
    uint8_t bitsPerGlyph = width * height;

    // TODO: - extract to iterate funciton with function pointer executable block
    for (uint8_t iBit = 0, iByte = 0, currentByte = '\0'; iBit < bitsPerGlyph; iBit += 1) {
        uint8_t bytesBit = iBit % 8;
        if (bytesBit == 0) {
            currentByte = pgm_read_byte((const uint8_t*)(glyphPtr + 1 + iByte++));
        }
        bool value = currentByte >> (7 - bytesBit) & 1;
        if (!value) { continue; }

        uint8_t bitPosX = iBit % width;
        glyphWidth = max(glyphWidth, bitPosX + 1);
    }

    return glyphWidth;
}
