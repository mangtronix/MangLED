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

const char* gSSID = WIFI_SSID;
const char* gPassword = WIFI_PASSWORD;
const int gNodeNumber = NODE_NUMBER;

#define STATUS_LED LED_BUILTIN
void statusLedOff();
void statusLedOn();

String gNodeName;

void setup() {
  pinMode(STATUS_LED, OUTPUT);
  statusLedOn();

  Serial.begin(115200);
  Serial.println();
  Serial.println();

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

  // Set up our .local hostname
  gNodeName = String("mled-") + String(NODE_NUMBER);

  // Let background processing happen
  yield();

  // Set up our mled-n.local hostname
  setupBonjour();

  yield();

  // Set up a receiving AppleMIDI session
  setupAppleMidi();


  Serial.println("Finished setup, here we go!");
  statusLedOff();
}

void loop() {
  // Process Apple MIDI messages
  AppleMIDI.run();

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

  // Connect event handlers
  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);

  // AppleMIDI.OnReceiveControlChange(OnAppleMidiControlChange);

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
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected(uint32_t ssrc) {
  gMidiIsConnected  = false;
  Serial.println(F("Disconnected"));
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