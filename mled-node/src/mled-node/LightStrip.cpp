#include "Arduino.h"
#include "LightStrip.h"

// Get the PIXEL_COUNT
#include "private_config.h"

// With our current library there can only be one strip AFAIK, so we statically allocate it
static NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> gStrip(PIXEL_COUNT);




LightStrip::LightStrip()
{
    _pixelCount = PIXEL_COUNT;
    _hueValues = new float[_pixelCount];
    _brightnessValues = new float[_pixelCount];
    _brightnessMultipliers = new float[_pixelCount];

    _masterBrightness = 1;
    for (int i = 0; i < _pixelCount; i++) {
        _hueValues[i] = 0;
        _brightnessValues[i] = 0;
        _brightnessMultipliers[i] = 1;
    }
}

void LightStrip::setup()
{
    gStrip.Begin();
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
    float saturation = 1;

    for (int i = 0; i < _pixelCount; i++) {

        /* Debug
        Serial.print("LS: Setting index ");
        Serial.print(i);
        Serial.print(" to hue ");
        Serial.print(_hueValues[i]);
        Serial.print(", brightness ");
        Serial.println(_brightnessValues[i] * _brightnessMultipliers[i] * _masterBrightness);
        */

        gStrip.SetPixelColor(
            i,
            HsbColor(_hueValues[i], saturation, _brightnessValues[i] * _brightnessMultipliers[i] * _masterBrightness)
        );
    }
}

void LightStrip::sendToStrip()
{
    gStrip.Show();
}