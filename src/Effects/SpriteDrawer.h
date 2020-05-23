
#ifndef SpriteDrawer_hpp
#define SpriteDrawer_hpp

#include "LEDMatrix.h"
#include "Sprite.h"
#include "Ticker.h"

class SpriteDrawer {
    LEDMatrix* display;
    Ticker drawTicker;
    uint16_t currentFrame;

public:
    uint16_t repeats;
    uint16_t frameDuration;
    bool isPlaying;
    Sprite* sprite;

    SpriteDrawer(LEDMatrix* display);

    void play();
    void stop();
    void next();

    // emulates Arduino's loop
    void update();

    void drawNextFrame();
private:
    void draw(const uint32_t* colorPalette, const uint8_t* colorMap);
};

#endif 