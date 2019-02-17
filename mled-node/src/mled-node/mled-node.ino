#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

// -- For Wemos D1 Mini --
// Connect NeoPixel pins to D1 Mini pins
//   +5V  -> 5V
//    Din -> RX
//    GND -> GND
//
// 2019-02-14 Tested and working with 5 LED strip, powered USB hub

// Define your wifi SSID / pass in the external header file
#include "private_config.h"
const char* gSSID = WIFI_SSID;
const char* gPassword = WIFI_PASSWORD;
const int gNodeNumber = NODE_NUMBER;


// Apple MIDI library allows us to connect to MIDI network sessions
// Server is builtin to OSX (Audio Midi Setup)
// On Windows you need to install rtpMIDI
#include "AppleMidi.h"
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);
bool gMidiIsConnected = false;

// Forward declaration of our Apple MIDI event handlers
void OnAppleMidiConnected(uint32_t ssrc, char* name);
void OnAppleMidiDisconnected(uint32_t ssrc);
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity);
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity);
void OnAppleMidiControlChange(byte channel, byte controller, byte value);



// Manager for the LED strip - talks directly to the NeoPixels
#include "LightStrip.h"
LightStrip gLightStrip;

// Our "light instrument" that responds to MIDI data and decides
// what to send to the LightStrip
#include "LightInstrument.h"
LightInstrument *pLightInstrument;


// LED showing basic status (connecting, MIDI data received)
#define STATUS_LED LED_BUILTIN
void statusLedOff();
void statusLedOn();


// The name of our node, that we will register with mDNS / Bonjour
// e.g. mled-7.local
String gNodeName;


void setup() {
  // Hello, world! Turn the light on.
  pinMode(STATUS_LED, OUTPUT);
  statusLedOn();


  // Start Serial with debugging info
  Serial.begin(115200);
  Serial.println();
  Serial.println();


  // Create the light instrument and attach the led strip
  gLightStrip.setup();
  gLightStrip.sendToStrip(); // Turn LED strip off
  pLightInstrument = new LightInstrument(MLED_CHANNEL, &gLightStrip);
  pLightInstrument->sendToStrip(); // Initial display on the LED strip

  // Set up the mapping of MIDI controllers and notes
  // Set these in the private_config.h file
  pLightInstrument->setBaseNote(MLED_BASE_NOTE);
  pLightInstrument->setBaseHueController(MLED_BASE_HUE_CC);
  pLightInstrument->setBaseBrightnessController(MLED_BASE_BRIGHTNESS_CC);
  pLightInstrument->setMasterBrightnessController(MLED_MASTER_BRIGHTNESS_CC);


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
  yield();


  // Accept over the air updates
  setupOTA();
  yield();


  // Set up a receiving AppleMIDI session
  setupAppleMidi();


  // Print out what we're listening for
  pLightInstrument->printParameters();


  // $$$ TODO
  // - blink the number of the node
  // - show that everything is OK
  // - add OTA update capability


  Serial.println("Finished setup, here we go!");
  statusLedOff();
}


void loop() {
  // Process any over the air update
  ArduinoOTA.handle();
  yield();


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


void setupOTA() {
  // XXX no password!!
  Serial.println("Setting up OTA updates"); 

  ArduinoOTA.setHostname(gNodeName.c_str());

  // Set the password for OTA updates in private_config.h
  //ArduinoOTA.setPasswordHash(OTA_PASSWORD_HASH);
  ArduinoOTA.setPassword((const char*)"superinsecure");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
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
