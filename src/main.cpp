#include <Arduino.h>
#include <OneButton.h>

#define LEDS_COUNT 144
#define LEDS_DATA_PIN 3
#define BUTTON_1_PIN 2
#define BUTTON_2_PIN 4

#include "Effects/EffectSquareZoomInOut.h"
#include "LEDMatrix.h"
#include "Assets/sprites.h"
#include "Effects/SpriteDrawer.h"
#include "Effects/FontDrawer.h"

enum Mode {
    ModeIdle,
    ModeSprite,
    ModeText
};

void nextSpriteAction();
void prevSpriteAction();
void displaySpriteAction();
void displayTextAction();
void brightnessUp();
void brightnessDown();

const uint8_t ROWS_COUNT = 12;
const uint8_t COLUMN_LEDS = 12;
const uint8_t LED_BRIGHTNESS = 16;

LEDMatrix matrix(ROWS_COUNT, COLUMN_LEDS, LED_BRIGHTNESS);
SpriteDrawer sDrawer(&matrix);
FontDrawer fDrawer(&matrix);
OneButton button1 = OneButton( BUTTON_1_PIN, HIGH, true);
OneButton button2 = OneButton(BUTTON_2_PIN, HIGH, true);

unsigned long lastTimeLaps = 0;
Mode currentMode = ModeIdle;

int spriteInd = 0;
Sprite sprites[] = {spriteMshrm, spriteGoose_6Col, spriteCoin};

void setup() {
    Serial.begin(9600);
    Serial.println("Setup");

    EffectSquareZoomInOut(&matrix,
                          EffectSquareZoomInOut::Type::ZoomIn,
                          CRGB::Green,
                          100)
        .run();
    matrix.clear();

    fDrawer.color = 0xFFFF00;
    fDrawer.drawText("START");
    currentMode = ModeText;

    button1.attachDoubleClick(prevSpriteAction);
    button1.attachClick(displaySpriteAction);
    button1.attachLongPressStart(brightnessDown);

    button2.attachDoubleClick(nextSpriteAction);
    button2.attachClick(displayTextAction);
    button2.attachLongPressStart(brightnessUp);
}

void loop() {
    matrix.setColor(0, 0, 0xFF0000);

    Serial.println("loop tick");
    button1.tick();
    button2.tick();
    sDrawer.update();

    switch (currentMode) {
        case ModeSprite:
        if (!sDrawer.isPlaying || millis() - lastTimeLaps > 8000) {
            displayTextAction();
        }
        break;

        case ModeText:
        if (millis() - lastTimeLaps > 5000) {
            displaySpriteAction();
        }   
        break;

        default: break;
    }
}

void displaySpriteAction() {
    lastTimeLaps = millis();
    currentMode = ModeSprite;
    sDrawer.sprite = &sprites[spriteInd];
    sDrawer.play();
}

void displayTextAction() {
    lastTimeLaps = millis();
    currentMode = ModeText;

    sDrawer.stop();

    fDrawer.setToBegin();
    fDrawer.color = 0x80FF00;
    fDrawer.drawText((const char*)"JA");
    fDrawer.color = 0xff3e3e;
    fDrawer.drawText((const char*)"KOTA");
}

void brightnessUp() {
    matrix.brightness = min(matrix.brightness * 2, 255);
    matrix.flush();
}

void brightnessDown() {
    matrix.brightness = max(matrix.brightness / 2, 4);
    matrix.flush();
}

void nextSpriteAction() {
    spriteInd++;
    if(spriteInd >= 3) { spriteInd = 0; }
    displaySpriteAction();
}

void prevSpriteAction() {
    spriteInd--;
    if(spriteInd < 0) { spriteInd = 2; }
    displaySpriteAction();
}