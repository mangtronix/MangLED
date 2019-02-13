#include "LightInstrument.h"
#include "LightStrip.h"

LightInstrument::LightInstrument(uint8_t midiChannel, LightStrip* lightStrip)
{
    _baseNote = 0;
    _baseBrightnessCC = 0;
    _baseHueCC = 0;
    _baseNote = 0;

    _lightStrip = lightStrip;
}

// Event handlers
void LightInstrument::onAppleMidiConnected(uint32_t ssrc, char* name)
{
   Serial.println("LI: got connect");
}

void LightInstrument::onAppleMidiDisconnected(uint32_t ssrc)
{
   Serial.println("LI: got disconnect");
}


void LightInstrument::onAppleMidiNoteOn(byte channel, byte note, byte velocity)
{
   Serial.println("LI: got note on");
}


void LightInstrument::onAppleMidiNoteOff(byte channel, byte note, byte velocity)
{
   Serial.println("LI: got note off");
}


void LightInstrument::onAppleMidiControlChange(byte channel, byte controller, byte value)
{
    Serial.println("LI: got CC");
}