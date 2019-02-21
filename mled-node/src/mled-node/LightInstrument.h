#ifndef _LIGHTINSTRUMENT_H
#define _LIGHTINSTRUMENT_H

// The LightInstrument responds to MIDI data and drives an LED strip.
// You send notes and control change messages to the LightInstrument
// and it keeps track of what brightnesses the LEDs should be set to.
//
// LightInstrument:
// - handles mapping of notes to LED strip segments
// - adjusts hue of each segment
// - the brigntness of each segment is the result of
//   - velocity of the note that triggered the segment
//   - the brightness "multiplier" for the segment that sets the maximum brightness of the segment
//   - the master brightness (for all segments)

// TODO
// - allow setting white (via saturation?)
// - fading for on/on
//   - requires periodic update, with e.g. ADSR
// - set all segments via one parameter?
// - toggle segment on/off
//   - then can control brightness directly via controller, no notes
// - RGB control mode?

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

        // The number of individually controllable LED segments
        uint16_t segmentCount() { return _segmentCount; };


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
        uint16_t _segmentCount; // count of addressable light elements
        uint16_t _pixelsPerSegment; // number of physical pixels per segment

        LightStrip* _lightStrip;


};


#endif