#ifndef Sprite_hpp
#define Sprite_hpp

#include "Arduino.h"

struct Sprite {
    const uint32_t* palette;
    const uint8_t framesCount;
    const uint8_t* framesSequence;
    const uint8_t* frameData;
};

#endif /* Sprite_h */