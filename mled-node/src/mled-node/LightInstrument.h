#ifndef _LIGHTINSTRUMENT_H
#define _LIGHTINSTRUMENT_H

#include "Arduino.h"

class LightStrip;

enum ControllerType { CONTROLLER_HUE, CONTROLLER_BRIGHTNESS, CONTROLLER_MASTER_BRIGHTNESS, CONTROLLER_UNKNOWN };

class LightInstrument
{
    public:

        // midiChannel can be <= 0 for omni response
        LightInstrument(int8_t midiChannel, LightStrip* lightStrip);

        // First note, goes to LED 0
        void setBaseNote(byte note) { _baseNote = note; };

        // First controller for hue
        void setBaseHueController(byte cc) { _baseHueCC = cc; };

        // First controller for the brightness multipliers
        void setBaseBrightnessController(byte cc) { _baseBrightnessCC = cc; };

        // Master brightness controller
        void setMasterBrightnessController(byte cc) { _masterBrightnessCC = cc; };


        // Event handlers
        void onAppleMidiConnected(uint32_t ssrc, char* name);
        void onAppleMidiDisconnected(uint32_t ssrc);
        void onAppleMidiNoteOn(byte channel, byte note, byte velocity);
        void onAppleMidiNoteOff(byte channel, byte note, byte velocity);
        void onAppleMidiControlChange(byte channel, byte controller, byte value);

        bool shouldRespondToChannel(byte channel) { return ((_midiChannel <= 0) || (channel == _midiChannel)); };


        // Update what's shown on the LED strip
        void sendToStrip();

        // Dump some useful information
        void printParameters();


        ///// MIDI Mapping

        // Midi note / controller to logical segment
        uint8_t noteToSegment(byte note);
        uint8_t controllerToSegment(byte controller);

        // For a given Midi CC, determine if it is a mapped controller type
        ControllerType controllerType(byte controller);

        // Change these functions to change the mapping of incoming midi values to
        // e.g. brightness and hue
        float velocityToBrightness(byte velocity) { return velocity / 127.; };
        float valueToHue(byte value) { return value / 127.; };
        float valueToBrightnessMultiplier(byte value) { return value / 127.; };
        float valueToMasterBrightness(byte value) { return value / 127.; };

        void segmentOn(uint8_t segment, float brightness);
        void segmentOff(uint8_t segment);

    private:
        int8_t _midiChannel;
        byte _baseNote;
        byte _baseHueCC;
        byte _baseBrightnessCC;
        byte _masterBrightnessCC;
        uint8_t _segmentCount; // count of addressable light elements

        LightStrip* _lightStrip;

};


#endif