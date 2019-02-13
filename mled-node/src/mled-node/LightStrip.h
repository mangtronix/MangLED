#ifndef _LIGHTSTRIP_H
#define _LIGHTSTRIP_H

#include <NeoPixelBus.h>


class LightStrip
{
    public:
        LightStrip();

        // Call this in main sketch setup
        void setup();

        void ledOn(uint16_t index, float brightness);
        void ledOff(uint16_t index);

        // Hue for the pixel, 0..1., red=0
        void setHue(uint16_t index, float hue);

        // Brightness multiplier for individual pixels
        void setBrightnessMultiplier(uint16_t index, float multiplier);

        // Brightness multiplier for all pixels
        void setMasterBrightness(float brightness);

        uint16_t pixelCount() { return _pixelCount; };

        // Update internally calculated values
        void updateValues();

        // Send values to physical LED strip
        void sendToStrip();


    private:
        uint16_t _pixelCount;
        float *_hueValues;
        float *_brightnessValues;
        float *_brightnessMultipliers;
        float _masterBrightness;


};



#endif