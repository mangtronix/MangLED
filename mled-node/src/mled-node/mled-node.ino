#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "AppleMidi.h"
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);
bool gMidiIsConnected = false;

// Forward declaration of our Apple MIDI event handlers
void OnAppleMidiConnected(uint32_t ssrc, char* name);
void OnAppleMidiDisconnected(uint32_t ssrc);
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity);
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity);
void OnAppleMidiControlChange(byte channel, byte controller, byte value);

// Define your wifi SSID / pass in the external header file
#include "private_config.h"

// Manager for the LED strip
#include "LightStrip.h"
LightStrip gLightStrip;

#include "LightInstrument.h"
LightInstrument *pLightInstrument;

const char* gSSID = WIFI_SSID;
const char* gPassword = WIFI_PASSWORD;
const int gNodeNumber = NODE_NUMBER;

#define STATUS_LED LED_BUILTIN
void statusLedOff();
void statusLedOn();

String gNodeName;

void setup() {
  // Hello, world!
  pinMode(STATUS_LED, OUTPUT);
  statusLedOn();


  // Start Serial with debugging info
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Create the light instrument and attach the led strip
  gLightStrip.setup();
  gLightStrip.sendToStrip(); // Turn LED strip off
  pLightInstrument = new LightInstrument(11, &gLightStrip);
  pLightInstrument->sendToStrip(); // Initial display on the LED strip

  // A mapping for X-Touch MIDI controller
  pLightInstrument->setBaseNote(8);
  pLightInstrument->setBaseHueController(1);
  pLightInstrument->setBaseBrightnessController(11);
  pLightInstrument->setMasterBrightnessController(9);


  // Connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(gSSID, gPassword);
  Serial.print("Connecting to SSID: ");
  Serial.println(gSSID);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to SSID ");
  Serial.println(gSSID);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  // Let background processing happen
  yield();


  // Set up our mled-n.local hostname
  gNodeName = String("mled-") + String(NODE_NUMBER);
  setupBonjour();


  // Can't hurt
  yield();


  // Set up a receiving AppleMIDI session
  setupAppleMidi();


  // $$$ TODO
  // - blink the number of the node
  // - show that everything is OK
  // - add OTA update capability


  Serial.println("Finished setup, here we go!");
  statusLedOff();
}

void loop() {
  // Process Apple MIDI messages
  AppleMIDI.run();
  yield();

  MDNS.update();
  yield();

  // $$$ TODO
  // - show that we are alive, every n seconds

}


void setupBonjour() {
   // Set up mDNS responder:
  Serial.println("Setting up mDNS / Bonjour with hostname / IP:");
  Serial.print(gNodeName);
  Serial.print(".local ");
  Serial.println(WiFi.localIP());
  if (!MDNS.begin(gNodeName.c_str())) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      Serial.print(".");
      delay(1000);
      // XXX break out if not done after awhile
      // XXX what happens if name is already registered by another node?
    }
  }
  Serial.println("mDNS responder started");
}


void setupAppleMidi() {
  // Create a receiving session - the remote master needs to connect to us

  Serial.println("Starting AppleMIDI session");
  AppleMIDI.begin(gNodeName.c_str());


  // Register as an Apple MIDI receiver
  // XXX this doesn't always work, debug
  const char* service = "apple-midi";
  const char* protocol = "udp";
  int port = 5004;
  Serial.print("Adding mDNS service ");
  Serial.print(service);
  Serial.print(" ");
  Serial.print(protocol);
  Serial.print(" ");
  Serial.println(port);
  MDNS.addService(service, protocol, port);

  // Connect event handlers
  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);

  AppleMIDI.OnReceiveControlChange(OnAppleMidiControlChange);

  Serial.println("Open Audio Midi Setup on OSX, or rtpMIDI on Windows");
  Serial.print("Add device named ");
  Serial.print(gNodeName);
  Serial.println(".local:5004 and press Connect");
}


// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  gMidiIsConnected  = true;
  Serial.print(F("Connected to session "));
  Serial.println(name);

  pLightInstrument->onAppleMidiConnected(ssrc, name);
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected(uint32_t ssrc) {
  gMidiIsConnected  = false;
  Serial.println(F("Disconnected"));
  // XXX in this case, we should re-connect!
  // $$$ turn off LEDs when disconnected for "awhile"

  pLightInstrument->onAppleMidiDisconnected(ssrc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity) {
  statusLedOn();

  Serial.print(F("Incoming NoteOn from channel:"));
  Serial.print(channel);
  Serial.print(F(" note:"));
  Serial.print(note);
  Serial.print(F(" velocity:"));
  Serial.print(velocity);
  Serial.println();

  pLightInstrument->onAppleMidiNoteOn(channel, note, velocity);
  pLightInstrument->sendToStrip();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity) {
  statusLedOff();

  Serial.print(F("Incoming NoteOff from channel:"));
  Serial.print(channel);
  Serial.print(F(" note:"));
  Serial.print(note);
  Serial.print(F(" velocity:"));
  Serial.print(velocity);
  Serial.println();

  pLightInstrument->onAppleMidiNoteOff(channel, note, velocity);
  pLightInstrument->sendToStrip();

}

void OnAppleMidiControlChange(byte channel, byte controller, byte value)
{
  Serial.print(F("Got control change "));
  Serial.print(controller);
  Serial.print(F(" - "));
  Serial.print(value);
  Serial.println();

  pLightInstrument->onAppleMidiControlChange(channel, controller, value);
  pLightInstrument->sendToStrip();

}


// Utility functions
void statusLedOn()
{
  digitalWrite(STATUS_LED, LOW);
}

void statusLedOff()
{
  digitalWrite(STATUS_LED, HIGH);
}
