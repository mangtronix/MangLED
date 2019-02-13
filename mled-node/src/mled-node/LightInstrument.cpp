#include "LightInstrument.h"
#include "LightStrip.h"

LightInstrument::LightInstrument(uint8_t midiChannel, LightStrip* lightStrip)
{
    _baseNote = 0;
    _baseBrightnessCC = 0;
    _baseHueCC = 0;
    _baseNote = 0;

    _midiChannel = midiChannel;
    _lightStrip = lightStrip;

    // Currently we map one pixel to one segment/controller/note
    _segmentCount = _lightStrip->pixelCount();
}

////// Event handlers //////

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
    if (shouldRespondToChannel(channel)) {
        Serial.print("LI: got note on ");
        Serial.println(note);

        segmentOn(noteToSegment(note), velocityToBrightness(velocity));
    }
}


void LightInstrument::onAppleMidiNoteOff(byte channel, byte note, byte velocity)
{
    if (shouldRespondToChannel(channel)) {
        Serial.print("LI: got note off ");
        Serial.println(note);

        segmentOff(noteToSegment(note));
    }
}


void LightInstrument::onAppleMidiControlChange(byte channel, byte controller, byte value)
{
    // Determine if the received controller was one we have mapped, and act accordingly

    if (shouldRespondToChannel(channel)) {
        Serial.print("LI: got CC ");
        Serial.print(controller);
        Serial.print(" ");

        
        ControllerType ccType = controllerType(controller);
        if (ccType == CONTROLLER_UNKNOWN) {
            Serial.println("UNKNOWN");
            return;
        }

        if (ccType == CONTROLLER_MASTER_BRIGHTNESS) {
            Serial.println("MASTER BRIGHTNESS");
            _lightStrip->setMasterBrightness(valueToMasterBrightness(value));
            return;
        }

        uint8_t segment = controllerToSegment(controller);
        if (segment >=0 && segment < _segmentCount) {
            switch (ccType) {
                case CONTROLLER_HUE:
                    Serial.println("HUE");
                    _lightStrip->setHue(segment, valueToHue(value));
                    break;

                case CONTROLLER_BRIGHTNESS:
                    Serial.println("BRIGHTNESS");
                    _lightStrip->setBrightnessMultiplier(segment, valueToBrightnessMultiplier(value));
                    break;

                default:
                    Serial.print("unhandled");
            }
        } else {
            Serial.println(" invalid segment");
        }

    }
}



///// Mapping functions //////

uint8_t LightInstrument::noteToSegment(byte note)
{
    // Map the note to our expected segments, and check if in range

    byte segment = note - _baseNote;

    if (segment < 0) {
        return -1; // Too low, unmapped
    }

    if (segment > (_segmentCount - 1)) {
        return -1; // Too high
    }

    Serial.print("LI: Note ");
    Serial.print(note);
    Serial.print(" mapped to segment ");
    Serial.println(segment);

    return segment;
}

uint8_t LightInstrument::controllerToSegment(byte controller)
{
    // Find the controller type, this already checks for valid range
    ControllerType ccType = controllerType(controller);

    uint8_t segment;

    switch (ccType) {
        case CONTROLLER_HUE:
            segment = controller - _baseHueCC;
            break;

        case CONTROLLER_BRIGHTNESS:
            segment = controller - _baseBrightnessCC;
            break;

        case CONTROLLER_MASTER_BRIGHTNESS:
            segment = controller;
            break;

        default:
            segment = -1; // Unknown

    }

    return segment;

}

// For a given Midi CC, determine if it is a mapped controller type
ControllerType LightInstrument::controllerType(byte controller)
{
    if (controller >= _baseBrightnessCC && controller < (_baseBrightnessCC + _segmentCount)) {
        return CONTROLLER_BRIGHTNESS;
    }

    if (controller >= _baseHueCC && controller < (_baseHueCC + _segmentCount)) {
        return CONTROLLER_HUE;
    }

    if (controller == _masterBrightnessCC) {
        return CONTROLLER_MASTER_BRIGHTNESS;
    }

    return CONTROLLER_UNKNOWN;
}


///// LED strip functions /////
void LightInstrument::segmentOn(uint8_t segment, float brightness)
{
    // Currently a segment corresponds directly to a single LED
    if (segment >= 0 && segment < _segmentCount) {
        _lightStrip->ledOn(segment, brightness);
    }
}

void LightInstrument::segmentOff(uint8_t segment)
{
    // Currently a segment corresponds directly to a single LED
    if (segment >= 0 && segment < _segmentCount) {
        _lightStrip->ledOff(segment);
    }
}