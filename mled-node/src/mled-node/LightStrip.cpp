#include "Arduino.h"
#include "LightStrip.h"

// XXX hack to make the brightness knobs set the default brightness
//     notes will temporarily override
#define INDEPENDENT_BRIGHTNESS_CC


LightStrip::LightStrip()
{
    _pixelCount = LED_COUNT;
    _hueValues = new float[_pixelCount];
    _brightnessValues = new float[_pixelCount];
    _brightnessMultipliers = new float[_pixelCount];

    _masterBrightness = 1;
    for (int i = 0; i < _pixelCount; i++) {
        _hueValues[i] = 0;
        _brightnessValues[i] = 0;

#ifdef INDEPENDENT_BRIGHTNESS_CC
        // Set the "baseline" brightness to 0 (off) until it gets set
        // via a CC value
        _brightnessMultipliers[i] = 0;
#else
        // Set the multiplier to 1 (allow full brightness from note) until it gets reduced by a CC
        _brightnessMultipliers[i] = 1;
#endif

    }
}

void LightStrip::setup()
{
    Serial.print("LS: Setting up with LED_PIN(");
    Serial.print(LED_PIN);
    Serial.print(") LED_COUNT(");
    Serial.print(LED_COUNT);
    Serial.println(")");
    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(_leds, LED_COUNT).setCorrection( TypicalLEDStrip );
    FastLED.show();
}

void LightStrip::ledOn(uint16_t index, float brightness)
{
    Serial.print("LS: Turning on ");
    Serial.println(index);

    _brightnessValues[index] = brightness;
}

void LightStrip::ledOff(uint16_t index)
{
    Serial.print("LS: Turning off ");
    Serial.println(index);

    _brightnessValues[index] = 0;
}

// Hue for the pixel, 0..1., red=0
void LightStrip::setHue(uint16_t index, float hue)
{
    _hueValues[index] = hue;
}

// Brightness multiplier for individual pixels
void LightStrip::setBrightnessMultiplier(uint16_t index, float multiplier)
{
    _brightnessMultipliers[index] = multiplier;
}

// Brightness multiplier for all pixels
void LightStrip::setMasterBrightness(float brightness)
{
    _masterBrightness = brightness;
}

// Set pixel colors in internal buffer
void LightStrip::updateValues()
{
    uint8_t saturation = 255;

    for (int i = 0; i < _pixelCount; i++) {

        /* Debug
        Serial.print("LS: Setting index ");
        Serial.print(i);
        Serial.print(" to hue ");
        Serial.print(_hueValues[i]);
        Serial.print(", brightness ");
        Serial.println(_brightnessValues[i] * _brightnessMultipliers[i] * _masterBrightness);
        */

#ifdef INDEPENDENT_BRIGHTNESS_CC
        uint16_t brightness = (_brightnessValues[i] + _brightnessMultipliers[i]) * _masterBrightness * 255;
       _leds[i] = CHSV( _hueValues[i] * 255, saturation, brightness);
#else
       // FastLED uses 0-255 values for HSV
#endif

    }
}

void LightStrip::sendToStrip()
{
    updateValues();
    FastLED.show();
}