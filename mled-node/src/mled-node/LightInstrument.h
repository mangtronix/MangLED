#ifndef _LIGHTINSTRUMENT_H
#define _LIGHTINSTRUMENT_H

#include "Arduino.h"

class LightStrip;

class LightInstrument
{
    public:
        LightInstrument(uint8_t midiChannel, LightStrip* lightStrip);


        // First note, goes to LED 0
        void setBaseNote(uint8_t note) { _baseNote = note; };

        // First controller for hue
        void setBaseHueController(uint8_t cc) { _baseHueCC = cc; };

        // First controller for the brightness multipliers
        void setBaseBrightnessController(uint8_t cc) { _baseBrightnessCC = cc; };

        // Master brightness controller
        void setMasterBrightnessController(uint8_t cc) { _masterBrightnessCC = cc; };


        // Event handlers
        void onAppleMidiConnected(uint32_t ssrc, char* name);
        void onAppleMidiDisconnected(uint32_t ssrc);
        void onAppleMidiNoteOn(byte channel, byte note, byte velocity);
        void onAppleMidiNoteOff(byte channel, byte note, byte velocity);
        void onAppleMidiControlChange(byte channel, byte controller, byte value);


    private:
        uint8_t _baseNote;
        uint8_t _baseHueCC;
        uint8_t _baseBrightnessCC;
        uint8_t _masterBrightnessCC;

        LightStrip* _lightStrip;

};


#endif